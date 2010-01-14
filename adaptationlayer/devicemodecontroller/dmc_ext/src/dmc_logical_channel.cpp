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
*     Implementation of DMC logical channel.
*
*/


// INCLUDE FILES 
#include <kern_priv.h>               // class DThread
#include "dmc_extension.h"           // iDmcDfcQueuePtrv
#include "dmc_user_if.h"
#include "dmc_logical_channel.h"
#include "dmc_logical_device.h"
#include "dmc_trace.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dmc_logical_channelTraces.h"
#endif

// EXTERNAL DATA STRUCTURES 
// EXTERNAL FUNCTION PROTOTYPES 
// FORWARD DECLARATIONS
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES  


// ==================== LOCAL FUNCTIONS ====================
// ================= OTHER EXPORTED FUNCTIONS ==============

// ================= MEMBER FUNCTIONS =======================

//-----------------------------------------------------------
//  DDmcLogicalChannel::DDmcLogicalChannel
//-----------------------------------------------------------
DDmcLogicalChannel::DDmcLogicalChannel(DLogicalDevice* aLogicalDevicePtr)
    : DLogicalChannel()
    , iDmcLogicalDevPtr(NULL)
    , iClientThreadPtr(NULL)
    , iClientStatusPtr(NULL)
    , iClientEventBufferPtr(NULL)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DDMC_LOGICAL_CHANNEL_ENTRY,
              "DMC:LOGIC_CHNL: DDmcLogicalChannel() # IN - aLogicalDevicePtr, 0x%x:",
              aLogicalDevicePtr);
    DMC_TRACE((("DMC:LOGIC_CHNL: DDmcLogicalChannel() # IN - aLogicalDevicePtr, 0x%x:"),
                 aLogicalDevicePtr));

    DMC_TRACE_ASSERT(aLogicalDevicePtr);

    iDmcLogicalDevPtr = reinterpret_cast<DDmcLogicalDevice*>(aLogicalDevicePtr);

    iClientThreadPtr  = &Kern::CurrentThread();
    TInt result = ((DObject*)iClientThreadPtr)->Open();

    DMC_TRACE_ASSERT(result == KErrNone);

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DDMC_LOGICAL_CHANNEL_RETURN,
              "DMC:LOGIC_CHNL: DDmcLogicalChannel() # OUT");
    DMC_TRACE(("DMC:LOGIC_CHNL: DDmcLogicalChannel() # OUT"));
    }


//-----------------------------------------------------------
//  DDmcLogicalChannel::~DDmcLogicalChannel
//-----------------------------------------------------------
DDmcLogicalChannel::~DDmcLogicalChannel()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL__DDMC_LOGICAL_CHANNEL_ENTRY,
              "DMC:LOGIC_CHNL: ~DDmcLogicalChannel() # IN");
    DMC_TRACE(("DMC:LOGIC_CHNL: ~DDmcLogicalChannel() # IN"));

    // Calling of Kern::SafeClose must be in a critical section 
    NKern::ThreadEnterCS();

    OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DDMC_LOGICAL_CHANNEL_1,
              "DMC:LOGIC_CHNL: ~DDmcLogicalChannel() - call Kern::SafeClose");
    DMC_TRACE(("DMC:LOGIC_CHNL: ~DDmcLogicalChannel() - call Kern::SafeClose"));

    // Close our reference on the client thread
    Kern::SafeClose((DObject*&)iClientThreadPtr, NULL);

    NKern::ThreadLeaveCS();

    // Cancel reset and power off events
    iDmcLogicalDevPtr->UnsubscribeEvents(this);

    if (iDmcLogicalDevPtr)
        {
        iDmcLogicalDevPtr = NULL;
        }
    
    if (iClientThreadPtr)
        {
        iClientThreadPtr = NULL;
        }
    
    if (iClientStatusPtr)
        {
        iClientStatusPtr = NULL;
        }

    if (iClientEventBufferPtr)
        {
        iClientEventBufferPtr = NULL;
        }
    
    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL__DDMC_LOGICAL_CHANNEL_RETURN,
              "DMC:LOGIC_CHNL: ~DDmcLogicalChannel() # OUT");
    DMC_TRACE(("DMC:LOGIC_CHNL: ~DDmcLogicalChannel() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcLogicalChannel::DoCreate
// -----------------------------------------------------------------------------
TInt DDmcLogicalChannel::DoCreate(TInt /* aUnit */, const TDesC8*  /* aInfo */, const TVersion& aVer)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DO_CREATE_ENTRY,
              "DMC:LOGIC_CHNL: DoCreate() # IN - aVer: 0x%x", (TUint)&(aVer));
    DMC_TRACE((("DMC:LOGIC_CHNL: DoCreate() # IN - aVer: 0x%x "), &aVer));

    TInt retVal(KErrNone);

    // capability security check
    if (!Kern::CurrentThreadHasCapability(ECapabilityCommDD, 
                                          __PLATSEC_DIAGNOSTIC_STRING("Checked by dmc.ldd") ) )
        {
        DMC_TRACE_ALWAYS_ASSERT;

        retVal = KErrPermissionDenied;
        }

    // Check version
    if (retVal == KErrNone)
        {
        if (!Kern::QueryVersionSupported(TVersion(RDmc::EMajorVersionNumber,
                                                  RDmc::EMinorVersionNumber,
                                                  RDmc::EBuildVersionNumber),
                                                  aVer))
            {
            OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CREATE_1,
                      "DMC:LOGIC_CHNL: DoCreate() - KErrNotSupported");
            DMC_TRACE_ALWAYS_ASSERT;

            retVal = KErrNotSupported;
            }
        }

    // check user's Vendor ID to ensure that only known client are accessing
    if (retVal == KErrNone)
        {
        TUint32 vendorId = Kern::CurrentThread().iOwningProcess->iS.iVendorId;

        OstTrace1(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CREATE_2,
                  "DMC:LOGIC_CHNL: DoCreate() - vendorId: 0x%x", vendorId);
        DMC_TRACE((("DMC:LOGIC_CHNL: DoCreate() - vendorId: 0x%x "), vendorId));

        if (vendorId != VID_DEFAULT)
            {   
            OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CREATE_3,
                      "DMC:LOGIC_CHNL: DoCreate() - VID check FAILED !");
            DMC_TRACE(("DMC:LOGIC_CHNL: DoCreate() - VID check FAILED !"));

            DMC_TRACE_ALWAYS_ASSERT;

            retVal = KErrAccessDenied;
            }
        }

    // Initialize DFC queue
    DMC_TRACE_ASSERT_RESET(iDmcLogicalDevPtr->GetDynamicDfcQueue(),
                           "DMC:LOGIC_CHNL: DoCreate() - iDmcLogicalDevPtr->GetDynamicDfcQueue()",
                           KErrNoMemory);

    SetDfcQ(iDmcLogicalDevPtr->GetDynamicDfcQueue());
    iMsgQ.Receive();

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DO_CREATE_RETURN,
              "DMC:LOGIC_CHNL: DoCreate() # OUT - retVal: %d", retVal);
    DMC_TRACE((("DMC:LOGIC_CHNL: DoCreate() # OUT - retVal: %d "), retVal));
    
    return retVal;
    }


// ---------------------------------------------------------
// DDmcLogicalChannel::HandleMsg
// ---------------------------------------------------------
void DDmcLogicalChannel::HandleMsg(TMessageBase* aMsg)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_HANDLE_MSG_ENTRY,
              "DMC:LOGIC_CHNL: HandleMsg() # IN - aMsg: 0x%x",
              (TUint)(aMsg));
    DMC_TRACE((("DMC:LOGIC_CHNL: HandleMsg() # IN - aMsg: 0x%x "), aMsg));

    DMC_TRACE_ASSERT_RESET(aMsg,
                           "DMC:LOGIC_CHNL: HandleMsg() - aMsg",
                           KErrNoMemory);

    TThreadMessage& m = *reinterpret_cast<TThreadMessage*>(aMsg);

    // Get message type
    TInt msgId(m.iValue);

    OstTrace1(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_HANDLE_MSG_1,
              "DMC:LOGIC_CHNL: HandleMsg() - msgId: 0x%x",
              (TUint)(msgId));
    DMC_TRACE((("DMC:LOGIC_CHNL: HandleMsg() - msgId: 0x%x "), msgId));

    // close request
    if (msgId == static_cast<TInt>(ECloseMsg))
        {
        OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_HANDLE_MSG_3,
                  "DMC:LOGIC_CHNL: HandleMsg() - ECloseMsg");
        DMC_TRACE(("DMC:LOGIC_CHNL: HandleMsg() - ECloseMsg)"));

        m.Complete(KErrNone, EFalse);
        }
    else if (msgId == KMaxTInt)
        {
        // cancel request
        OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_HANDLE_MSG_4,
                  "DMC:LOGIC_CHNL: HandleMsg() - KMaxTInt, DoCancel");
        DMC_TRACE(("DMC:LOGIC_CHNL: HandleMsg() - KMaxTInt, DoCancel"));

        m.Complete(KErrNone, ETrue);
        }
    else if (msgId >= 0)
        {
        // sync request
        OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_HANDLE_MSG_5,
                  "DMC:LOGIC_CHNL: HandleMsg() - DoControl");
        DMC_TRACE(("DMC:LOGIC_CHNL: HandleMsg() - DoControl"));

        // DoControl
        TInt retStatus(DoControl(msgId, m.Ptr0()));
        m.Complete(retStatus, ETrue);
        }
    else
        {
        // async request
        OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_HANDLE_MSG_6,
                  "DMC:LOGIC_CHNL: HandleMsg() - DoRequest");
        DMC_TRACE(("DMC:LOGIC_CHNL: HandleMsg() - DoRequest"));
        
        // DoRequest
        TRequestStatus* usrReqStatPtr = (TRequestStatus *)m.Ptr0();

        TInt retStatus(DoRequest(~msgId, usrReqStatPtr, m.Ptr1()));

        if (retStatus != KErrNone)
            {
            Kern::RequestComplete(iClientThreadPtr, usrReqStatPtr, retStatus);
            }

        m.Complete(KErrNone, ETrue);
        }

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_HANDLE_MSG_RETURN,
              "DMC:LOGIC_CHNL: HandleMsg() # OUT ");
    DMC_TRACE(("DMC:LOGIC_CHNL: HandleMsg() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcLogicalChannel::SendEvent
// -----------------------------------------------------------------------------
void DDmcLogicalChannel::SendEvent(const RDmc::TEvent& aEvent)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_SEND_EVENT_ENTRY,
              "DMC:LOGIC_CHNL: SendEvent() # IN - aEvent: %d", aEvent);
    DMC_TRACE((("DMC:LOGIC_CHNL: SendEvent() # IN - aEvent: %d"), aEvent));

    // Make sure that a client has subscribed events.
    if (iClientStatusPtr)
        {
        OstTrace1(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_SEND_1,
                  "DMC:LOGIC_CHNL: SendEvent() - KErrNone: %d", KErrNone);
        DMC_TRACE((("DMC:LOGIC_CHNL: SendEvent() - KErrNone: %d"), KErrNone));

        TInt retStatus = Kern::ThreadRawWrite(iClientThreadPtr,
                                              iClientEventBufferPtr,
                                              &aEvent,
                                              sizeof(aEvent));

        OstTrace1(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_SEND_EVENT_1,
              "DMC:LOGIC_CHNL: SendEvent() # IN - retStatus: %d", retStatus);
        DMC_TRACE((("DMC:LOGIC_CHNL: SendEvent() # IN - retStatus: %d, iClientStatusPtr: 0x%x"),
                     retStatus, iClientStatusPtr));

        Kern::RequestComplete(iClientThreadPtr, iClientStatusPtr, retStatus);
        }

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_SEND_EVENT_RETURN,
              "DMC:LOGIC_CHNL: SendEvent() # OUT ");
    DMC_TRACE(("DMC:LOGIC_CHNL: SendEvent() # OUT"));
    }


// -----------------------------------------------------------------------------
// DDmcLogicalChannel::DoControl
// -----------------------------------------------------------------------------
TInt DDmcLogicalChannel::DoControl(const TInt aFunction, TAny* a1)
    {
    OstTraceExt2(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DO_CONTROL_ENTRY,
                 "DMC:LOGIC_CHNL: DoControl() # IN - aFunction: 0x%x, a1: 0x%x",
                 (TUint)(aFunction), (TUint)(a1));
    DMC_TRACE((("DMC:LOGIC_CHNL: DoControl() # IN - aFunction: 0x%x, a1: 0x%x"), aFunction, a1));

    TInt retStatus(KErrBadHandle);

    if (iDmcLogicalDevPtr)
        {
        retStatus = KErrNone;

        switch (aFunction)
            {
            case RDmc::EGetTargetStartUpMode:
                {
                OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CONTROL_1,
                         "DMC:LOGIC_CHNL: DoControl() - EGetTargetStartUpMode");
                DMC_TRACE(("DMC:LOGIC_CHNL: DoControl() - EGetTargetStartUpMode"));

                RDmc::TTargetStartupMode targetStartUpMode;

                retStatus = iDmcLogicalDevPtr->GetTargetStartUpMode(targetStartUpMode);

                DMC_TRACE_ASSERT(retStatus == KErrNone);

                if (retStatus == KErrNone)
                    {
                    retStatus = Kern::ThreadRawWrite(iClientThreadPtr,
                                                     a1,
                                                     &targetStartUpMode,
                                                     sizeof(targetStartUpMode));
            
                    OstTraceExt2(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CONTROL_2,
                                 "DMC:LOGIC_CHNL: DoControl() - retStatus: %d, targetStartUpMode: %d",
                                 retStatus, targetStartUpMode);
                    DMC_TRACE((("DMC:LOGIC_CHNL: DoControl() - retStatus: %d, targetStartUpMode: %d"),
                                 retStatus, targetStartUpMode));
                    }
                }
                break;

            case RDmc::EGenerateReset:
                {
                OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CONTROL_3,
                         "DMC:LOGIC_CHNL: DoControl() - EGenerateReset");
                DMC_TRACE(("DMC:LOGIC_CHNL: DoControl() - EGenerateReset"));

                RDmc::TTargetStartupMode targetStartUpMode;

                retStatus = Kern::ThreadRawRead(iClientThreadPtr,
                                                a1,
                                                &targetStartUpMode,
                                                sizeof(targetStartUpMode));

                DMC_TRACE_ASSERT(retStatus == KErrNone);
                OstTraceExt2(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CONTROL_4,
                             "DMC:LOGIC_CHNL: DoControl() - retStatus: %d, targetStartUpMode: %d",
                             retStatus, targetStartUpMode);
                DMC_TRACE((("DMC:LOGIC_CHNL: DoControl() - retStatus: %d, targetStartUpMode: %d"),
                             retStatus, targetStartUpMode));

                if (retStatus == KErrNone)
                    {
                    retStatus = iDmcLogicalDevPtr->GenerateReset(targetStartUpMode);
                    }

                DMC_TRACE_ASSERT(retStatus == KErrNone);
                }
                break;

            case RDmc::EPowerOff:
                {
                OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CONTROL_5,
                         "DMC:LOGIC_CHNL: DoControl() - EPowerOff");
                DMC_TRACE(("DMC:LOGIC_CHNL: DoControl() - EPowerOff"));

                retStatus = iDmcLogicalDevPtr->PowerOff();

                DMC_TRACE_ASSERT(retStatus == KErrNone);
                }
                break;


            case RDmc::EIsHiddenReset:
                {
                OstTrace0(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CONTROL_6,
                         "DMC:LOGIC_CHNL: DoControl() - EIsHiddenReset");
                DMC_TRACE(("DMC:LOGIC_CHNL: DoControl() - EIsHiddenReset"));

                TBool hiddenStatus;

                retStatus = iDmcLogicalDevPtr->IsHiddenReset(hiddenStatus);

                DMC_TRACE_ASSERT(retStatus == KErrNone);

                if (retStatus == KErrNone)
                    {
                    retStatus = Kern::ThreadRawWrite(iClientThreadPtr,
                                                     a1,
                                                     &hiddenStatus,
                                                     sizeof(hiddenStatus));
            
                    OstTraceExt2(TRACE_FLOW, DDMC_LOGICAL_CHANNEL_DO_CONTROL_7,
                                 "DMC:LOGIC_CHNL: DoControl() - retStatus: %d, hiddenStatus: %d",
                                 retStatus, hiddenStatus);
                    DMC_TRACE((("DMC:LOGIC_CHNL: DoControl() - retStatus: %d, hiddenStatus: %d"),
                                 retStatus, hiddenStatus));
                    }
                }
                break;

            default:
                {
                DMC_TRACE_ALWAYS_ASSERT;

                retStatus = KErrNotSupported;
                }
                break;
            }
        }

    OstTrace1(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DO_CONTROL_RETURN,
              "DMC:LOGIC_CHNL: DoControl() # OUT - retStatus %d:", retStatus);
    DMC_TRACE((("DMC:LOGIC_CHNL: DoControl() # OUT - retStatus: %d"), retStatus));
    
    return retStatus;
    }


// -----------------------------------------------------------------------------
// DDmcLogicalChannel::DoRequest
// -----------------------------------------------------------------------------
TInt DDmcLogicalChannel::DoRequest(const TInt aReqId, TRequestStatus* const aStatusPtr, TAny* const a1)
    {
    OstTraceExt3(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DO_REQUEST_ENTRY,
                 "DMC:LOGIC_CHNL: DoRequest() # IN - aRegId: 0x%x, aStatusPtr: 0x%x, a1: 0x%x",
                 (TUint)(aReqId), (TUint)aStatusPtr, (TUint)(a1));
    DMC_TRACE((("DMC:LOGIC_CHNL: DoRequest() # IN - aReqId: 0x%x, aStatusPtr: 0x%x, a1: 0x%x"),
                 aReqId, aStatusPtr, a1));
    DMC_TRACE_ASSERT_RESET(a1, "DMC:LOG_DEVICE: DDmcLogicalDevice() - a1", KErrNoMemory);

    iClientStatusPtr      = aStatusPtr;
    iClientEventBufferPtr = static_cast<RDmc::TEvent*>(a1);

    iDmcLogicalDevPtr->SubscribeEvents(this);

    OstTrace0(TRACE_ENTRY_EXIT, DDMC_LOGICAL_CHANNEL_DO_REQUEST_RETURN,
              "DMC:LOGIC_CHNL: DoRequest() # OUT");
    DMC_TRACE(("DMC:LOGIC_CHNL: DoRequest() # OUT"));

    return KErrNone;
    }

//  End of File
