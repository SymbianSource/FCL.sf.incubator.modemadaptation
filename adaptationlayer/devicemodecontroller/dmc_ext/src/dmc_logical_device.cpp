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
*     Implementation of DMC logical device.
*
*/


// INCLUDE FILES 
#include <kern_priv.h>                   // class DThread
#include "dmc_extension.h"               // iDmcDfcQueuePtrv
#include "dmc_user_if.h"                 // RDmc
#include "dmc_logical_channel.h"
#include "dmc_logical_device.h"
#include "dmc_event_handler_ape_cent.h"  // HandleUserEvent

#include "dmc_trace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dmc_logical_deviceTraces.h"
#endif

// EXTERNAL DATA STRUCTURES 
// EXTERNAL FUNCTION PROTOTYPES 
// FORWARD DECLARATIONS
// CONSTANTS
// MACROS

// LOCAL CONSTANTS AND MACROS
#define LOG_CHNL_TBL_GRANULARITY      2  // Define smaller granularity than default
                                         // value(4) because we haven't identified more
                                         // than 2 clients so far.
#define DFC_OBJECT_PRIORITY           1  // priority for DFC object, 1 is default
_LIT(KDMCLogDevDfc, "DMCLogDevDFC");     // Name given to DFC dynamic queue

// MODULE DATA STRUCTURES  
// LOCAL FUNCTION PROTOTYPES  


// ==================== LOCAL FUNCTIONS ====================

// -----------------------------------------------------------------------------
// DLL entry point
// DLL export number 1: Create a new LDD factory on the Kernel heap
// Return Values:  A new Kernel side "factory" object
// -----------------------------------------------------------------------------
DECLARE_EXTENSION_LDD()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DECLARE_STANDARD_LDD_ENTRY,
              "DMC:LOG_DEVICE: DECLARE_EXTENSION_LDD() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: DECLARE_EXTENSION_LDD() # IN"));

    DDmcLogicalDevice* dmcLogDevPtr = new DDmcLogicalDevice;

    DMC_TRACE_ASSERT_RESET(dmcLogDevPtr,
                           "DMC:LOG_DEVICE: DMC create DECLARE_STANDARD_LDD() failed",
                           KErrNoMemory);

    OstTrace1(TRACE_ENTRY_EXIT, DECLARE_STANDARD_LDD_RETURN,
              "DECLARE_EXTENSION_LDD() # OUT - dmcLogDevPtr: 0x%x", (TUint)dmcLogDevPtr);
    DMC_TRACE((("DMC:LOG_DEVICE: DECLARE_EXTENSION_LDD() # OUT, dmcLogDevPtr: 0x%x"),
                 dmcLogDevPtr));

    return dmcLogDevPtr;
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

// ================= MEMBER FUNCTIONS =======================

//-----------------------------------------------------------
//  DDmcLogicalDevice::DDmcLogicalDevice
//-----------------------------------------------------------
DDmcLogicalDevice::DDmcLogicalDevice()
    : iLogChannelsPtr(LOG_CHNL_TBL_GRANULARITY)
    , iDmcLogDeviceDfcQueuePtr(NULL)
    , iUserEventDfcPtr(NULL)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE__ENTRY,
              "DMC:LOG_DEVICE: DDmcLogicalDevice() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: DDmcLogicalDevice() # IN"));

     // No PDD, so set to zero
    iParseMask = 0;
    iUnitsMask = 0;

    TInt dfcQueueDmcPriority = (Kern::DfcQue0()->iThread->iPriority) + 1;

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_1,
              "DMC:LOG_DEVICE: DDmcLogicalDevice() - dfcQueueDmcPriority: %d",
               dfcQueueDmcPriority);
    DMC_TRACE((("DMC:LOG_DEVICE: DDmcLogicalDevice() - dfcQueueDmcPriority: %d"),
                 dfcQueueDmcPriority));

     // DFC queue must be created in the critical section 
    NKern::ThreadEnterCS();

    TInt retVal = Kern::DynamicDfcQCreate(iDmcLogDeviceDfcQueuePtr,
                                          dfcQueueDmcPriority,
                                          KDMCLogDevDfc);
    NKern::ThreadLeaveCS();

    OstTrace0(TRACE_FLOW, DDMC_LOGICAL_DEVICE_2,
              "DMC:LOG_DEVICE: DDmcLogicalDevice() - Create iUserEventDfcPtr");
    DMC_TRACE(("DMC:LOG_DEVICE: DDmcLogicalDevice() - Create iUserEventDfcPtr"));

    iUserEventDfcPtr = new TDfc(ReceiveEventsDfc,
                                this,
                                iDmcLogDeviceDfcQueuePtr,
                                DFC_OBJECT_PRIORITY);

    DMC_TRACE_ASSERT_RESET(iUserEventDfcPtr,
                           "DMC:LOG_DEVICE: DDmcLogicalDevice() - iUserEventDfcPtr",
                           KErrNoMemory);

    // Assign the TDfc-queue to the event handler used for event sending.
    DmcEvHandApeCent::SubscribeEvents(DMC_USER_BIT, iUserEventDfcPtr);


    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_3,
              "DMC:LOG_DEVICE: DDmcLogicalDevice() - return, retVal: %d", retVal);
    DMC_TRACE((("DMC:LOG_DEVICE: DDmcLogicalDevice() - DynamicDfcQCreate, retVal: %d "), retVal));
    DMC_TRACE_ASSERT_RESET(retVal == KErrNone , "Kern::DynamicDfcQCreate() failed", retVal);

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_ENTRY__RETURN,
              "DMC:LOG_DEVICE: DDmcLogicalDevice()  # OUT");
    DMC_TRACE(("DMC:LOG_DEVICE: DDmcLogicalDevice() # OUT"));
    }


//-----------------------------------------------------------
//  DDmcLogicalDevice::~DDmcLogicalDevice
//-----------------------------------------------------------
DDmcLogicalDevice::~DDmcLogicalDevice()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_ENTRY,
              "DMC:LOG_DEVICE: ~DDmcLogicalDevice # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: ~DDmcLogicalDevice # IN"));

    OstTrace0(TRACE_FLOW, DDMC_LOGICAL_DEVICE_DDMC_LOGICAL_DEVICE_1,
              "DMC:LOG_DEVICE: ~DDmcLogicalChannel() - Close RPointerArray");
    DMC_TRACE(("DMC:LOG_DEVICE: ~DDmcLogicalChannel() - Close RPointerArray"));

    iLogChannelsPtr.Close();

    // Delete the TDfc-object owned by this class.
    delete iUserEventDfcPtr;
    iUserEventDfcPtr = NULL;

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_DDMC_LOGICAL_DEVICE_2,
              "DMC:LOG_DEVICE: ~DDmcLogicalChannel() - iUserEventDfcPtr: 0x%x:", iUserEventDfcPtr);
    DMC_TRACE((("DMC:LOG_DEVICE: ~DDmcLogicalChannel() - iUserEventDfcPtr: 0x%x"), iUserEventDfcPtr));

    // Set the NULL pointer to make sure that no more events are sent.
    DmcEvHandApeCent::UnsubscribeEvents(iUserEventDfcPtr);

    if (iDmcLogDeviceDfcQueuePtr)
        {
        OstTrace0(TRACE_FLOW, DDMC_LOGICAL_DEVICE_DDMC_LOGICAL_DEVICE_3,
                  "DMC:LOGIC_CHNL: ~DDmcLogicalChannel() - call iDmcLogChannelDfcQueuePtr->Destroy");
        DMC_TRACE(("DMC:LOGIC_CHNL: ~DDmcLogicalChannel() - call iDmcLogChannelDfcQueuePtr->Destroy"));

        // Calling of dfc queue destroy must be in a critical section.
        NKern::ThreadEnterCS();
        iDmcLogDeviceDfcQueuePtr->Destroy();
        NKern::ThreadLeaveCS();

        iDmcLogDeviceDfcQueuePtr = NULL;
        }

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_ENTRY_RETURN,
              "DMC:LOG_DEVICE: ~DDmcLogicalDevice # OUT");
    DMC_TRACE(("DMC:LOG_DEVICE: ~DDmcLogicalDevice # OUT"));
    }


//-----------------------------------------------------------
//  DDmcLogicalDevice::Install
//-----------------------------------------------------------
TInt DDmcLogicalDevice::Install()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_INSTALL_ENTRY,
              "DMC:LOG_DEVICE: Install() # IN" );
    DMC_TRACE(("DMC:LOG_DEVICE: Install() # IN"));

    // Name of the logical driver
    TInt retStatus = SetName(&KDmcDrvName);

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_INSTALL_RETURN,
              "DMC:LOG_DEVICE: Install() # OUT retStatus: %d:", retStatus);
    DMC_TRACE((("DMC:LOG_DEVICE: Install() # OUT - retStatus: %d"), retStatus));

    return retStatus;
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::GetCaps
// -----------------------------------------------------------------------------
void DDmcLogicalDevice::GetCaps(TDes8& /* aDes */) const
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_GET_CAPS_ENTRY,
              "DMC:LOG_DEVICE: GetCaps() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: GetCaps() # IN"));


    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_GET_CAPS_RETURN,
              "DMC:LOG_DEVICE: GetCaps() # OUT");
    DMC_TRACE(("DMC:LOG_DEVICE: GetCaps() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::Create
// -----------------------------------------------------------------------------
TInt DDmcLogicalDevice::Create(DLogicalChannelBase*& aChannel)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_CREATE_ENTRY,
              "DMC:LOG_DEVICE: Create() # IN - aChannel: 0x%x",
              (TUint)(aChannel));
    DMC_TRACE((("DMC:LOG_DEVICE: Create() # IN - aChannel: 0x%x"), aChannel));

    TInt retVal(KErrNone);
    
    aChannel = new DDmcLogicalChannel(this);

    if (!aChannel)
        {
        retVal = KErrNoMemory;
        }

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_CREATE_RETURN,
              "DMC:LOG_DEVICE: Create() # OUT, retVal: %d", retVal);
    DMC_TRACE((("DMC:LOG_DEVICE: Create() # OUT, retVal: %d"), retVal));

    return retVal;
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::GetTargetStartUpMode
// -----------------------------------------------------------------------------
TInt DDmcLogicalDevice::GetTargetStartUpMode(RDmc::TTargetStartupMode& aTargetStartUpmode) const
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_GET_TARGET_STARTUP_MODE_ENTRY,
              "DMC:LOG_DEVICE: GetTargetStartUpMode() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: GetTargetStartUpMode() # IN"));

    TUint bootReasonApiStartUpMode;

    // Get the boot reason API target startup mode    
    (void)DmcEvHandApeCent::HandleEvent(USER_GET_TARGET_STARTUP_MODE,
                                        NULL/*Kernel*/,
                                        &bootReasonApiStartUpMode);

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_GET_TARGET_STARTUP_MODE_1,
                 "DMC:LOGIC_CHNL: GetTargetStartUpMode() - *bootReasonApiStartUpMode: 0x%x",
                 bootReasonApiStartUpMode);
    DMC_TRACE((("DMC:LOG_DEVICE: GetTargetStartUpMode() - *bootReasonApiStartUpMode: 0x%x"),
                 bootReasonApiStartUpMode));

    // Convert the Boot Reason API mode to the user side target startup mode.
    switch (bootReasonApiStartUpMode)
        {
        case TBootReason::EStartupModeNone:
            {
            aTargetStartUpmode = RDmc::EStartupModeNone;
            }
            break;

        case TBootReason::EStartupModeNormal:
            {
            aTargetStartUpmode = RDmc::EStartupModeNormal;
            }
            break;

        case TBootReason::EStartupModeCharging:
            {
            aTargetStartUpmode = RDmc::EStartupModeCharging;
            }
            break;

        case TBootReason::EStartupModeAlarm:
            {
            aTargetStartUpmode = RDmc::EStartupModeAlarm;
            }
            break;

        case TBootReason::EStartupModeTest:
            {
            aTargetStartUpmode = RDmc::EStartupModeTest;
            }
            break;

        case TBootReason::EStartupModeFota:
            {
            aTargetStartUpmode = RDmc::EStartupModeFota;
            }
            break;

        default:
            {
            // A fatal error, make a reset
            OstTrace0(TRACE_FLOW, DDMC_LOGICAL_DEVICE_GET_TARGET_STARTUP_MODE_5,
                      "DMC:EH_APE_CEN: GetTargetStartUpMode() - bootReasonApiStartUpMode");
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: GetTargetStartUpMode() - bootReasonApiStartUpMode",
                                           KErrArgument);
            }
            break;
        }

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_GET_TARGET_STARTUP_MODE_RETURN,
             "DMC:LOG_DEVICE: GetTargetStartUpMode() # OUT - RDmc::aTargetStartUpmode: 0x%x",
             aTargetStartUpmode);
    DMC_TRACE((("DMC:LOG_DEVICE: GetTargetStartUpMode() # OUT - RDmc::aTargetStartUpmode: 0x%x"),
                 aTargetStartUpmode));

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::GenerateReset
// -----------------------------------------------------------------------------
TInt DDmcLogicalDevice::GenerateReset(const RDmc::TTargetStartupMode aTargetStartUpmode) const
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_GENERATE_RESET_ENTRY,
              "DMC:LOG_DEVICE: GenerateReset() # IN - aTargetStartUpmode: 0x%x",
              (aTargetStartUpmode));
    DMC_TRACE((("DMC:LOG_DEVICE: GenerateReset() # IN - aTargetStartUpmode: 0x%x"),
                 aTargetStartUpmode));

    TInt ret(KErrNone);
    TUint bootReasonApiStartUpMode;

    // Convert the user side target mode, in which a device is reset, to Boot Reason API mode.
    switch (aTargetStartUpmode)
        {
        case  RDmc::EStartupModeNone:
            {
            bootReasonApiStartUpMode = TBootReason::EStartupModeNone;
            }
            break;

        case RDmc::EStartupModeNormal:
            {
            bootReasonApiStartUpMode = TBootReason::EStartupModeNormal;
            }
            break;

        case RDmc::EStartupModeCharging:
            {
            bootReasonApiStartUpMode = TBootReason::EStartupModeCharging;
            }
            break;

        case RDmc::EStartupModeAlarm:
            {
            bootReasonApiStartUpMode = TBootReason::EStartupModeAlarm;
            }
            break;

        case RDmc::EStartupModeTest:
            {
            bootReasonApiStartUpMode = TBootReason::EStartupModeTest;
            }
            break;

        case RDmc::EStartupModeFota:
            {
            bootReasonApiStartUpMode = TBootReason::EStartupModeFota;
            }
            break;

        default:
            {
            // A fatal error, make a reset
            OstTrace0(TRACE_FLOW, DDMC_LOGICAL_DEVICE_GENERATE_RESET_5,
                      "DMC:EH_APE_CEN: GenerateReset() - aTargetStartUpmode");
            DMC_TRACE_ASSERT_RESET_BRUTAL("DMC:EH_APE_CEN: GenerateReset() - aTargetStartUpmode",
                                               KErrArgument);
            }
            break;
        }

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_GENERATE_RESET_1,
              "DMC:LOG_DEVICE: GenerateReset() - bootReasonApiStartUpMode: 0x%x",
              bootReasonApiStartUpMode);
    DMC_TRACE((("DMC:LOG_DEVICE: GenerateReset() - bootReasonApiStartUpMode: 0x%x"),
                 bootReasonApiStartUpMode));

    // Generate reset
    ret = DmcEvHandApeCent::HandleEvent(USER_GENERATE_RESET,
                                        NULL/*Kernel*/,
                                        &bootReasonApiStartUpMode);

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE__GENERATE_RESET_RETURN,
             "DMC:LOG_DEVICE: GenerateReset() # OUT - ret: %d",
              ret);
    DMC_TRACE((("DMC:LOG_DEVICE: GenerateReset() # OUT - ret: %d"),
                 ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::PowerOff
// -----------------------------------------------------------------------------
TInt DDmcLogicalDevice::PowerOff() const
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_POWER_OFF_ENTRY,
              "DMC:LOG_DEVICE: PowerOff() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: PowerOff() # IN"));

    TInt ret(KErrNone);
    TUint bootReasonApiStartUpMode;

    // None means that a device does not reset
    bootReasonApiStartUpMode = TBootReason::EStartupModeNone;

    // Power off the device
    ret = DmcEvHandApeCent::HandleEvent(USER_POWER_OFF,
                                        NULL/*Kernel*/,
                                        &bootReasonApiStartUpMode);

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_POWER_OFF_RETURN,
             "DMC:LOG_DEVICE: PowerOff() # OUT - ret: %d",
              ret);
    DMC_TRACE((("DMC:LOG_DEVICE: PowerOff() # OUT - ret: %d"), ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::IsHiddenReset
// -----------------------------------------------------------------------------
TInt DDmcLogicalDevice::IsHiddenReset(TBool& aHiddenStatus) const
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_HIDDEN_RESET_ENTRY,
              "DMC:LOG_DEVICE: IsHiddenReset() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: IsHiddenReset() # IN"));

    TInt ret(KErrNone);
    TUint isHidden;

    // Power off the device
    ret = DmcEvHandApeCent::HandleEvent(USER_GET_HIDDEN_RESET_STATUS, NULL/*Kernel*/, &isHidden);
    
    aHiddenStatus = static_cast<TBool>(isHidden);

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_HIDDEN_RESET_1,
             "DMC:LOG_DEVICE: IsHiddenReset() - aHiddenStatus: %d", aHiddenStatus);
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_HIDDEN_RESET_RETURN,
              "DMC:LOG_DEVICE: IsHiddenReset() # OUT - ret: %d", ret);
    DMC_TRACE((("DMC:LOG_DEVICE: IsHiddenReset() # OUT - ret: %d, aHiddenStatus: %d"),
                 ret, aHiddenStatus));

    return ret;
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::SubscribeEvents
// -----------------------------------------------------------------------------
void DDmcLogicalDevice::SubscribeEvents(const DDmcLogicalChannel* const aLogChnlPtr)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_SUBSCRIBE_EVENTS_ENTRY,
              "DMC:LOG_DEVICE: SubscribeEvents() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: SubscribeEvents() # IN"));

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_SUBSCRIBE_EVENTS_1,
             "DMC:LOG_DEVICE: SubscribeEvents() - iUserEventDfcPtr: 0x%x", iUserEventDfcPtr);
    DMC_TRACE((("DMC:LOG_DEVICE: SubscribeEvents() - iUserEventDfcPtr: 0x%x"), iUserEventDfcPtr));

    // Needed to deliver asynchronous events.
    iLogChannelsPtr.Append(aLogChnlPtr);

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_CREATE_1,
              "DMC:LOG_DEVICE: SubscribeEvents() - iLogChannelsPtr.Count: %d",
              (TUint)(iLogChannelsPtr.Count()));
    DMC_TRACE((("DMC:LOG_DEVICE: SubscribeEvents() iLogChannelsPtr.Count: %d"),
                 iLogChannelsPtr.Count()));

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_SUBSCRIBE_EVENTS_RETURN,
              "DMC:LOG_DEVICE: SubscribeEvents() # OUT");
    DMC_TRACE(("DMC:LOG_DEVICE: SubscribeEvents() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::UnsubscribeEvents
// -----------------------------------------------------------------------------
void DDmcLogicalDevice::UnsubscribeEvents(const DDmcLogicalChannel* const aLogChnlPtr)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_UNSUBSCRIBE_EVENTS_ENTRY,
              "DMC:LOG_DEVICE: UnsubscribeEvents() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: UnsubscribeEvents() # IN"));

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_UNSUBSCRIBE_EVENTS_1,
              "DMC:LOG_DEVICE: UnsubscribeEvents() - Nbr of Logical Channels, Count: %d",
              iLogChannelsPtr.Count());
    DMC_TRACE((("DMC:LOG_DEVICE: UnsubscribeEvents() - Nbr of Logical Channels, Count: %d"),
                 iLogChannelsPtr.Count()));
    TInt i(0);

    if (iLogChannelsPtr.Count() > 0)
        {
        i = iLogChannelsPtr.Find(aLogChnlPtr);

        OstTrace1(TRACE_FLOW, DDMC_LOGICAL_DEVICE_UNSUBSCRIBE_EVENTS_2,
                  "DMC:LOG_DEVICE: UnsubscribeEvents() - Array index: %d", i);
        DMC_TRACE((("DMC:LOG_DEVICE: UnsubscribeEvents() - Array index: %d"), i));
        
    
        /* No need to check the position of the array because "Remove()" panics with the 
           "PANIC: 130" if the position value is negative or greator than elements
           in the array.
    
           Removes only a logical channel object from the array, it does not delete it.
           We do not have to delete the object itself because framework does it. */
        iLogChannelsPtr.Remove(i);
        }

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_UNSUBSCRIBE_EVENTS_RETURN,
              "DMC:LOG_DEVICE: UnsubscribeEvents() # OUT");
    DMC_TRACE(("DMC:LOG_DEVICE: UnsubscribeEvents() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::ReceiveEventsDfc
// -----------------------------------------------------------------------------
void DDmcLogicalDevice::ReceiveEventsDfc(TAny* aPtr)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_REC_EVENTS_DFC_ENTRY,
              "DMC:LOG_DEVICE: ReceiveEventsDfc() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: ReceiveEventsDfc() # IN"));
    DMC_TRACE_ASSERT_RESET(aPtr, "DMC:LOG_DEVICE: ReceiveEventsDfc() - Kernel NULL pointer",
                           KErrNoMemory);

    DDmcLogicalDevice* dmcLogDevPtr = static_cast<DDmcLogicalDevice *>(aPtr);

    OstTrace1(TRACE_FLOW, DDMC_REC_EVENTS_DFC_1,
              "DMC:LOG_DEVICE: ReceiveEventsDfc() - dmcLogDevPtr: 0x%x",
              (TUint)(dmcLogDevPtr));
    DMC_TRACE((("DMC:LOG_DEVICE: ReceiveEventsDfc() - dmcLogDevPtr: 0x%x"),
                 dmcLogDevPtr));

    // Get the type of the event. Power Off or reset are allowed.
    TUint event;
    RDmc::TEvent eventType;

    /* No need to detect the ret value because the ret value indicates whether a request
       is taken into process or not and this request is part of the any state, thus always
       handled. */
    (void)DmcEvHandApeCent::HandleEvent(USER_GET_EVENT_TYPE, NULL/*Kernel*/, &event);

    OstTrace1(TRACE_FLOW, DDMC_REC_EVENTS_DFC_2,
              "DMC:LOG_DEVICE: ReceiveEventsDfc() - Ext event type: 0x%x",
              (TUint)(event));
    DMC_TRACE((("DMC:LOG_DEVICE: ReceiveEventsDfc() - Ext event type: 0x%x"),
                 event));

    switch (event)
        {
        case DMC_CLIENT_POWER_OFF_EVENT:
            {
            eventType = RDmc::EPowerOffEvent;
            }
            break;

        case DMC_CLIENT_RESET_EVENT:
            {
            eventType = RDmc::EResetEvent;
            }
            break;

        default:
            {
            DMC_TRACE_ALWAYS_ASSERT;
            DMC_TRACE(("DMC:LOG_DEVICE: ReceiveEventsDfc() - default:"));
            }
            break;
        }

    OstTrace1(TRACE_FLOW, DDMC_REC_EVENTS_DFC_3,
              "DMC:LOG_DEVICE: ReceiveEventsDfc() - RDmc eventType: %d",
              (TUint)(eventType));
    DMC_TRACE((("DMC:LOG_DEVICE: ReceiveEventsDfc() - RDmc eventType: %d"),
                 eventType));

    OstTrace1(TRACE_FLOW, DDMC_REC_EVENTS_DFC_4,
              "DMC:LOG_DEVICE: ReceiveEventsDfc() - Nbr of Logical Channels, Count: %d",
              dmcLogDevPtr->iLogChannelsPtr.Count());
    DMC_TRACE((("DMC:LOG_DEVICE: ReceiveEventsDfc() - Nbr of Logical Channels, Count: %d"),
                 dmcLogDevPtr->iLogChannelsPtr.Count()));

    // Send an event to all clients.
    for (TUint i = 0; i < dmcLogDevPtr->iLogChannelsPtr.Count(); i++)
        {
        dmcLogDevPtr->iLogChannelsPtr[i]->SendEvent(eventType);
        }

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_REC_EVENTS_DFC_RETURN,
              "DMC:LOG_DEVICE: ReceiveEventsDfc() # OUT");
    DMC_TRACE(("DMC:LOG_DEVICE: ReceiveEventsDfc() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcLogicalDevice::GetDynamicDfcQueue
// -----------------------------------------------------------------------------
TDynamicDfcQue* DDmcLogicalDevice::GetDynamicDfcQueue() const
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_GET_DFCQ_ENTRY,
              "DMC:LOG_DEVICE: GetDynamicDfcQueue() # IN");
    DMC_TRACE(("DMC:LOG_DEVICE: GetDynamicDfcQueue() # IN"));

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_DEVICE_GET_DFCQ_RETURN,
              "DMC:LOG_DEVICE: GetDynamicDfcQueue() # OUT - iDmcLogDeviceDfcQueuePtr: 0x%x",
              iDmcLogDeviceDfcQueuePtr);
    DMC_TRACE((("DMC:LOG_DEVICE: GetDynamicDfcQueue() # OUT - iDmcLogDeviceDfcQueuePtr: 0x%x"),
                 iDmcLogDeviceDfcQueuePtr));

    return iDmcLogDeviceDfcQueuePtr;
    }

//  End of File
