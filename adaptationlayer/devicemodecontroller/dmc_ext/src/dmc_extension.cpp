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
*     Implementation of DMC extension.
*
*/


// INCLUDE FILES
#include <kern_priv.h>              // DfcQue0()->iThread->iPriority
#include <isikernel.h>              // TIsiKernelSend
#include <isakernelapi.h>           // DIsaKernelAPI
#include <iscnokiadefinitions.h>    // EIscNokiaDMC
#include <pn_const.h>               // PN constants
#include <phonetisi.h>              // ISI_HEADER_***
#include "dmc_extension.h"          // DDmcExtension
#include "dmc_trace.h"
#include "dmc_event_handler_ape_cent.h"

#include "OstTraceDefinitions.h"    // For Open System Trace
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dmc_extensionTraces.h"
#endif


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
#define DFC_OBJECT_PRIORITY           1     // priority for DFC object, 1 is default

_LIT(KDmcName, "DMC");                      // Name given to power handler 
_LIT(KDMCExtDfc, "DMCExtDFC");              // Name given to DfcQCreate()

// MODULE DATA STRUCTURES
DDmcExtension* DDmcExtension::iDmcExtPtr(NULL);

// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// Extension entry point
// -----------------------------------------------------------------------------
//
DECLARE_STANDARD_EXTENSION()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DECLARE_STANDARD_EXTENSION_ENTRY,
              "DMC:KERN_EXTEN: DECLARE_STANDARD_EXTENSION() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: DECLARE_STANDARD_EXTENSION() # IN"));

    DDmcExtension::iDmcExtPtr = new DDmcExtension;
    
    DMC_TRACE((("DMC:KERN_EXTEN: DECLARE_STANDARD_EXTENSION() - iDmcExtPtr 0x%x:"),
                 DDmcExtension::iDmcExtPtr));

    DMC_TRACE_ASSERT_RESET(DDmcExtension::iDmcExtPtr, "DMC:KERN_EXTEN: create failed",
                           KErrNoMemory);

    DDmcExtension::iDmcExtPtr->Init();

    DMC_TRACE(("DMC:KERN_EXTEN: DECLARE_STANDARD_EXTENSION() - call Init()"));

    OstTrace0(TRACE_ENTRY_EXIT, DECLARE_STANDARD_EXTENSION_RETURN,
              "DMC:KERN_EXTEN:DECLARE_STANDARD_EXTENSION()# OUT - KErrNone");
    DMC_TRACE(("DMC:KERN_EXTEN: DECLARE_STANDARD_EXTENSION() # OUT - KErrNone"));

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// DDmcExtension::DDmcExtension
// Constructor
// -----------------------------------------------------------------------------
//
DDmcExtension::DDmcExtension() 
    : DPowerHandler(KDmcName)
    , iDmcDfcQueuePtr(NULL)
    , iReceiveMsgPtr(NULL)
    , iIsaKernelAPIOpenStatus(KErrUnknown)
    , iIsaKernelAPIReceiveStatus(KErrUnknown)
    , iIsaKernelModemStatus(EIscConnectionNotOk)
    , iChannelOpenedDfcPtr(NULL)
    , iMsgReceivedDfcPtr(NULL)
    , iStateChangedDfcPtr(NULL)
    , iPowerDownDfcPtr(NULL)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_DDMC_ENTRY, "DMC:KERN_EXTEN: DDmcExtension() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: DDmcExtension() # IN"));

    TInt dfcQueueDmcPriority = (Kern::DfcQue0()->iThread->iPriority) + 1;

    DMC_TRACE((("DMC:KERN_EXTEN: DDmcExtension() - dfcQueueDmcPriority: %d"),
                 dfcQueueDmcPriority));
    
    // DFC queue must be created in the critical section
    NKern::ThreadEnterCS();
    TInt dfcStatus = Kern::DfcQCreate(iDmcDfcQueuePtr, dfcQueueDmcPriority, &KDMCExtDfc);

    NKern::ThreadLeaveCS();

    DMC_TRACE((("DMC:KERN_EXTEN: DDmcExtension() - dfcStatus: 0x%x "), dfcStatus));
    DMC_TRACE_ASSERT_RESET(dfcStatus == KErrNone , "Kern::DfcQCreate failed", dfcStatus);
    
    OstTrace1(TRACE_FLOW, DDMC_DMC_1,"DMC:KERN_EXTEN: DDmcExtension() - return; KErrNone=%d", KErrNone);
    DMC_TRACE((("DMC:KERN_EXTEN: DDmcExtension() - InitiDmcDfcQueuePtr: 0x%x "), iDmcDfcQueuePtr));

    // We never delete this objects because DMC is running as long as a device is running.
    iChannelOpenedDfcPtr = new TDfc(ChannelOpenedDfc, this, iDmcDfcQueuePtr, DFC_OBJECT_PRIORITY);
    iMsgReceivedDfcPtr   = new TDfc(MsgReceivedDfc,   this, iDmcDfcQueuePtr, DFC_OBJECT_PRIORITY);
    iStateChangedDfcPtr  = new TDfc(StateChangedDfc,  this, iDmcDfcQueuePtr, DFC_OBJECT_PRIORITY);
    iPowerDownDfcPtr     = new TDfc(PowerDownDfc,     this, iDmcDfcQueuePtr, DFC_OBJECT_PRIORITY);

    DMC_TRACE_ASSERT_RESET(iChannelOpenedDfcPtr,
                           "DMC:KERN_EXTEN: DDmcExtension() - iChannelOpenedDfcPtr",
                           KErrNoMemory);
    DMC_TRACE_ASSERT_RESET(iMsgReceivedDfcPtr,
                           "DMC:KERN_EXTEN: DDmcExtension() - iMsgReceivedDfcPtr",
                           KErrNoMemory);
    DMC_TRACE_ASSERT_RESET(iStateChangedDfcPtr,
                           "DMC:KERN_EXTEN: DDmcExtension() - iStateChangedDfcPtr", 
                           KErrNoMemory);

    DMC_TRACE_ASSERT_RESET(iPowerDownDfcPtr,
                           "DMC:KERN_EXTEN: DDmcExtension() - iPowerDownDfcPtr", 
                           KErrNoMemory);

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_DDMC_RETURN, "DMC:KERN_EXTEN: DDmcExtension() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: DDmcExtension() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::Init
// Handles the initialization of DMC
// -----------------------------------------------------------------------------
//
void DDmcExtension::Init()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_INIT_ENTRY, "DMC:KERN_EXTEN: Init() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: Init() # IN"));

    OstTrace0(TRACE_FLOW, DDMC_INIT_1, "DMC:KERN_EXTEN: Init() - DPowerHandler -> Add");
    DMC_TRACE(("DMC:KERN_EXTEN: Init() - DPowerHandler -> Add"));

    // Register to Power Handler
    Add();

    OstTrace0(TRACE_FLOW, DDMC_INIT_2, "DMC:KERN_EXTEN: Init() - DIsaKernelAPI::NewF");
    DMC_TRACE(("DMC:KERN_EXTEN: Init() - DIsaKernelAPI::NewF"));

    // Create new IsaKernelAPI object. Throws Kern::Fault if the allocation fails
    iIsaKernelApiPtr = DIsaKernelAPI::NewF();

    iIsaKernelApiPtr->Open(EIscNokiaDMC, iIsaKernelAPIOpenStatus, *iChannelOpenedDfcPtr);

    OstTrace1(TRACE_FLOW, DDMC_INIT_3,"DMC:KERN_EXTEN: Init() - iIsaKernelAPIOpenStatus: %d ",
              iIsaKernelAPIOpenStatus);
    DMC_TRACE((("DMC:KERN_EXTEN: Init() - iIsaKernelAPIOpenStatus %d: "), iIsaKernelAPIOpenStatus));

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_INIT_RETURN, "DMC:KERN_EXTEN: Init() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: Init() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::PowerUp
// Handles the powering up of the device
// -----------------------------------------------------------------------------
//
void DDmcExtension::PowerUp()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_POWER_UP_ENTRY, "DMC:KERN_EXTEN: PowerUp() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: PowerUp() # IN"));

    OstTrace0(TRACE_FLOW, DDMC_POWERUP, "DMC:KERN_EXTEN: PowerUp() - PM asks PowerUp");
    DMC_TRACE(("DMC:KERN_EXTEN: PowerUp() - PM asks PowerUp"));

    // We have nothing to do in this phase, so acknowledge to powerhandler.
    PowerUpDone();

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_POWER_UP_RETURN, "DMC:KERN_EXTEN: PowerUp() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: PowerUp() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::PowerDown
// Handles the powering down of the extension.
// -----------------------------------------------------------------------------
//
void DDmcExtension::PowerDown(TPowerState aTargetState)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_POWERDOWN_ENTRY,
              "DMC:KERN_EXTEN: PowerDown() # IN - aTargetState: 0x%x", (TUint)(aTargetState));
    // TPowerState is printed only for debugging purpose. See the values in e32power.h.
    DMC_TRACE((("DMC:KERN_EXTEN: PowerDown() # IN - aTargetState: 0x%x "), aTargetState));

    if (NKern::CurrentContext() == NKern::EInterrupt)
        {
        iPowerDownDfcPtr->Add();
        }
    else
        {
        iPowerDownDfcPtr->Enque();
        }

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_POWERDOWN_RETURN, "DMC:KERN_EXTEN: PowerDown() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: PowerDown() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::ChannelOpenedDfc
// Dfc function, called by IsaKernelIF when channel is opened
// -----------------------------------------------------------------------------
//
void DDmcExtension::ChannelOpenedDfc(TAny *aPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_CHANNEL_OPEN_DFC_ENTRY,
              "DMC:KERN_EXTEN: ChannelOpenedDfc()  # IN - aPtr: 0x%x", (TUint)(aPtr));
    DMC_TRACE((("DMC:KERN_EXTEN: ChannelOpenedDfc() # IN - aPtr: 0x%x"), aPtr));
    DMC_TRACE_ASSERT_RESET(aPtr, "DMC:EH_APE_CEN: ChannelOpenedDfc() - Kernel NULL pointer",
                           KErrNoMemory);

    DDmcExtension* dmcExtPtr = static_cast<DDmcExtension*>(aPtr);

    dmcExtPtr->CompleteIadInit();

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_CHANNEL_OPEN_DFC_RETURN,
              "DMC:KERN_EXTEN: ChannelOpenedDfc() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: ChannelOpenedDfc() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::MsgReceivedDfc
// Dfc function, called by IsaKernelIF when a message is received
// -----------------------------------------------------------------------------
//
void DDmcExtension::MsgReceivedDfc(TAny *aPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_MSG_RECEIVED_DFC_ENTRY, 
              "DMC:KERN_EXTEN: MsgReceivedDfc() # IN - aPtr: 0x%x", (TUint)(aPtr));
    DMC_TRACE((("DMC:KERN_EXTEN: MsgReceivedDfc() # IN - aPtr: 0x%x"), aPtr));
    DMC_TRACE_ASSERT_RESET(aPtr, "DMC:EH_APE_CEN: MsgReceivedDfc() - Kernel NULL pointer",
                           KErrNoMemory);

    DDmcExtension* dmcExtPtr = static_cast<DDmcExtension*>(aPtr);

    DMC_TRACE_ASSERT_RESET((dmcExtPtr->iIsaKernelAPIReceiveStatus == KErrNone),
                            "DMC:KERN_EXTEN: MsgReceivedDfc() - IsaKernelAPI Msg receive error",
                            dmcExtPtr->iIsaKernelAPIReceiveStatus);    

    TUint8* msgPtr = const_cast<TUint8*>(dmcExtPtr->iReceiveMsgPtr->Ptr());

    OstTrace1(TRACE_FLOW, DDMC_MSG_RECEIVED_DFC_1,
              "DMC:KERN_EXTEN: MsgReceivedDfc() - msgPtr: 0x%x", (TUint)(msgPtr));
    DMC_TRACE((("DMC:KERN_EXTEN: MsgReceivedDfc() - msgPtr: 0x%x"), msgPtr));

    DmcEvHandApeCent::HandleEvent(KERNEL_EVENT_ISI, msgPtr, NULL /* User */);

    // Free the message buffer
    dmcExtPtr->iIsaKernelApiPtr->DeAllocateMsgBlock(*dmcExtPtr->iReceiveMsgPtr);
    dmcExtPtr->iReceiveMsgPtr = NULL;

    // Continue receiving messages
    dmcExtPtr->ReceiveMessage();

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_MSG_RECEIVED_DFC_RETURN,
              "DMC:KERN_EXTEN: MsgReceivedDfc() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: MsgReceivedDfc() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::StateChangedDfc
// Dfc function, called by IsaKernelIF when a channel state has been changed
// -----------------------------------------------------------------------------
//
void DDmcExtension::StateChangedDfc(TAny *aPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_STATE_CHANGED_DFC_ENTRY,
              "DMC:KERN_EXTEN: StateChangedDfc() # IN - aPtr: 0x%x", (TUint)(aPtr));
    DMC_TRACE((("DMC:KERN_EXTEN: StateChangedDfc() # IN - aPtr: 0x%x"), aPtr));
    DMC_TRACE_ASSERT_RESET(aPtr, "DMC:EH_APE_CEN: StateChangedDfc() - Kernel NULL pointer",
                           KErrNoMemory);

    DDmcExtension* dmcExtPtr = static_cast<DDmcExtension*>(aPtr);

    OstTraceExt2(TRACE_FLOW, DDMC_STATE_CHANGED_DFC_1,
                 "DMC:KERN_EXTEN: StateChangedDfc() - iIsaKernelAPIOpenStatus: %d, EIscConnectionOk: %d",
                 dmcExtPtr->iIsaKernelModemStatus, EIscConnectionOk);
    DMC_TRACE((("DMC:KERN_EXTEN: StateChangedDfc() - iIsaKernelAPIOpenStatus: %d, EIscConnectionOk: %d"),
                 dmcExtPtr->iIsaKernelModemStatus, EIscConnectionOk));

    // A Modem connenction lost.
    if (dmcExtPtr->iIsaKernelModemStatus != EIscConnectionOk)
        {
        DmcEvHandApeCent::HandleEvent(KERNEL_EVENT_MODEM_CONNECTION_NOT_OK,
                                      NULL /* Kernel*/, NULL /* User */);
        }

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_STATE_CHANGED_DFC_RETURN,
              "DMC:KERN_EXTEN: StateChangedDfc() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: StateChangedDfc() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::PowerDownDfc
// Dfc function, called by the Power Handler in a shutdown
// -----------------------------------------------------------------------------
//
void DDmcExtension::PowerDownDfc(TAny* aPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_POWER_DOWN_DFC_ENTRY,
              "DMC:KERN_EXTEN: PowerDownDfc() # IN - aPtr: 0x%x ", (TUint)(aPtr));
    DMC_TRACE((("DMC:KERN_EXTEN: PowerDownDfc() # IN - aPtr: 0x%x"), aPtr));

    (void)DmcEvHandApeCent::HandleEvent(KERNEL_EVENT_PWR_HANDLER, NULL/*Kernel*/, NULL /* User */);

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_POWER_DOWN_DFC_RETURN,
              "DMC:KERN_EXTEN: PowerDownDfc() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: PowerDownDfc() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::CompleteIadInit
// Completes the IAD initialization for ISI message handling
// -----------------------------------------------------------------------------
//
void DDmcExtension::CompleteIadInit()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_COMPLETE_ENTRY,
              "DMC:KERN_EXTEN: CompleteIadInit() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: CompleteIadInit() # IN"));

    // Initialize an event handler
    DmcEvHandApeCent::Init(DDmcExtension::iDmcExtPtr);

    OstTrace0(TRACE_FLOW, DDMC_COMPLETE_1,
             "DMC:KERN_EXTEN: CompleteIadInit() - Start listen IAD state change");
    DMC_TRACE(("DMC:KERN_EXTEN: CompleteIadInit() - Start listen IAD state change"));

    iIsaKernelApiPtr->NotifyStateChange(*iStateChangedDfcPtr, iIsaKernelModemStatus);

    // Starts listening ISI messages
    ReceiveMessage();

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_COMPLETE_RETURN, "DMC:KERN_EXTEN: CompleteIadInit() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: CompleteIadInit() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::ReceiveMessage
// Allocates a message and starts receiving ISI messages from IsaKernelIF
// -----------------------------------------------------------------------------
//
void DDmcExtension::ReceiveMessage()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_RECEIVEM_ESSAGE_ENTRY,
              "DMC:KERN_EXTEN: ReceiveMessage() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: ReceiveMessage() # IN"));

    iReceiveMsgPtr = NULL;
    iIsaKernelApiPtr->Receive(*&iReceiveMsgPtr, iIsaKernelAPIReceiveStatus, *iMsgReceivedDfcPtr);

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_RECEIVE_MESSAGE_RETURN,
              "DMC:KERN_EXTEN: ReceiveMessage # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: ReceiveMessage() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::AllocateMsgBlock
// Returns a reference to a message block
// -----------------------------------------------------------------------------
//
TDes8& DDmcExtension::AllocateMsgBlock(const TUint32 aSize) const
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_ALLOC_MSG_BLOCK_ENTRY,
              "DMC:KERN_EXTEN: AllocateMsgBlock() # IN - aSize: %d", aSize);
    DMC_TRACE((("DMC:KERN_EXTEN: AllocateMsgBlock() # IN - aSize: %d"), aSize));
    
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_ALLOC_MSG_BLOCK_RETURN,
              "DMC:KERN_EXTEN: AllocateMsgBlock() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: AllocateMsgBlock() # OUT"));
    
    return (iIsaKernelApiPtr->AllocateMsgBlock(aSize));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::Send
// Sends a message through Isa Access Driver
// -----------------------------------------------------------------------------
//
TInt DDmcExtension::Send(TDes8& aSendMsg)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_SEND_ENTRY,
              "DMC:KERN_EXTEN: Send() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: Send() # IN"));
    
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_SEND_RETURN,
              "DMC:KERN_EXTEN: Send() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: Send() # OUT"));

    return (iIsaKernelApiPtr->Send(aSendMsg));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::SubscribeIndications
// Sends a message through Isa Access Driver
// -----------------------------------------------------------------------------
//
TInt DDmcExtension::SubscribeIndications(TDes8& aIndications)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_SUBSCRIBE_IND_ENTRY,
              "DMC:KERN_EXTEN: SubscribeIndications() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: SubscribeIndications() # IN"));

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_SUBSCRIBE_IND_RETURN,
              "DMC:KERN_EXTEN: SubscribeIndications() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: SubscribeIndications() # OUT"));

    return (iIsaKernelApiPtr->SubscribeIndications(aIndications));
    }


// -----------------------------------------------------------------------------
// DDmcExtension::CompletePowerDown
// Completes the power down
// -----------------------------------------------------------------------------
//
void DDmcExtension::CompletePowerDown()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_COMPLETE_POWER_DOWN_ENTRY,
              "DMC:KERN_EXTEN: CompletePowerDown() # IN");
    DMC_TRACE(("DMC:KERN_EXTEN: CompletePowerDown() # IN"));

    // Tell to Power Handler that we are ready for power off / reset
    PowerDownDone();

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_COMPLETE_POWER_DOWN_RETURN,
              "DMC:KERN_EXTEN: CompletePowerDown() # OUT");
    DMC_TRACE(("DMC:KERN_EXTEN: CompletePowerDown() # OUT"));
    }

// End of File

