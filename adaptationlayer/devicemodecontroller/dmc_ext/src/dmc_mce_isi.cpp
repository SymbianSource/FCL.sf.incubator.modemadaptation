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
*     Implementation of DMC MCE ISI utility functions.
*
*/


// INCLUDE FILES 
#include <pn_const.h>               // PN constants
#include <phonetisi.h>              // ISI_HEADER_***

#ifdef USE_MTC_SERVER
#include <mtcisi.h>                 // MTC related definitions. for testing only
#else // USE_MTC_SERVER
#include <mceisi.h>                 // MCE ISI stuff
#endif // USE_MTC_SERVER

#include <isikernel.h>              // TIsiKernelSend
#include "dmc_extension.h"          // DMC extions pointer
#include "dmc_mce_isi.h"
#include "dmc_trace.h"

#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dmc_mce_isiTraces.h"
#endif

// EXTERNAL DATA STRUCTURES 
// EXTERNAL FUNCTION PROTOTYPES 
// FORWARD DECLARATIONS

// CONSTANTS
#define MAX_ISI_MSG_SIZE   32

/**
* Move a pointer over the phonet-header(8 bytes)
* This is used because a received ISI-message contains
* a phonet header, 8 bytes, which we are not interested
* in. 
*/
#define ISI_MSG(a) (ISI_HEADER_SIZE + a)

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES  
DDmcExtension* MceIsi::iDmcExtPtr(NULL);

// LOCAL FUNCTION PROTOTYPES  

// ==================== LOCAL FUNCTIONS ====================
// ================= OTHER EXPORTED FUNCTIONS ==============

// ================= MEMBER FUNCTIONS =======================
void MceIsi::Init(DDmcExtension* const aDmcExtPtr)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DMC_MCE_INIT_ENTRY,
              "DMC:MCE_ISIMSG: Init() # IN - aDmcExtPtr: 0x%x:", aDmcExtPtr);
    DMC_TRACE((("DMC:MCE_ISIMSG: Init() # IN - aDmcExtPtr: 0x%x"), aDmcExtPtr));

    iDmcExtPtr = aDmcExtPtr;

    DMC_TRACE_ASSERT_RESET(aDmcExtPtr, "DMC:MCE_ISIMSG: NULL pointer",
                           KErrNoMemory);

#ifdef USE_MTC_SERVER
    // List of MCE indications to be subcribed
    TUint8 dmcIndArray[] = {PN_MTC, MTC_STATE_INFO_IND};

#else // USE_MTC_SERVER
    TUint8 dmcIndArray[] = {PN_MODEM_MCE, MCE_MODEM_STATE_IND};

#endif // USE_MTC_SERVER

    TPtr8  ptr(dmcIndArray, sizeof(dmcIndArray), sizeof(dmcIndArray));

    TInt err = iDmcExtPtr->SubscribeIndications(ptr);
    DMC_TRACE_ASSERT_RESET(err == KErrNone , "Subscirbe ISI events failed", err);

#ifdef _DEBUG
    DMC_TRACE(("DMC:EH_APE_CEN: Init() - Subscribe MCE Indications"));

    for (TUint32 i = 0; i < (sizeof(dmcIndArray) / 2); i++)
        {
        DMC_TRACE((("DMC:EH_APE_CEN: Init() - Resource   ID: 0x%x"), dmcIndArray[i]));
        DMC_TRACE((("DMC:EH_APE_CEN: Init() - Indication ID: 0x%x"), dmcIndArray[i + 1]));
        }
#endif // _DEBUG

    OstTrace1(TRACE_ENTRY_EXIT, DMC_MCE_INIT_RETURN,
              "DMC:MCE_ISIMSG: Init() # OUT - iDmcExtPtr: 0x%x:", iDmcExtPtr);
    DMC_TRACE((("DMC:MCE_ISIMSG: Init() # OUT - iDmcExtPtr: 0x%x"), iDmcExtPtr));
    }


// -----------------------------------------------------------------------------
// MceIsi::MceModemStatQueryReq
// This function sends a state query req to Modem MCE-server.
// -----------------------------------------------------------------------------
//
void MceIsi::MceModemStateQueryReq()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_MODEM_STATE_QUERY_REQ_ENTRY,
              "DMC:MCE_ISIMSG: MceModemStateQueryReq() # IN");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceModemStateQueryReq() # IN"));

#ifdef USE_MTC_SERVER
    TIsiKernelSend sendMsg(iDmcExtPtr->AllocateMsgBlock(MAX_ISI_MSG_SIZE));
    sendMsg.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,   PN_MTC);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_TRANSID,      1);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_MESSAGEID,    MTC_STATE_QUERY_REQ);
    sendMsg.Set8bit(ISI_MSG(MTC_STATE_QUERY_REQ_OFFSET_FILLERBYTE1), 0);
    sendMsg.Set8bit(ISI_MSG(MTC_STATE_QUERY_REQ_OFFSET_FILLERBYTE2), 0);

#else // USE_MTC_SERVER
    TIsiKernelSend sendMsg(iDmcExtPtr->AllocateMsgBlock(MAX_ISI_MSG_SIZE));
    sendMsg.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,                 PN_MODEM_MCE);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_TRANSID,                    1);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_MESSAGEID,                  MCE_MODEM_STATE_QUERY_REQ);
    sendMsg.Set8bit(ISI_MSG(MCE_MODEM_STATE_QUERY_REQ_OFFSET_FILLERBYTE1), 0);
    sendMsg.Set8bit(ISI_MSG(MCE_MODEM_STATE_QUERY_REQ_OFFSET_FILLERBYTE2), 0);

#endif // USE_MTC_SERVER

     // Send message to receiver
    TInt err = iDmcExtPtr->Send(sendMsg.Complete());

    DMC_TRACE((("DMC:MCE_ISIMSG: MceModemStateQueryReq() - err: %d"), err));
    
    DMC_TRACE_ASSERT_RESET(err == KErrNone, "Send failed", err);

    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_MODEM_STATE_QUERY_REQ_RETURN,
              "DMC:MCE_ISIMSG: MceModemStatQueryReq() # OUT");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceModemStateQueryReq() # OUT"));
    }


//-----------------------------------------------------------
//  MceIsi::MceModemStateQueryResp
//-----------------------------------------------------------
TInt MceIsi::MceModemStateQueryResp(const TUint8* const aMsgPtr, TUint8& aModemCurrentState)
    {
    OstTrace1(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_STATE_QUERY_RESP_ENTRY,
              "DMC:MCE_ISIMSG: MceModemStateQueryResp() # IN - aMsgPtr: 0x%x",
              (TUint)aMsgPtr);
    DMC_TRACE((("DMC:EH_APE_CEN: MceModemStateQueryResp() # IN - aMsgPtr: 0x%x"), aMsgPtr));
    
    TInt retVal(KErrNone);

#ifdef USE_MTC_SERVER
    TUint8 trId(aMsgPtr[ISI_MSG(MTC_STATE_QUERY_RESP_OFFSET_TRANSID)]);
    TUint8 modemCurrentState(aMsgPtr[ISI_MSG(MTC_STATE_QUERY_RESP_OFFSET_CURRENT)]);
    TUint8 modemTargetState(aMsgPtr[ISI_MSG(MTC_STATE_QUERY_RESP_OFFSET_TARGET)]);

    DMC_TRACE((("DMC:MCE_ISIMSG: MceModemStateQueryResp() - trId: %d, modemCurrentState: 0x%x, modemTargetState: 0x%x:"),
                 trId, modemCurrentState, modemTargetState));

#else // USE_MTC_SERVER
    TUint8 trId(aMsgPtr[ISI_MSG(MCE_MODEM_STATE_QUERY_RESP_OFFSET_TRANSID)]);
    TUint8 modemCurrentState(aMsgPtr[ISI_MSG(MCE_MODEM_STATE_QUERY_RESP_OFFSET_CURRENT)]);
    TUint8 modemTargetState(aMsgPtr[ISI_MSG(MCE_MODEM_STATE_QUERY_RESP_OFFSET_TARGET)]);

    OstTraceExt3(TRACE_FLOW, DMC_MCE_ISI_MCE_STATE_QUERY_RESP_1,
                 "DMC:MCE_ISIMSG: MceModemStateQueryResp() - trId: %d, modemCurrentState: 0x%x, modemTargetState: 0x%x:",
                 trId, modemCurrentState, modemTargetState);
    DMC_TRACE((("DMC:MCE_ISIMSG: MceModemStateQueryResp() - trId: %d, modemCurrentState: 0x%x, modemTargetState: 0x%x:"),
                 trId, modemCurrentState, modemTargetState));
#endif // USE_MTC_SERVER

    // Check if Modem has already powered up. 
    if (modemCurrentState == modemTargetState)
        {
        // Modem ready
        aModemCurrentState = modemTargetState;

        DMC_TRACE(("DMC:MCE_ISIMSG: MceModemStateQueryResp() - modem ready"));
        }
    else
        {
        /* Modem is not ready because current and target states
           are not the same. */
        retVal = KErrNotReady;

        DMC_TRACE(("DMC:MCE_ISIMSG: MceModemStateQueryResp() - modem not ready"));
        }

    OstTrace1(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_STATE_QUERY_RESP_RETURN,
              "DMC:MCE_ISIMSG: MceModemStateQueryResp() # OUT - retVal: %d", retVal);
    DMC_TRACE((("DMC:MCE_ISIMSG: MceModemStateQueryResp() # OUT - retVal: %d"), retVal));

    return retVal;
    }


//-----------------------------------------------------------
//  MceIsi::McePowerOffReq
//-----------------------------------------------------------
void MceIsi::McePowerOffReq()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_POWER_OFF_REQ_ENTRY,
              "DMC:MCE_ISIMSG: McePowerOffReq() # IN");
    DMC_TRACE(("DMC:MCE_ISIMSG: McePowerOffReq() # IN"));

#ifdef USE_MTC_SERVER
    TIsiKernelSend sendMsg(iDmcExtPtr->AllocateMsgBlock(MAX_ISI_MSG_SIZE));
    sendMsg.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,   PN_MTC);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_TRANSID,      3);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_MESSAGEID,    MTC_POWER_OFF_REQ);
    sendMsg.Set8bit(ISI_MSG(MTC_POWER_OFF_REQ_OFFSET_FILL1), MTC_NORMAL_POWER_OFF);
    sendMsg.Set8bit(ISI_MSG(MTC_POWER_OFF_REQ_OFFSET_FILLERBYTE1), 0); // filler

#else // USE_MTC_SERVER
    TIsiKernelSend sendMsg(iDmcExtPtr->AllocateMsgBlock(MAX_ISI_MSG_SIZE));
    sendMsg.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,         PN_MODEM_MCE);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_TRANSID,            3);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_MESSAGEID,          MCE_POWER_OFF_REQ);
    sendMsg.Set8bit(ISI_MSG(MCE_POWER_OFF_REQ_OFFSET_FILLERBYTE1), 0);

#endif // USE_MTC_SERVER

     // Send message to receiver
    TInt err = iDmcExtPtr->Send(sendMsg.Complete());

    DMC_TRACE((("DMC:MCE_ISIMSG: McePowerOffReq() - err: %d"), err));

    DMC_TRACE_ASSERT_RESET(err == KErrNone, "Send failed", err);

    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_POWER_OFF_REQ_RETURN,
              "DMC:MCE_ISIMSG: McePowerOffReq() # OUT");
    DMC_TRACE(("DMC:MCE_ISIMSG: McePowerOffReq() # OUT"));
    }


//-----------------------------------------------------------
//  MceIsi::McePowerOffResp
//-----------------------------------------------------------
void MceIsi::McePowerOffResp(const TUint8* const aMsgPtr)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_POWER_OFF_RESP_ENTRY,
              "DMC:MCE_ISIMSG: MceResetResp() # IN");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceResetResp() # IN"));

#if USE_MTC_SERVER
    TUint8 trId(aMsgPtr[ISI_MSG(MTC_POWER_OFF_RESP_OFFSET_TRANSID)]);
    TUint8 status(aMsgPtr[ISI_MSG(MTC_POWER_OFF_RESP_OFFSET_STATUS)]);
#else // USE_MTC_SERVER
    TUint8 trId(aMsgPtr[ISI_MSG(MCE_RESET_RESP_OFFSET_TRANSID)]);
    TUint8 status(aMsgPtr[ISI_MSG(MCE_RESET_RESP_OFFSET_STATUS)]);
#endif // USE_MTC_SERVER
    /* Modem reset cannot fail, thus just print out 
       received information. */
    OstTraceExt2(TRACE_FLOW, DMC_MCE_ISI_MCE_POWER_OFF_RESP_1,
                 "DMC:MCE_ISIMSG: McePowerOff() - trId: %d, status: 0x%x",
                 trId, status);
    DMC_TRACE((("DMC:MCE_ISIMSG: McePowerOff() - trId: %d, status: 0x%x"),
                 trId, status));

    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_POWER_OFF_RETURN,
              "DMC:MCE_ISIMSG: McePowerOff() # OUT");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceResetResp() # OUT"));
    }


//-----------------------------------------------------------
//  MceIsi::MceResetReq
//-----------------------------------------------------------
void MceIsi::MceResetReq()
    {
    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_RESET_REQ_ENTRY,
              "DMC:MCE_ISIMSG: MceResetReq() # IN");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceResetReq() # IN"));

#if USE_MTC_SERVER
    TIsiKernelSend sendMsg(iDmcExtPtr->AllocateMsgBlock(MAX_ISI_MSG_SIZE));
    sendMsg.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,   PN_MTC);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_TRANSID,      2);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_MESSAGEID,    MTC_RESET_GENERATE_REQ);
    sendMsg.Set8bit(ISI_MSG(MTC_RESET_GENERATE_REQ_OFFSET_TYPE), MTC_STATE_VIA_RESET); // type
    sendMsg.Set8bit(ISI_MSG(MTC_RESET_GENERATE_REQ_OFFSET_STATE), MTC_LOCAL);           // state

#else // USE_MTC_SERVER
    TIsiKernelSend sendMsg(iDmcExtPtr->AllocateMsgBlock(MAX_ISI_MSG_SIZE));
    sendMsg.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,     PN_MODEM_MCE);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_TRANSID,        2);
    sendMsg.Set8bit(ISI_HEADER_OFFSET_MESSAGEID,      MCE_RESET_REQ_OFFSET_MESSAGEID);
    sendMsg.Set8bit(ISI_MSG(MCE_RESET_REQ_OFFSET_FILLERBYTE1), 0);

#endif // USE_MTC_SERVER

     // Send message to receiver
    TInt err = iDmcExtPtr->Send(sendMsg.Complete());

    DMC_TRACE((("DMC:MCE_ISIMSG: MceModemStateQueryReq() - err: %d"), err));

    DMC_TRACE_ASSERT_RESET(err == KErrNone, "Send failed", err);

    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_RESET_REQ_RETURN,
              "DMC:MCE_ISIMSG: MceResetReq() # OUT");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceResetReq() # OUT"));
    }


//-----------------------------------------------------------
//  MceIsi::MceResetResp
//-----------------------------------------------------------
void MceIsi::MceResetResp(const TUint8* const aMsgPtr)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_RESET_RESP_ENTRY,
              "DMC:MCE_ISIMSG: MceResetResp() # IN");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceResetResp() # IN"));

#if USE_MTC_SERVER
    TUint8 trId(aMsgPtr[ISI_MSG(MTC_RESET_GENERATE_RESP_OFFSET_TRANSID)]);
    TUint8 status(aMsgPtr[ISI_MSG(MTC_RESET_GENERATE_RESP_OFFSET_STATUS)]);

#else // USE_MTC_SERVER
    TUint8 trId(aMsgPtr[ISI_MSG(MCE_RESET_RESP_OFFSET_TRANSID)]);
    TUint8 status(aMsgPtr[ISI_MSG(MCE_RESET_RESP_OFFSET_STATUS)]);


#endif // USE_MTC_SERVER

    /* Modem reset cannot fail, thus just print out 
       received information. */
    OstTraceExt2(TRACE_FLOW, DMC_MCE_ISI_MCE_RESET_RESP_1,
                 "DMC:MCE_ISIMSG: MceResetResp() - trId: %d, status: 0x%x",
                 trId, status);
    DMC_TRACE((("DMC:MCE_ISIMSG: MceResetResp() - trId: %d, status: 0x%x"),
                 trId, status));

    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_RESET_RESP_RETURN,
              "DMC:MCE_ISIMSG: MceResetResp() # OUT");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceResetResp() # OUT"));
    }
    

//-----------------------------------------------------------
//  MceIsi::MceModemStateInd
//-----------------------------------------------------------
void MceIsi::MceModemStateInd(const TUint8* const aMsgPtr, TUint8& aModemState, TUint8& aModemAction)
    {
    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_STATE_IND_ENTRY,
              "DMC:MCE_ISIMSG: MceModemStateInd() # IN");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceModemStateInd() # IN"));

#if USE_MTC_SERVER
    TUint8 trId(aMsgPtr[ISI_MSG(MTC_STATE_INFO_IND_OFFSET_TRANSID)]);
    aModemState  = aMsgPtr[ISI_MSG(MTC_STATE_INFO_IND_OFFSET_STATE)];
    aModemAction = aMsgPtr[ISI_MSG(MTC_STATE_INFO_IND_OFFSET_ACTION)];

#else // USE_MTC_SERVER
        TUint8 trId(aMsgPtr[ISI_MSG(MCE_MODEM_STATE_IND_OFFSET_TRANSID)]);
    aModemState  = aMsgPtr[ISI_MSG(MCE_MODEM_STATE_IND_OFFSET_STATE)];
    aModemAction = aMsgPtr[ISI_MSG(MCE_MODEM_STATE_IND_OFFSET_ACTION)];

#endif // USE_MTC_SERVER

    OstTraceExt3(TRACE_FLOW, DMC_MCE_ISI_MCE_STATE_IND_1,
                 "DMC:MCE_ISIMSG: MceModemStateInd() - trId: %d, aModemState: 0x%x, aModemAction: 0x%x",
                 trId, aModemState, aModemAction);
    DMC_TRACE((("DMC:MCE_ISIMSG: MceModemStateInd() - trId: %d, aModemState: 0x%x, aModemAction: 0x%x"),
                 trId, aModemState, aModemAction));

    OstTrace0(TRACE_ENTRY_EXIT, DMC_MCE_ISI_MCE_STATE_IND_RETURN,
              "DMC:MCE_ISIMSG: MceModemStateInd() # OUT");
    DMC_TRACE(("DMC:MCE_ISIMSG: MceModemStateInd() # OUT"));
    }

// End of File
