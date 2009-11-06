/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class of all other test classes
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "cmmstaticutility.h"
#include "core.h"
#include "nwe.h"


// ==================== LOCAL FUNCTIONS =====================================

const TDesC& MapIpcToName(TInt aIpc)
    {
    const TDesC* name(NULL);
    switch (aIpc)
    {
     // Next IPC values are defined in et_clsvr.h
     case IPC_ETEL_CORE: { _LIT(KName, "EEtelOpenFromSession"); name=&KName; break; }
     case IPC_ETEL_CORE + 1: { _LIT(KName, "EEtelOpenFromSubSession"); name=&KName; break; }
     case IPC_ETEL_CORE + 2: { _LIT(KName, "EEtelOpenByNameFromSession"); name=&KName; break; }
     case IPC_ETEL_CORE + 3: { _LIT(KName, "EEtelOpenByNameFromSubSession"); name=&KName; break; }
     case IPC_ETEL_CORE + 4: { _LIT(KName, "EEtelClose"); name=&KName; break; }
     case IPC_ETEL_CORE + 5: { _LIT(KName, "EEtelCancelSubSession"); name=&KName; break; }
     case IPC_ETEL_CORE + 6: { _LIT(KName, "EETelLineCapsChangeNotification"); name=&KName; break; }
     case IPC_ETEL_CORE + 7: { _LIT(KName, "EETelLineCapsChangeNotificationCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 8: { _LIT(KName, "EETelPhoneCapsChangeNotification"); name=&KName; break; }
     case IPC_ETEL_CORE + 9: { _LIT(KName, "EETelPhoneCapsChangeNotificationCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 10: { _LIT(KName, "EEtelCallCapsChangeNotification"); name=&KName; break; }
     case IPC_ETEL_CORE + 11: { _LIT(KName, "EEtelCallCapsChangeNotificationCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 12: { _LIT(KName, "EEtelCallGetBearerServiceInfo"); name=&KName; break; }
     case IPC_ETEL_CORE + 13: { _LIT(KName, "EEtelCallGetCallDuration"); name=&KName; break; }
     case IPC_ETEL_CORE + 14: { _LIT(KName, "EEtelCallGetCallParams"); name=&KName; break; }
     case IPC_ETEL_CORE + 15: { _LIT(KName, "EEtelCallGetCaps"); name=&KName; break; }
     case IPC_ETEL_CORE + 16: { _LIT(KName, "EEtelCallGetFaxSettings"); name=&KName; break; }
     case IPC_ETEL_CORE + 17: { _LIT(KName, "EEtelCallGetInfo"); name=&KName; break; }
     case IPC_ETEL_CORE + 18: { _LIT(KName, "EEtelCallGetOwnershipStatus"); name=&KName; break; }
     case IPC_ETEL_CORE + 19: { _LIT(KName, "EEtelCallGetStatus"); name=&KName; break; }
     case IPC_ETEL_CORE + 20: { _LIT(KName, "EEtelCallNotifyDurationChange"); name=&KName; break; }
     case IPC_ETEL_CORE + 21: { _LIT(KName, "EEtelCallNotifyDurationChangeCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 22: { _LIT(KName, "EEtelCallNotifyHookChange"); name=&KName; break; }
     case IPC_ETEL_CORE + 23: { _LIT(KName, "EEtelCallNotifyHookChangeCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 24: { _LIT(KName, "EEtelCallNotifyStatusChange"); name=&KName; break; }
     case IPC_ETEL_CORE + 25: { _LIT(KName, "EEtelCallNotifyStatusChangeCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 26: { _LIT(KName, "EEtelCallReferenceCount"); name=&KName; break; }
     case IPC_ETEL_CORE + 27: { _LIT(KName, "EEtelDbgCheckHeap"); name=&KName; break; }
     case IPC_ETEL_CORE + 28: { _LIT(KName, "EEtelDbgDoDumpDebugInfo"); name=&KName; break; }
     case IPC_ETEL_CORE + 29: { _LIT(KName, "EEtelDbgFailNext"); name=&KName; break; }
     case IPC_ETEL_CORE + 30: { _LIT(KName, "EEtelDbgMarkEnd"); name=&KName; break; }
     case IPC_ETEL_CORE + 31: { _LIT(KName, "EEtelDbgMarkHeap"); name=&KName; break; }
     case IPC_ETEL_CORE + 32: { _LIT(KName, "EEtelDbgSetDebugPrintMask"); name=&KName; break; }
     case IPC_ETEL_CORE + 33: { _LIT(KName, "EEtelFaxWaitForEndOfPage"); name=&KName; break; }
     case IPC_ETEL_CORE + 34: { _LIT(KName, "EEtelLineEnumerateCall"); name=&KName; break; }
     case IPC_ETEL_CORE + 35: { _LIT(KName, "EEtelLineGetCallInfo"); name=&KName; break; }
     case IPC_ETEL_CORE + 36: { _LIT(KName, "EEtelLineGetCaps"); name=&KName; break; }
     case IPC_ETEL_CORE + 37: { _LIT(KName, "EEtelLineGetHookStatus"); name=&KName; break; }
     case IPC_ETEL_CORE + 38: { _LIT(KName, "EEtelLineGetInfo"); name=&KName; break; }
     case IPC_ETEL_CORE + 39: { _LIT(KName, "EEtelLineGetStatus"); name=&KName; break; }
     case IPC_ETEL_CORE + 40: { _LIT(KName, "EEtelLineNotifyCallAdded"); name=&KName; break; }
     case IPC_ETEL_CORE + 41: { _LIT(KName, "EEtelLineNotifyCallAddedCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 42: { _LIT(KName, "EEtelLineNotifyHookChange"); name=&KName; break; }
     case IPC_ETEL_CORE + 43: { _LIT(KName, "EEtelLineNotifyHookChangeCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 44: { _LIT(KName, "EEtelLineNotifyStatusChange"); name=&KName; break; }
     case IPC_ETEL_CORE + 45: { _LIT(KName, "EEtelLineNotifyStatusChangeCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 46: { _LIT(KName, "EEtelPhoneEnumerateLines"); name=&KName; break; }
     case IPC_ETEL_CORE + 47: { _LIT(KName, "EEtelPhoneGetCaps"); name=&KName; break; }
     case IPC_ETEL_CORE + 48: { _LIT(KName, "EEtelPhoneGetInfo"); name=&KName; break; }
     case IPC_ETEL_CORE + 49: { _LIT(KName, "EEtelPhoneGetLineInfo"); name=&KName; break; }
     case IPC_ETEL_CORE + 50: { _LIT(KName, "EEtelPhoneGetStatus"); name=&KName; break; }
     case IPC_ETEL_CORE + 51: { _LIT(KName, "EEtelPhoneNotifyModemDetected"); name=&KName; break; }
     case IPC_ETEL_CORE + 52: { _LIT(KName, "EEtelPhoneNotifyModemDetectedCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 53: { _LIT(KName, "EEtelServerClosePhoneModule"); name=&KName; break; }
     case IPC_ETEL_CORE + 54: { _LIT(KName, "EEtelServerEnumeratePhones"); name=&KName; break; }
     case IPC_ETEL_CORE + 55: { _LIT(KName, "EEtelServerGetTsyName"); name=&KName; break; }
     case IPC_ETEL_CORE + 56: { _LIT(KName, "EEtelServerGetTsyVersionNo"); name=&KName; break; }
     case IPC_ETEL_CORE + 57: { _LIT(KName, "EEtelServerLoadPhoneModule"); name=&KName; break; }
     case IPC_ETEL_CORE + 58: { _LIT(KName, "EEtelServerOpen"); name=&KName; break; }
     case IPC_ETEL_CORE + 59: { _LIT(KName, "EEtelServerPhoneInfoByIndex"); name=&KName; break; }
     case IPC_ETEL_CORE + 60: { _LIT(KName, "EEtelServerQueryTsyFunctionality"); name=&KName; break; }
     case IPC_ETEL_CORE + 61: { _LIT(KName, "EEtelServerSetExtendedErrorGranularity"); name=&KName; break; }
     case IPC_ETEL_CORE + 62: { _LIT(KName, "EEtelAdoptFaxSharedHeaderFile"); name=&KName; break; }
     case IPC_ETEL_CORE + 63: { _LIT(KName, "EEtelCallAcquireOwnership"); name=&KName; break; }
     case IPC_ETEL_CORE + 64: { _LIT(KName, "EEtelCallAcquireOwnershipCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 65: { _LIT(KName, "EEtelCallConnect"); name=&KName; break; }
     case IPC_ETEL_CORE + 66: { _LIT(KName, "EEtelCallConnectCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 67: { _LIT(KName, "EEtelCallHangUp"); name=&KName; break; }
     case IPC_ETEL_CORE + 68: { _LIT(KName, "EEtelCallHangUpCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 69: { _LIT(KName, "EEtelCallTransferOwnership"); name=&KName; break; }
     case IPC_ETEL_CORE + 70: { _LIT(KName, "EEtelFaxTerminateFaxSession"); name=&KName; break; }
     case IPC_ETEL_CORE + 71: { _LIT(KName, "EEtelLineNotifyIncomingCall"); name=&KName; break; }
     case IPC_ETEL_CORE + 72: { _LIT(KName, "EEtelLineNotifyIncomingCallCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 73: { _LIT(KName, "EEtelCallSetFaxSettings"); name=&KName; break; }
     case IPC_ETEL_CORE + 74: { _LIT(KName, "EEtelGlobalKernelObjectHandle"); name=&KName; break; }
     case IPC_ETEL_CORE + 75: { _LIT(KName, "EEtelPhoneInitialise"); name=&KName; break; }
     case IPC_ETEL_CORE + 76: { _LIT(KName, "EEtelPhoneInitialiseCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 77: { _LIT(KName, "EEtelServerSetPriorityClient"); name=&KName; break; }
     case IPC_ETEL_CORE + 78: { _LIT(KName, "EEtelCallAnswer"); name=&KName; break; }
     case IPC_ETEL_CORE + 79: { _LIT(KName, "EEtelCallAnswerCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 80: { _LIT(KName, "EEtelCallDial"); name=&KName; break; }
     case IPC_ETEL_CORE + 81: { _LIT(KName, "EEtelCallDialCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 82: { _LIT(KName, "EEtelCallLoanDataPort"); name=&KName; break; }
     case IPC_ETEL_CORE + 83: { _LIT(KName, "EEtelCallLoanDataPortCancel"); name=&KName; break; }
     case IPC_ETEL_CORE + 84: { _LIT(KName, "EEtelCallRecoverDataPort"); name=&KName; break; }
     case IPC_ETEL_CORE + 85: { _LIT(KName, "EEtelFaxRead"); name=&KName; break; }
     case IPC_ETEL_CORE + 86: { _LIT(KName, "EEtelFaxWrite"); name=&KName; break; }
     case IPC_ETEL_CORE + 87: { _LIT(KName, "EEtelDbgFailNextAvail"); name=&KName; break; }

     // Next IPC values are defined in etelmmcs.h
     case EEtelMMIpcOffset0: { _LIT(KName, "EMobileBroadcastMessagingGetCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 1: { _LIT(KName, "EMobileBroadcastMessagingNotifyIdListChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 2: { _LIT(KName, "EMobileBroadcastMessagingReceiveMessage"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 3: { _LIT(KName, "EMobileCallGetCurrentHscsdInfo"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 4: { _LIT(KName, "EMobileCallGetMobileCallCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 5: { _LIT(KName, "EMobileCallGetMobileCallStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 6: { _LIT(KName, "EMobileCallGetMobileDataCallCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 7: { _LIT(KName, "EMobileCallGetUUSCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 8: { _LIT(KName, "EMobileCallNotifyAlternatingCallSwitch"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 9: { _LIT(KName, "EMobileCallNotifyAudioToneEvent"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 10: { _LIT(KName, "EMobileCallNotifyCallEvent"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 11: { _LIT(KName, "EMobileCallNotifyHscsdInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 12: { _LIT(KName, "EMobileCallNotifyMobileCallCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 13: { _LIT(KName, "EMobileCallNotifyMobileCallStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 14: { _LIT(KName, "EMobileCallNotifyMobileDataCallCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 15: { _LIT(KName, "EMobileCallNotifyUUSCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 16: { _LIT(KName, "EMobileCallNotifyVoiceFallback"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 17: { _LIT(KName, "EMobileConferenceCallEnumerateCalls"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 18: { _LIT(KName, "EMobileConferenceCallGetCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 19: { _LIT(KName, "EMobileConferenceCallGetConferenceStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 20: { _LIT(KName, "EMobileConferenceCallNotifyCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 21: { _LIT(KName, "EMobileConferenceCallNotifyConferenceEvent"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 22: { _LIT(KName, "EMobileConferenceCallNotifyConferenceStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 23: { _LIT(KName, "EMobileLineGetMobileLineStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 24: { _LIT(KName, "EMobileLineNotifyMobileLineStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 25: { _LIT(KName, "EMobilePhoneEnumerateUSimApplications"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 26: { _LIT(KName, "EMobilePhoneGetALSLine"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 27: { _LIT(KName, "EMobilePhoneGetAlternatingCallCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 28: { _LIT(KName, "EMobilePhoneGetAlternatingCallMode"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 29: { _LIT(KName, "EMobilePhoneGetBatteryCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 30: { _LIT(KName, "EMobilePhoneGetBatteryInfo"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 31: { _LIT(KName, "EMobilePhoneGetCCBSStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 32: { _LIT(KName, "EMobilePhoneGetCallForwardingIndicator"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 33: { _LIT(KName, "EMobilePhoneGetCallServiceCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 34: { _LIT(KName, "EMobilePhoneGetCipheringIndicatorStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 35: { _LIT(KName, "EMobilePhoneGetCompMethodName"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 36: { _LIT(KName, "EMobilePhoneGetCostCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 37: { _LIT(KName, "EMobilePhoneGetCurrentMode"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 38: { _LIT(KName, "EMobilePhoneGetCustomerServiceProfile"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 39: { _LIT(KName, "EMobilePhoneGetDTMFCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 40: { _LIT(KName, "EMobilePhoneGetFdnStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 41: { _LIT(KName, "EMobilePhoneGetHomeNetworkSearchPeriod"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 42: { _LIT(KName, "EMobilePhoneGetIccAccessCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 43: { _LIT(KName, "EMobilePhoneGetIdentityCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 44: { _LIT(KName, "EMobilePhoneGetIdentityServiceStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 45: { _LIT(KName, "EMobilePhoneGetIncomingCallType"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 46: { _LIT(KName, "EMobilePhoneGetIndicator"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 47: { _LIT(KName, "EMobilePhoneGetIndicatorCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 48: { _LIT(KName, "EMobilePhoneGetLastUsedAccessTechnology"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 49: { _LIT(KName, "EMobilePhoneGetMulticallParams"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 50: { _LIT(KName, "EMobilePhoneGetMultimediaCallPreference"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 51: { _LIT(KName, "EMobilePhoneGetMultimodeCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 52: { _LIT(KName, "EMobilePhoneGetNetworkCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 53: { _LIT(KName, "EMobilePhoneGetNetworkInvScanSetting"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 54: { _LIT(KName, "EMobilePhoneGetNetworkRegistrationStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 55: { _LIT(KName, "EMobilePhoneGetNetworkSecurityLevel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 56: { _LIT(KName, "EMobilePhoneGetPersonalisationCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 57: { _LIT(KName, "EMobilePhoneGetSecurityCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 58: { _LIT(KName, "EMobilePhoneGetServiceTable"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 59: { _LIT(KName, "EMobilePhoneGetSignalCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 60: { _LIT(KName, "EMobilePhoneGetSignalStrength"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 61: { _LIT(KName, "EMobilePhoneGetUSimApplicationsInfo"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 62: { _LIT(KName, "EMobilePhoneGetUSimAppsSelectionMode"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 63: { _LIT(KName, "EMobilePhoneGetUUSSetting"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 64: { _LIT(KName, "EMobilePhoneNotifyALSLineChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 65: { _LIT(KName, "EMobilePhoneNotifyAlternatingCallCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 66: { _LIT(KName, "EMobilePhoneNotifyAlternatingCallModeChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 67: { _LIT(KName, "EMobilePhoneNotifyBatteryInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 68: { _LIT(KName, "EMobilePhoneNotifyCCBSRecall"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 69: { _LIT(KName, "EMobilePhoneNotifyCCBSStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 70: { _LIT(KName, "EMobilePhoneNotifyCallBarringStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 71: { _LIT(KName, "EMobilePhoneNotifyCallForwardingActive"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 72: { _LIT(KName, "EMobilePhoneNotifyCallForwardingStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 73: { _LIT(KName, "EMobilePhoneNotifyCallServiceCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 74: { _LIT(KName, "EMobilePhoneNotifyCallWaitingStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 75: { _LIT(KName, "EMobilePhoneNotifyCipheringIndicatorStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 76: { _LIT(KName, "EMobilePhoneNotifyCostCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 77: { _LIT(KName, "EMobilePhoneNotifyDTMFCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 78: { _LIT(KName, "EMobilePhoneNotifyDTMFEvent"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 79: { _LIT(KName, "EMobilePhoneNotifyFdnStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 80: { _LIT(KName, "EMobilePhoneNotifyIccAccessCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 81: { _LIT(KName, "EMobilePhoneNotifyIdentityServiceStatus"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 82: { _LIT(KName, "EMobilePhoneNotifyIdentitySuppressionRejected"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 83: { _LIT(KName, "EMobilePhoneNotifyIncomingCallTypeChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 84: { _LIT(KName, "EMobilePhoneNotifyIndicatorChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 85: { _LIT(KName, "EMobilePhoneNotifyMessageWaiting"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 86: { _LIT(KName, "EMobilePhoneNotifyModeChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 87: { _LIT(KName, "EMobilePhoneNotifyMulticallIndicatorChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 88: { _LIT(KName, "EMobilePhoneNotifyMulticallParamsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 89: { _LIT(KName, "EMobilePhoneNotifyMultimediaCallPreferenceChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 90: { _LIT(KName, "EMobilePhoneNotifyNetworkInvScanChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 91: { _LIT(KName, "EMobilePhoneNotifyNetworkInvScanEvent"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 92: { _LIT(KName, "EMobilePhoneNotifyNetworkRegistrationStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 93: { _LIT(KName, "EMobilePhoneNotifyNetworkSecurityLevelChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 94: { _LIT(KName, "EMobilePhoneNotifySecurityCapsChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 95: { _LIT(KName, "EMobilePhoneNotifySecurityEvent"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 96: { _LIT(KName, "EMobilePhoneNotifySendNetworkServiceRequest"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 97: { _LIT(KName, "EMobilePhoneNotifySignalStrengthChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 98: { _LIT(KName, "EMobilePhoneNotifyStorePreferredNetworksListChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 99: { _LIT(KName, "EMobilePhoneNotifyUSimApplicationsInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 100: { _LIT(KName, "EMobilePhoneNotifyUSimAppsSelectionModeChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 101: { _LIT(KName, "EMobilePhoneNotifyUUSSettingChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 102: { _LIT(KName, "EMobileSmsMessagingEnumerateMessageStores"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 103: { _LIT(KName, "EMobileSmsMessagingGetCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 104: { _LIT(KName, "EMobileSmsMessagingGetMessageStoreInfo"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 105: { _LIT(KName, "EMobileSmsMessagingGetMoSmsBearer"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 106: { _LIT(KName, "EMobileSmsMessagingGetReceiveMode"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 107: { _LIT(KName, "EMobileSmsMessagingNotifyMoSmsBearerChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 108: { _LIT(KName, "EMobileSmsMessagingNotifyReceiveModeChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 109: { _LIT(KName, "EMobileSmsMessagingNotifySmspListChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 110: { _LIT(KName, "EMobileUssdMessagingGetCaps"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 111: { _LIT(KName, "EMobileUssdMessagingNotifyNetworkRelease"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 112: { _LIT(KName, "EMobilePhoneNotifyImsAuthorizationInfoChanged"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 113: { _LIT(KName, "EMobilePhoneEnumerateSmartCardApplications"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 114: { _LIT(KName, "EMobilePhoneGetSmartCardApplicationInfo"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 115: { _LIT(KName, "EMobilePhoneNotifySmartCardApplicationInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset0 + 116: { _LIT(KName, "EMobilePhoneGetCurrentActiveUSimApplication"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileBroadcastMessagingNotifyIdListChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileBroadcastMessagingReceiveMessageCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 8: { _LIT(KName, "EMobileCallNotifyAlternatingCallSwitchCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 9: { _LIT(KName, "EMobileCallNotifyAudioToneEventCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 10: { _LIT(KName, "EMobileCallNotifyCallEventCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 11: { _LIT(KName, "EMobileCallNotifyHscsdInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 12: { _LIT(KName, "EMobileCallNotifyMobileCallCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 13: { _LIT(KName, "EMobileCallNotifyMobileCallStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 14: { _LIT(KName, "EMobileCallNotifyMobileDataCallCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 15: { _LIT(KName, "EMobileCallNotifyUUSCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 16: { _LIT(KName, "EMobileCallNotifyVoiceFallbackCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 20: { _LIT(KName, "EMobileConferenceCallNotifyCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 21: { _LIT(KName, "EMobileConferenceCallNotifyConferenceEventCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 22: { _LIT(KName, "EMobileConferenceCallNotifyConferenceStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 24: { _LIT(KName, "EMobileLineNotifyMobileLineStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 25: { _LIT(KName, "EMobilePhoneEnumerateUSimApplicationsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 30: { _LIT(KName, "EMobilePhoneGetBatteryInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 31: { _LIT(KName, "EMobilePhoneGetCCBSStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 32: { _LIT(KName, "EMobilePhoneGetCallForwardingIndicatorCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 34: { _LIT(KName, "EMobilePhoneGetCipheringIndicatorStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 35: { _LIT(KName, "EMobilePhoneGetCompMethodNameCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 38: { _LIT(KName, "EMobilePhoneGetCustomerServiceProfileCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 40: { _LIT(KName, "EMobilePhoneGetFdnStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 41: { _LIT(KName, "EMobilePhoneGetHomeNetworkSearchPeriodCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 44: { _LIT(KName, "EMobilePhoneGetIdentityServiceStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 45: { _LIT(KName, "EMobilePhoneGetIncomingCallTypeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 46: { _LIT(KName, "EMobilePhoneGetIndicatorCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 48: { _LIT(KName, "EMobilePhoneGetLastUsedAccessTechnologyCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 49: { _LIT(KName, "EMobilePhoneGetMulticallParamsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 50: { _LIT(KName, "EMobilePhoneGetMultimediaCallPreferenceCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 54: { _LIT(KName, "EMobilePhoneGetNetworkRegistrationStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 55: { _LIT(KName, "EMobilePhoneGetNetworkSecurityLevelCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 56: { _LIT(KName, "EMobilePhoneGetPersonalisationCapsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 58: { _LIT(KName, "EMobilePhoneGetServiceTableCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 60: { _LIT(KName, "EMobilePhoneGetSignalStrengthCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 61: { _LIT(KName, "EMobilePhoneGetUSimApplicationsInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 62: { _LIT(KName, "EMobilePhoneGetUSimAppsSelectionModeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 63: { _LIT(KName, "EMobilePhoneGetUUSSettingCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 64: { _LIT(KName, "EMobilePhoneNotifyALSLineChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 65: { _LIT(KName, "EMobilePhoneNotifyAlternatingCallCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 66: { _LIT(KName, "EMobilePhoneNotifyAlternatingCallModeChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 67: { _LIT(KName, "EMobilePhoneNotifyBatteryInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 68: { _LIT(KName, "EMobilePhoneNotifyCCBSRecallCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 69: { _LIT(KName, "EMobilePhoneNotifyCCBSStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 70: { _LIT(KName, "EMobilePhoneNotifyCallBarringStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 71: { _LIT(KName, "EMobilePhoneNotifyCallForwardingActiveCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 72: { _LIT(KName, "EMobilePhoneNotifyCallForwardingStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 73: { _LIT(KName, "EMobilePhoneNotifyCallServiceCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 74: { _LIT(KName, "EMobilePhoneNotifyCallWaitingStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 75: { _LIT(KName, "EMobilePhoneNotifyCipheringIndicatorStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 76: { _LIT(KName, "EMobilePhoneNotifyCostCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 77: { _LIT(KName, "EMobilePhoneNotifyDTMFCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 78: { _LIT(KName, "EMobilePhoneNotifyDTMFEventCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 79: { _LIT(KName, "EMobilePhoneNotifyFdnStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 80: { _LIT(KName, "EMobilePhoneNotifyIccAccessCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 81: { _LIT(KName, "EMobilePhoneNotifyIdentityServiceStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 82: { _LIT(KName, "EMobilePhoneNotifyIdentitySuppressionRejectedCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 83: { _LIT(KName, "EMobilePhoneNotifyIncomingCallTypeChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 84: { _LIT(KName, "EMobilePhoneNotifyIndicatorChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 85: { _LIT(KName, "EMobilePhoneNotifyMessageWaitingCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 86: { _LIT(KName, "EMobilePhoneNotifyModeChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 87: { _LIT(KName, "EMobilePhoneNotifyMulticallIndicatorChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 88: { _LIT(KName, "EMobilePhoneNotifyMulticallParamsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 89: { _LIT(KName, "EMobilePhoneNotifyMultimediaCallPreferenceChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 90: { _LIT(KName, "EMobilePhoneNotifyNetworkInvScanChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 91: { _LIT(KName, "EMobilePhoneNotifyNetworkInvScanEventCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 92: { _LIT(KName, "EMobilePhoneNotifyNetworkRegistrationStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 93: { _LIT(KName, "EMobilePhoneNotifyNetworkSecurityLevelChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 94: { _LIT(KName, "EMobilePhoneNotifySecurityCapsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 95: { _LIT(KName, "EMobilePhoneNotifySecurityEventCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 96: { _LIT(KName, "EMobilePhoneNotifySendNetworkServiceRequestCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 97: { _LIT(KName, "EMobilePhoneNotifySignalStrengthChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 98: { _LIT(KName, "EMobilePhoneNotifyStorePreferredNetworksListChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 99: { _LIT(KName, "EMobilePhoneNotifyUSimApplicationsInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 100: { _LIT(KName, "EMobilePhoneNotifyUSimAppsSelectionModeChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 101: { _LIT(KName, "EMobilePhoneNotifyUUSSettingChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 104: { _LIT(KName, "EMobileSmsMessagingGetMessageStoreInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 107: { _LIT(KName, "EMobileSmsMessagingNotifyMoSmsBearerChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 108: { _LIT(KName, "EMobileSmsMessagingNotifyReceiveModeChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 109: { _LIT(KName, "EMobileSmsMessagingNotifySmspListChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 111: { _LIT(KName, "EMobileUssdMessagingNotifyNetworkReleaseCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 112: { _LIT(KName, "EMobilePhoneNotifyImsAuthorizationInfoChangedCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 113: { _LIT(KName, "EMobilePhoneEnumerateSmartCardApplicationsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 114: { _LIT(KName, "EMobilePhoneGetSmartCardApplicationInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 115: { _LIT(KName, "EMobilePhoneNotifySmartCardApplicationInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset0 + EMobileCancelOffset + 116: { _LIT(KName, "EMobilePhoneGetCurrentActiveUSimApplicationCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1: { _LIT(KName, "EMobileCallActivateCCBS"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 1: { _LIT(KName, "EMobileCallAnswerISV"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 2: { _LIT(KName, "EMobileCallAnswerMultimediaCallAsVoice"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 3: { _LIT(KName, "EMobileCallDeflect"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 4: { _LIT(KName, "EMobileCallDialISV"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 5: { _LIT(KName, "EMobileCallGoOneToOne"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 6: { _LIT(KName, "EMobileCallHold"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 7: { _LIT(KName, "EMobileCallNotifyPrivacyConfirmation"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 8: { _LIT(KName, "EMobileCallNotifyTrafficChannelConfirmation"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 9: { _LIT(KName, "EMobileCallRejectCCBS"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 10: { _LIT(KName, "EMobileCallResume"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 11: { _LIT(KName, "EMobileCallSetDynamicHscsdParams"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 12: { _LIT(KName, "EMobileCallSetPrivacy"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 13: { _LIT(KName, "EMobileCallSetTrafficChannel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 14: { _LIT(KName, "EMobileCallSwap"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 15: { _LIT(KName, "EMobileCallSwitchAlternatingCall"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 16: { _LIT(KName, "EMobileCallTransfer"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 17: { _LIT(KName, "EMobileConferenceCallAddCall"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 18: { _LIT(KName, "EMobileConferenceCallCreateConference"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 19: { _LIT(KName, "EMobileConferenceCallHangUp"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 20: { _LIT(KName, "EMobileConferenceCallSwap"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 21: { _LIT(KName, "EMobilePhoneAcceptCCBSRecall"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 22: { _LIT(KName, "EMobilePhoneContinueDTMFStringSending"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 23: { _LIT(KName, "EMobilePhoneNotifyStopInDTMFString"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 24: { _LIT(KName, "EMobilePhoneReadDTMFTones"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 25: { _LIT(KName, "EMobilePhoneRefuseCCBSRecall"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 26: { _LIT(KName, "EMobilePhoneSendDTMFTones"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 27: { _LIT(KName, "EMobilePhoneStartDTMFTone"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 28: { _LIT(KName, "EMobilePhoneStopDTMFTone"); name=&KName; break; }
     case EEtelMMIpcOffset1 + 29: { _LIT(KName, "EMobilePhoneTerminateAllCalls"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset: { _LIT(KName, "EMobileCallActivateCCBSCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileCallAnswerISVCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileCallAnswerMultimediaCallAsVoiceCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 3: { _LIT(KName, "EMobileCallDeflectCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 4: { _LIT(KName, "EMobileCallDialISVCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 5: { _LIT(KName, "EMobileCallGoOneToOneCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 6: { _LIT(KName, "EMobileCallHoldCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 7: { _LIT(KName, "EMobileCallNotifyPrivacyConfirmationCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 8: { _LIT(KName, "EMobileCallNotifyTrafficChannelConfirmationCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 10: { _LIT(KName, "EMobileCallResumeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 11: { _LIT(KName, "EMobileCallSetDynamicHscsdParamsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 14: { _LIT(KName, "EMobileCallSwapCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 15: { _LIT(KName, "EMobileCallSwitchAlternatingCallCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 16: { _LIT(KName, "EMobileCallTransferCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 17: { _LIT(KName, "EMobileConferenceCallAddCallCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 18: { _LIT(KName, "EMobileConferenceCallCreateConferenceCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 19: { _LIT(KName, "EMobileConferenceCallHangUpCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 20: { _LIT(KName, "EMobileConferenceCallSwapCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 21: { _LIT(KName, "EMobilePhoneAcceptCCBSRecallCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 23: { _LIT(KName, "EMobilePhoneNotifyStopInDTMFStringCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 24: { _LIT(KName, "EMobilePhoneReadDTMFTonesCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 26: { _LIT(KName, "EMobilePhoneSendDTMFTonesCancel"); name=&KName; break; }
     case EEtelMMIpcOffset1 + EMobileCancelOffset + 29: { _LIT(KName, "EMobilePhoneTerminateAllCallsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset2: { _LIT(KName, "EMobileConferenceCallGetMobileCallInfo"); name=&KName; break; }
     case EEtelMMIpcOffset2 + 1: { _LIT(KName, "EMobilePhoneEnumerateAPNEntries"); name=&KName; break; }
     case EEtelMMIpcOffset2 + 2: { _LIT(KName, "EMobilePhoneGetAPNname"); name=&KName; break; }
     case EEtelMMIpcOffset2 + 3: { _LIT(KName, "EMobilePhoneGetFeatureCode"); name=&KName; break; }
     case EEtelMMIpcOffset17: { _LIT(KName, "EMobilePhoneGetHomeNetwork"); name=&KName; break; }
     case EEtelMMIpcOffset18: { _LIT(KName, "EMobilePhoneGetIccMessageWaitingIndicators"); name=&KName; break; }
     case EEtelMMIpcOffset18 + 1: { _LIT(KName, "EMobilePhoneGetLockInfo"); name=&KName; break; }
     case EEtelMMIpcOffset18 + 2: { _LIT(KName, "EMobilePhoneGetMmsConfig"); name=&KName; break; }
     case EEtelMMIpcOffset18 + 3: { _LIT(KName, "EMobilePhoneGetNITZInfo"); name=&KName; break; }
     case EEtelMMIpcOffset19: { _LIT(KName, "EMobilePhoneGetNetworkName"); name=&KName; break; }
     case EEtelMMIpcOffset20: { _LIT(KName, "EMobilePhoneGetNetworkSelectionSetting"); name=&KName; break; }
     case EEtelMMIpcOffset20 + 1: { _LIT(KName, "EMobilePhoneGetPersonalisationStatus"); name=&KName; break; }
     case EEtelMMIpcOffset15: { _LIT(KName, "EMobilePhoneGetPhoneId"); name=&KName; break; }
     case EEtelMMIpcOffset16: { _LIT(KName, "EMobilePhoneGetPhoneStoreInfo"); name=&KName; break; }
     case EEtelMMIpcOffset16 + 1: { _LIT(KName, "EMobilePhoneGetPreferredNetworksPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset16 + 2: { _LIT(KName, "EMobilePhoneGetPreferredNetworksPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset21: { _LIT(KName, "EMobilePhoneGetServiceProviderName"); name=&KName; break; }
     case EEtelMMIpcOffset22: { _LIT(KName, "EMobilePhoneGetSubscriberId"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 1: { _LIT(KName, "EMobilePhoneNotifyIccMessageWaitingIndicatorsChange"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 2: { _LIT(KName, "EMobilePhoneNotifyLockInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 3: { _LIT(KName, "EMobilePhoneNotifyMmsConfig"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 4: { _LIT(KName, "EMobilePhoneNotifyMmsUpdate"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 5: { _LIT(KName, "EMobilePhoneNotifyNITZInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 6: { _LIT(KName, "EMobilePhoneNotifyNetworkSelectionSettingChange"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 7: { _LIT(KName, "EMobilePhoneStoreReadAllPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 8: { _LIT(KName, "EMobilePhoneStoreReadAllPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 9: { _LIT(KName, "EMobilePhoneStoreReadAllV4Phase1"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 10: { _LIT(KName, "EMobilePhoneStoreReadAllV4Phase2"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 11: { _LIT(KName, "EMobileSmsMessagingGetSmspListPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 12: { _LIT(KName, "EMobileSmsMessagingGetSmspListPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 13: { _LIT(KName, "EMobilePhoneAuthorizationInfoPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 14: { _LIT(KName, "EMobilePhoneAuthorizationInfoPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 15: { _LIT(KName, "EMobilePhoneIMSAuthenticate"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 16: { _LIT(KName, "EMobilePhoneGetScFileInfo"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 17: { _LIT(KName, "EMobilePhoneReadScFile"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 18: { _LIT(KName, "EMobilePhoneGetIccIdentity"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 19: { _LIT(KName, "EMobilePhoneGetSecurityCodeInfo"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 20: { _LIT(KName, "EMobilePhoneNotifySecurityCodeInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 21: { _LIT(KName, "EMobilePhoneSmartCardAuthenticate"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 22: { _LIT(KName, "EMobileSmartCardEapGetAuthenticationStatus"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 23: { _LIT(KName, "EMobileSmartCardEapGetUserIdentity"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 24: { _LIT(KName, "EMobileSmartCardEapGetEapKey"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 25: { _LIT(KName, "EMobileSmartCardEapNotifyEapMethodAccessStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 26: { _LIT(KName, "EMobileSmartCardEapAuthenticationPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 27: { _LIT(KName, "EMobileSmartCardEapAuthenticationPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 28: { _LIT(KName, "EMobileSmartCardEapReleaseEapMethod"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 29: { _LIT(KName, "EMobileSmartCardEapGetEapMethodAccessStatus"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 30: { _LIT(KName, "EMobilePhoneNotifyAPNListChanged"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 31: { _LIT(KName, "EMobilePhoneGetAPNControlListServiceStatus"); name=&KName; break; }
     case EEtelMMIpcOffset22 + 32: { _LIT(KName, "EMobilePhoneNotifyAPNControlListServiceStatusChange"); name=&KName; break; }
     case EEtelMMIpcOffset2 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneEnumerateAPNEntriesCancel"); name=&KName; break; }
     case EEtelMMIpcOffset2 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneGetAPNnameCancel"); name=&KName; break; }
     case EEtelMMIpcOffset2 + EMobileCancelOffset + 3: { _LIT(KName, "EMobilePhoneGetFeatureCodeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset17 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetHomeNetworkCancel"); name=&KName; break; }
     case EEtelMMIpcOffset18 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetIccMessageWaitingIndicatorsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset18 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneGetLockInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset18 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneGetMmsConfigCancel"); name=&KName; break; }
     case EEtelMMIpcOffset19 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetNetworkNameCancel"); name=&KName; break; }
     case EEtelMMIpcOffset20 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneGetPersonalisationStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset15 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetPhoneIdCancel"); name=&KName; break; }
     case EEtelMMIpcOffset16 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetPhoneStoreInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset16 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneGetPreferredNetworksCancel"); name=&KName; break; }
     case EEtelMMIpcOffset21 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetServiceProviderNameCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetSubscriberIdCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneNotifyIccMessageWaitingIndicatorsChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneNotifyLockInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 3: { _LIT(KName, "EMobilePhoneNotifyMmsConfigCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 4: { _LIT(KName, "EMobilePhoneNotifyMmsUpdateCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 5: { _LIT(KName, "EMobilePhoneNotifyNITZInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 6: { _LIT(KName, "EMobilePhoneNotifyNetworkSelectionSettingChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 7: { _LIT(KName, "EMobilePhoneStoreReadAllCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 9: { _LIT(KName, "EMobilePhoneStoreReadAllV4Cancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 11: { _LIT(KName, "EMobileSmsMessagingGetSmspListCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 13: { _LIT(KName, "EMobilePhoneAuthorizationInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 15: { _LIT(KName, "EMobilePhoneIMSAuthenticateCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 16: { _LIT(KName, "EMobilePhoneGetScFileInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 17: { _LIT(KName, "EMobilePhoneReadScFileCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 18: { _LIT(KName, "EMobilePhoneGetIccIdentityCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 19: { _LIT(KName, "EMobilePhoneGetSecurityCodeInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 20: { _LIT(KName, "EMobilePhoneNotifySecurityCodeInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 21: { _LIT(KName, "EMobilePhoneSmartCardAuthenticateCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 22: { _LIT(KName, "EMobileSmartCardEapGetAuthenticationStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 23: { _LIT(KName, "EMobileSmartCardEapGetUserIdentityCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 24: { _LIT(KName, "EMobileSmartCardEapGetEapKeyCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 25: { _LIT(KName, "EMobileSmartCardEapNotifyEapMethodAccessStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 26: { _LIT(KName, "EMobileSmartCardEapAuthenticationCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 30: { _LIT(KName, "EMobilePhoneNotifyAPNListChangedCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 31: { _LIT(KName, "EMobilePhoneGetAPNControlListServiceStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset22 + EMobileCancelOffset + 32: { _LIT(KName, "EMobilePhoneNotifyAPNControlListServiceStatusChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3: { _LIT(KName, "EMobileNamStoreSetActiveNam"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 1: { _LIT(KName, "EMobileNamStoreStoreAll"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 2: { _LIT(KName, "EMobileNamStoreStoreAllV4"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 3: { _LIT(KName, "EMobilePhoneAbortSecurityCode"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 4: { _LIT(KName, "EMobilePhoneChangeSecurityCode"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 5: { _LIT(KName, "EMobilePhoneClearBlacklist"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 6: { _LIT(KName, "EMobilePhoneClearCostMeter"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 7: { _LIT(KName, "EMobilePhoneProgramFeatureCode"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 8: { _LIT(KName, "EMobilePhoneSetALSLine"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 9: { _LIT(KName, "EMobilePhoneSetAlternatingCallMode"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 10: { _LIT(KName, "EMobilePhoneSetFdnSetting"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 11: { _LIT(KName, "EMobilePhoneSetIccMessageWaitingIndicators"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 12: { _LIT(KName, "EMobilePhoneSetIdentityServiceStatus"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 13: { _LIT(KName, "EMobilePhoneSetIncomingCallType"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 14: { _LIT(KName, "EMobilePhoneSetLockSetting"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 15: { _LIT(KName, "EMobilePhoneSetMaxCostMeter"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 16: { _LIT(KName, "EMobilePhoneSetMmsUserConnParams"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 17: { _LIT(KName, "EMobilePhoneSetMmsUserPreferences"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 18: { _LIT(KName, "EMobilePhoneSetMulticallParams"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 19: { _LIT(KName, "EMobilePhoneSetMultimediaCallPreference"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 20: { _LIT(KName, "EMobilePhoneSetNetworkSelectionSetting"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 21: { _LIT(KName, "EMobilePhoneSetPuct"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 22: { _LIT(KName, "EMobilePhoneSetUSimApplicationStatus"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 23: { _LIT(KName, "EMobilePhoneSetUSimAppsSelectionMode"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 24: { _LIT(KName, "EMobilePhoneStorePreferredNetworksList"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 25: { _LIT(KName, "EMobilePhoneVerifySecurityCode"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 26: { _LIT(KName, "EMobileSmsMessagingSetMoSmsBearer"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 27: { _LIT(KName, "EMobileSmsMessagingSetReceiveMode"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 28: { _LIT(KName, "EMobileSmsMessagingStoreSmspList"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 29: { _LIT(KName, "EMobilePhoneSetSmartCardApplicationStatus"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 30: { _LIT(KName, "EMobilePhoneUpdateScFile"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 31: { _LIT(KName, "EMobilePhoneAppendAPNName"); name=&KName; break; }
     case EEtelMMIpcOffset3 + 32: { _LIT(KName, "EMobilePhoneDeleteAPNName"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset: { _LIT(KName, "EMobileNamStoreSetActiveNamCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileNamStoreStoreAllCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileNamStoreStoreAllV4Cancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 4: { _LIT(KName, "EMobilePhoneChangeSecurityCodeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 5: { _LIT(KName, "EMobilePhoneClearBlacklistCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 6: { _LIT(KName, "EMobilePhoneClearCostMeterCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 7: { _LIT(KName, "EMobilePhoneProgramFeatureCodeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 8: { _LIT(KName, "EMobilePhoneSetALSLineCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 9: { _LIT(KName, "EMobilePhoneSetAlternatingCallModeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 10: { _LIT(KName, "EMobilePhoneSetFdnSettingCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 11: { _LIT(KName, "EMobilePhoneSetIccMessageWaitingIndicatorsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 12: { _LIT(KName, "EMobilePhoneSetIdentityServiceStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 13: { _LIT(KName, "EMobilePhoneSetIncomingCallTypeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 14: { _LIT(KName, "EMobilePhoneSetLockSettingCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 15: { _LIT(KName, "EMobilePhoneSetMaxCostMeterCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 16: { _LIT(KName, "EMobilePhoneSetMmsUserConnParamsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 17: { _LIT(KName, "EMobilePhoneSetMmsUserPreferencesCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 18: { _LIT(KName, "EMobilePhoneSetMulticallParamsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 19: { _LIT(KName, "EMobilePhoneSetMultimediaCallPreferenceCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 20: { _LIT(KName, "EMobilePhoneSetNetworkSelectionSettingCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 21: { _LIT(KName, "EMobilePhoneSetPuctCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 22: { _LIT(KName, "EMobilePhoneSetUSimApplicationStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 23: { _LIT(KName, "EMobilePhoneSetUSimAppsSelectionModeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 24: { _LIT(KName, "EMobilePhoneStorePreferredNetworksListCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 25: { _LIT(KName, "EMobilePhoneVerifySecurityCodeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 26: { _LIT(KName, "EMobileSmsMessagingSetMoSmsBearerCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 27: { _LIT(KName, "EMobileSmsMessagingSetReceiveModeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 28: { _LIT(KName, "EMobileSmsMessagingStoreSmspListCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 29: { _LIT(KName, "EMobilePhoneSetSmartCardApplicationStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 30: { _LIT(KName, "EMobilePhoneUpdateScFileCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 31: { _LIT(KName, "EMobilePhoneAppendAPNNameCancel"); name=&KName; break; }
     case EEtelMMIpcOffset3 + EMobileCancelOffset + 32: { _LIT(KName, "EMobilePhoneDeleteAPNNameCancel"); name=&KName; break; }
     case EEtelMMIpcOffset4: { _LIT(KName, "EMobileBroadcastMessagingGetFilterSetting"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 1: { _LIT(KName, "EMobileBroadcastMessagingGetIdListPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 2: { _LIT(KName, "EMobileBroadcastMessagingGetIdListPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 3: { _LIT(KName, "EMobileBroadcastMessagingGetLanguageFilter"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 4: { _LIT(KName, "EMobileBroadcastMessagingNotifyFilterSettingChange"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 5: { _LIT(KName, "EMobileBroadcastMessagingNotifyLanguageFilterChange"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 6: { _LIT(KName, "EMobileCallGetMobileCallInfo"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 7: { _LIT(KName, "EMobileCallNotifyRemotePartyInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 8: { _LIT(KName, "EMobilePhoneBookStoreRead"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 9: { _LIT(KName, "EMobilePhoneGetAirTimeDuration"); name=&KName; break; }
     case EEtelMMIpcOffset4 + 10: { _LIT(KName, "EMobilePhoneGetCostInfo"); name=&KName; break; }
     case EEtelMMIpcOffset23: { _LIT(KName, "EMobilePhoneGetCurrentNetworkNoLocation"); name=&KName; break; }
     case EEtelMMIpcOffset24: { _LIT(KName, "EMobilePhoneGetMailboxNumbers"); name=&KName; break; }
     case EEtelMMIpcOffset24 + 1: { _LIT(KName, "EMobilePhoneNotifyAirTimeDurationChange"); name=&KName; break; }
     case EEtelMMIpcOffset24 + 2: { _LIT(KName, "EMobilePhoneNotifyCostInfoChange"); name=&KName; break; }
     case EEtelMMIpcOffset25: { _LIT(KName, "EMobilePhoneNotifyCurrentNetworkNoLocationChange"); name=&KName; break; }
     case EEtelMMIpcOffset26: { _LIT(KName, "EMobilePhoneNotifyMailboxNumbersChange"); name=&KName; break; }
     case EEtelMMIpcOffset26 + 1: { _LIT(KName, "EMobilePhoneStoreGetInfo"); name=&KName; break; }
     case EEtelMMIpcOffset26 + 2: { _LIT(KName, "EMobilePhoneStoreNotifyStoreEvent"); name=&KName; break; }
     case EEtelMMIpcOffset26 + 3: { _LIT(KName, "EMobilePhoneStoreRead"); name=&KName; break; }
     case EEtelMMIpcOffset4 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileBroadcastMessagingGetIdListCancel"); name=&KName; break; }
     case EEtelMMIpcOffset4 + EMobileCancelOffset + 3: { _LIT(KName, "EMobileBroadcastMessagingGetLanguageFilterCancel"); name=&KName; break; }
     case EEtelMMIpcOffset4 + EMobileCancelOffset + 4: { _LIT(KName, "EMobileBroadcastMessagingNotifyFilterSettingChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset4 + EMobileCancelOffset + 5: { _LIT(KName, "EMobileBroadcastMessagingNotifyLanguageFilterChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset4 + EMobileCancelOffset + 6: { _LIT(KName, "EMobileCallGetMobileCallInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset4 + EMobileCancelOffset + 7: { _LIT(KName, "EMobileCallNotifyRemotePartyInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset4 + EMobileCancelOffset + 10: { _LIT(KName, "EMobilePhoneGetCostInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset23 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetCurrentNetworkNoLocationCancel"); name=&KName; break; }
     case EEtelMMIpcOffset24 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetMailboxNumbersCancel"); name=&KName; break; }
     case EEtelMMIpcOffset24 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneNotifyAirTimeDurationChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset24 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneNotifyCostInfoChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset25 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneNotifyCurrentNetworkNoLocationChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset26 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneNotifyMailboxNumbersChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset26 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneStoreGetInfoCancel"); name=&KName; break; }
     case EEtelMMIpcOffset26 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneStoreNotifyStoreEventCancel"); name=&KName; break; }
     case EEtelMMIpcOffset26 + EMobileCancelOffset + 3: { _LIT(KName, "EMobilePhoneStoreReadCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5: { _LIT(KName, "EMobileBroadcastMessagingSetFilterSetting"); name=&KName; break; }
     case EEtelMMIpcOffset5 + 1: { _LIT(KName, "EMobileBroadcastMessagingSetLanguageFilter"); name=&KName; break; }
     case EEtelMMIpcOffset5 + 2: { _LIT(KName, "EMobileBroadcastMessagingStoreIdList"); name=&KName; break; }
     case EEtelMMIpcOffset5 + 3: { _LIT(KName, "EMobileONStoreStoreAll"); name=&KName; break; }
     case EEtelMMIpcOffset5 + 4: { _LIT(KName, "EMobilePhoneBookStoreWrite"); name=&KName; break; }
     case EEtelMMIpcOffset5 + 5: { _LIT(KName, "EMobilePhoneStoreDelete"); name=&KName; break; }
     case EEtelMMIpcOffset5 + 6: { _LIT(KName, "EMobilePhoneStoreDeleteAll"); name=&KName; break; }
     case EEtelMMIpcOffset5 + 7: { _LIT(KName, "EMobilePhoneStoreWrite"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset: { _LIT(KName, "EMobileBroadcastMessagingSetFilterSettingCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileBroadcastMessagingSetLanguageFilterCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileBroadcastMessagingStoreIdListCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset + 3: { _LIT(KName, "EMobileONStoreStoreAllCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset + 4: { _LIT(KName, "EMobilePhoneBookStoreWriteCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset + 6: { _LIT(KName, "EMobilePhoneStoreDeleteAllCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset + 5: { _LIT(KName, "EMobilePhoneStoreDeleteCancel"); name=&KName; break; }
     case EEtelMMIpcOffset5 + EMobileCancelOffset + 7: { _LIT(KName, "EMobilePhoneStoreWriteCancel"); name=&KName; break; }
     case EEtelMMIpcOffset6: { _LIT(KName, "EMobilePhoneGetCurrentNetwork"); name=&KName; break; }
     case EEtelMMIpcOffset6 + 1: { _LIT(KName, "EMobilePhoneNotifyCurrentNetworkChange"); name=&KName; break; }
     case EEtelMMIpcOffset6 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetCurrentNetworkCancel"); name=&KName; break; }
     case EEtelMMIpcOffset6 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneNotifyCurrentNetworkChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset7: { _LIT(KName, "EMobileCallDialEmergencyCall"); name=&KName; break; }
     case EEtelMMIpcOffset7 + 1: { _LIT(KName, "EMobileCallDialNoFdnCheck"); name=&KName; break; }
     case EEtelMMIpcOffset7 + 2: { _LIT(KName, "EMobileSmsMessagingAckSmsStored"); name=&KName; break; }
     case EEtelMMIpcOffset7 + 3: { _LIT(KName, "EMobileSmsMessagingNackSmsStored"); name=&KName; break; }
     case EEtelMMIpcOffset7 + 4: { _LIT(KName, "EMobileSmsMessagingResumeSmsReception"); name=&KName; break; }
     case EEtelMMIpcOffset7 + 5: { _LIT(KName, "EMobileSmsMessagingSendMessageNoFdnCheck"); name=&KName; break; }
     case EEtelMMIpcOffset7 + 6: { _LIT(KName, "EMobileUssdMessagingSendRelease"); name=&KName; break; }
     case EEtelMMIpcOffset7 + EMobileCancelOffset: { _LIT(KName, "EMobileCallDialEmergencyCallCancel"); name=&KName; break; }
     case EEtelMMIpcOffset7 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileCallDialNoFdnCheckCancel"); name=&KName; break; }
     case EEtelMMIpcOffset7 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileSmsMessagingAckSmsStoredCancel"); name=&KName; break; }
     case EEtelMMIpcOffset7 + EMobileCancelOffset + 3: { _LIT(KName, "EMobileSmsMessagingNackSmsStoredCancel"); name=&KName; break; }
     case EEtelMMIpcOffset7 + EMobileCancelOffset + 4: { _LIT(KName, "EMobileSmsMessagingResumeSmsReceptionCancel"); name=&KName; break; }
     case EEtelMMIpcOffset7 + EMobileCancelOffset + 5: { _LIT(KName, "EMobileSmsMessagingSendMessageNoFdnCheckCancel"); name=&KName; break; }
     case EEtelMMIpcOffset7 + EMobileCancelOffset + 6: { _LIT(KName, "EMobileUssdMessagingSendReleaseCancel"); name=&KName; break; }
     case EEtelMMIpcOffset8: { _LIT(KName, "EMobilePhoneSendNetworkServiceRequestNoFdnCheck"); name=&KName; break; }
     case EEtelMMIpcOffset8 + 1: { _LIT(KName, "EMobileUssdMessagingSendMessage"); name=&KName; break; }
     case EEtelMMIpcOffset8 + 2: { _LIT(KName, "EMobileUssdMessagingSendMessageNoFdnCheck"); name=&KName; break; }
     case EEtelMMIpcOffset8 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneSendNetworkServiceRequestNoFdnCheckCancel"); name=&KName; break; }
     case EEtelMMIpcOffset8 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileUssdMessagingSendMessageCancel"); name=&KName; break; }
     case EEtelMMIpcOffset8 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileUssdMessagingSendMessageNoFdnCheckCancel"); name=&KName; break; }
     case EEtelMMIpcOffset9: { _LIT(KName, "EMobileCallGetMobileDataCallRLPRange"); name=&KName; break; }
     case EEtelMMIpcOffset9 + 1: { _LIT(KName, "EMobilePhoneGetDefaultPrivacy"); name=&KName; break; }
     case EEtelMMIpcOffset9 + 2: { _LIT(KName, "EMobilePhoneNotifyDefaultPrivacyChange"); name=&KName; break; }
     case EEtelMMIpcOffset9 + 3: { _LIT(KName, "EMobileUssdMessagingReceiveMessage"); name=&KName; break; }
     case EEtelMMIpcOffset9 + 4: { _LIT(KName, "EMobileSmartCardEapInitialiseEapMethod"); name=&KName; break; }
     case EEtelMMIpcOffset9 + EMobileCancelOffset: { _LIT(KName, "EMobileCallGetMobileDataCallRLPRangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset9 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneNotifyDefaultPrivacyChangeCancel"); name=&KName; break; }
     case EEtelMMIpcOffset9 + EMobileCancelOffset + 3: { _LIT(KName, "EMobileUssdMessagingReceiveMessageCancel"); name=&KName; break; }
     case EEtelMMIpcOffset9 + EMobileCancelOffset + 4: { _LIT(KName, "EMobileSmartCardEapInitialiseEapMethodCancel"); name=&KName; break; }
     case EEtelMMIpcOffset10: { _LIT(KName, "EMobilePhoneMMInitialise"); name=&KName; break; }
     case EEtelMMIpcOffset10 + 1: { _LIT(KName, "EMobilePhoneSetDefaultPrivacy"); name=&KName; break; }
     case EEtelMMIpcOffset10 + 2: { _LIT(KName, "EMobilePhoneSetPersonalisationStatus"); name=&KName; break; }
     case EEtelMMIpcOffset10 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneMMInitialiseCancel"); name=&KName; break; }
     case EEtelMMIpcOffset10 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneSetDefaultPrivacyCancel"); name=&KName; break; }
     case EEtelMMIpcOffset10 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneSetPersonalisationStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset11: { _LIT(KName, "EMobilePhoneGetBarringStatusPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 1: { _LIT(KName, "EMobilePhoneGetBarringStatusPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 2: { _LIT(KName, "EMobilePhoneGetCallForwardingStatusPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 3: { _LIT(KName, "EMobilePhoneGetCallForwardingStatusPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 4: { _LIT(KName, "EMobilePhoneGetCompletionRequestsPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 5: { _LIT(KName, "EMobilePhoneGetCompletionRequestsPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 6: { _LIT(KName, "EMobilePhoneGetDetectedNetworksPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 7: { _LIT(KName, "EMobilePhoneGetDetectedNetworksPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 8: { _LIT(KName, "EMobilePhoneGetDetectedNetworksV2Phase1"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 9: { _LIT(KName, "EMobilePhoneGetDetectedNetworksV2Phase2"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 10: { _LIT(KName, "EMobilePhoneGetWaitingStatusPhase1"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 11: { _LIT(KName, "EMobilePhoneGetWaitingStatusPhase2"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 12: { _LIT(KName, "EMobilePhoneGetDetectedNetworksV5Phase1"); name=&KName; break; }
     case EEtelMMIpcOffset11 + 13: { _LIT(KName, "EMobilePhoneGetDetectedNetworksV5Phase2"); name=&KName; break; }
     case EEtelMMIpcOffset11 + EMobileCancelOffset: { _LIT(KName, "EMobilePhoneGetBarringStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset11 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneGetCallForwardingStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset11 + EMobileCancelOffset + 4: { _LIT(KName, "EMobilePhoneGetCompletionRequestsCancel"); name=&KName; break; }
     case EEtelMMIpcOffset11 + EMobileCancelOffset + 6: { _LIT(KName, "EMobilePhoneGetDetectedNetworksCancel"); name=&KName; break; }
     case EEtelMMIpcOffset11 + EMobileCancelOffset + 8: { _LIT(KName, "EMobilePhoneGetDetectedNetworksV2Cancel"); name=&KName; break; }
     case EEtelMMIpcOffset11 + EMobileCancelOffset + 10: { _LIT(KName, "EMobilePhoneGetWaitingStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset11 + EMobileCancelOffset + 11: { _LIT(KName, "EMobilePhoneGetDetectedNetworksV5Cancel"); name=&KName; break; }
     case EEtelMMIpcOffset12: { _LIT(KName, "EMobileCallReceiveUUI"); name=&KName; break; }
     case EEtelMMIpcOffset12 + 1: { _LIT(KName, "EMobileSmsMessagingReceiveMessage"); name=&KName; break; }
     case EEtelMMIpcOffset12 + EMobileCancelOffset: { _LIT(KName, "EMobileCallReceiveUUICancel"); name=&KName; break; }
     case EEtelMMIpcOffset12 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileSmsMessagingReceiveMessageCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13: { _LIT(KName, "EMobileCallActivateUUS"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 1: { _LIT(KName, "EMobilePhoneDeactivateCCBS"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 2: { _LIT(KName, "EMobilePhoneSelectNetwork"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 3: { _LIT(KName, "EMobilePhoneSendNetworkServiceRequest"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 4: { _LIT(KName, "EMobilePhoneSetCallBarringPassword"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 5: { _LIT(KName, "EMobilePhoneSetCallBarringStatus"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 6: { _LIT(KName, "EMobilePhoneSetCallForwardingStatus"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 7: { _LIT(KName, "EMobilePhoneSetCallWaitingStatus"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 8: { _LIT(KName, "EMobilePhoneSetSSPassword"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 9: { _LIT(KName, "EMobilePhoneSetUUSSetting"); name=&KName; break; }
     case EEtelMMIpcOffset13 + 10: { _LIT(KName, "EMobilePhoneSetAPNControlListServiceStatus"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset: { _LIT(KName, "EMobileCallActivateUUSCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 1: { _LIT(KName, "EMobilePhoneDeactivateCCBSCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 2: { _LIT(KName, "EMobilePhoneSelectNetworkCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 3: { _LIT(KName, "EMobilePhoneSendNetworkServiceRequestCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 4: { _LIT(KName, "EMobilePhoneSetCallBarringPasswordCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 5: { _LIT(KName, "EMobilePhoneSetCallBarringStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 6: { _LIT(KName, "EMobilePhoneSetCallForwardingStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 7: { _LIT(KName, "EMobilePhoneSetCallWaitingStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 8: { _LIT(KName, "EMobilePhoneSetSSPasswordCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 9: { _LIT(KName, "EMobilePhoneSetUUSSettingCancel"); name=&KName; break; }
     case EEtelMMIpcOffset13 + EMobileCancelOffset + 10: { _LIT(KName, "EMobilePhoneSetAPNControlListServiceStatusCancel"); name=&KName; break; }
     case EEtelMMIpcOffset14: { _LIT(KName, "EMobileCallAnswerWithUUI"); name=&KName; break; }
     case EEtelMMIpcOffset14 + 1: { _LIT(KName, "EMobileCallHangupWithUUI"); name=&KName; break; }
     case EEtelMMIpcOffset14 + 2: { _LIT(KName, "EMobileCallSendUUI"); name=&KName; break; }
     case EEtelMMIpcOffset14 + 3: { _LIT(KName, "EMobileSmsMessagingSendMessage"); name=&KName; break; }
     case EEtelMMIpcOffset14 + EMobileCancelOffset: { _LIT(KName, "EMobileCallAnswerWithUUICancel"); name=&KName; break; }
     case EEtelMMIpcOffset14 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileCallHangupWithUUICancel"); name=&KName; break; }
     case EEtelMMIpcOffset14 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileCallSendUUICancel"); name=&KName; break; }
     case EEtelMMIpcOffset14 + EMobileCancelOffset + 3: { _LIT(KName, "EMobileSmsMessagingSendMessageCancel"); name=&KName; break; }
     case EEtelMMIpcOffset27: { _LIT(KName, "EMobileLocationServicesNotifyMtlr"); name=&KName; break; }
     case EEtelMMIpcOffset27 + 1: { _LIT(KName, "EMobileLocationServicesNotifyMeasurementControl"); name=&KName; break; }
     case EEtelMMIpcOffset27 + EMobileCancelOffset: { _LIT(KName, "EMobileLocationServicesNotifyMtlrCancel"); name=&KName; break; }
     case EEtelMMIpcOffset27 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileLocationServicesNotifyMeasurementControlCancel"); name=&KName; break; }
     case EEtelMMIpcOffset28: { _LIT(KName, "EMobileLocationServicesSendMolr"); name=&KName; break; }
     case EEtelMMIpcOffset28 + EMobileCancelOffset: { _LIT(KName, "EMobileLocationServicesSendMolrCancel"); name=&KName; break; }
     case EEtelMMIpcOffset29: { _LIT(KName, "EMobileLocationServicesSendMtlrResponse"); name=&KName; break; }
     case EEtelMMIpcOffset29 + 1: { _LIT(KName, "EMobileLocationServicesSendMtlrError"); name=&KName; break; }
     case EEtelMMIpcOffset29 + 2: { _LIT(KName, "EMobileLocationServicesRejectMtlr"); name=&KName; break; }
     case EEtelMMIpcOffset29 + 3: { _LIT(KName, "EMobileLocationServicesSendMeasurementReport"); name=&KName; break; }
     case EEtelMMIpcOffset29 + 4: { _LIT(KName, "EMobileLocationServicesSendMeasurementControlFailure"); name=&KName; break; }
     case EEtelMMIpcOffset29 + EMobileCancelOffset: { _LIT(KName, "EMobileLocationServicesSendMtlrResponseCancel"); name=&KName; break; }
     case EEtelMMIpcOffset29 + EMobileCancelOffset + 1: { _LIT(KName, "EMobileLocationServicesSendMtlrErrorCancel"); name=&KName; break; }
     case EEtelMMIpcOffset29 + EMobileCancelOffset + 2: { _LIT(KName, "EMobileLocationServicesRejectMtlrCancel"); name=&KName; break; }
     case EEtelMMIpcOffset29 + EMobileCancelOffset + 3: { _LIT(KName, "EMobileLocationServicesSendMeasurementReportCancel"); name=&KName; break; }
     case EEtelMMIpcOffset29 + EMobileCancelOffset + 4: { _LIT(KName, "EMobileLocationServicesSendMeasurementControlFailureCancel"); name=&KName; break; }
     case EEtelMMIpcOffset30: { _LIT(KName, "EMobileLocationServicesSetLcsDomain"); name=&KName; break; }

     // Next IPC values are defined in pcktcs.h
     case EEtelPacketIpcOffset0: { _LIT(KName, "EPacketContextGetConnectionSpeed"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 1: { _LIT(KName, "EPacketContextGetLastErrorCause"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 2: { _LIT(KName, "EPacketContextGetProfileName"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 3: { _LIT(KName, "EPacketContextGetStatus"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 4: { _LIT(KName, "EPacketContextNotifyConnectionSpeedChange"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 5: { _LIT(KName, "EPacketContextNotifyStatusChange"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 6: { _LIT(KName, "EPacketEnumerateContexts"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 7: { _LIT(KName, "EPacketEnumerateNifs"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 8: { _LIT(KName, "EPacketGetAttachMode"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 9: { _LIT(KName, "EPacketGetContextInfo"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 10: { _LIT(KName, "EPacketGetCurrentReleaseMode"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 11: { _LIT(KName, "EPacketGetDynamicCaps"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 12: { _LIT(KName, "EPacketGetMSClass"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 13: { _LIT(KName, "EPacketGetNtwkRegStatus"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 14: { _LIT(KName, "EPacketGetPrefBearer"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 15: { _LIT(KName, "EPacketGetStaticCaps"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 16: { _LIT(KName, "EPacketGetStatus"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 17: { _LIT(KName, "EPacketNotifyAttachModeChange"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 18: { _LIT(KName, "EPacketNotifyChangeOfNtwkRegStatus"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 19: { _LIT(KName, "EPacketNotifyContextActivationRequested"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 20: { _LIT(KName, "EPacketNotifyContextAdded"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 21: { _LIT(KName, "EPacketNotifyDynamicCapsChange"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 22: { _LIT(KName, "EPacketNotifyMSClassChange"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 23: { _LIT(KName, "EPacketNotifyReleaseModeChange"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 24: { _LIT(KName, "EPacketNotifyStatusChange"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 25: { _LIT(KName, "EPacketPrepareOpenSecondary"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + 26: { _LIT(KName, "EPacketQoSGetProfileCaps"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset: { _LIT(KName, "EPacketContextGetConnectionSpeedCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 4: { _LIT(KName, "EPacketContextNotifyConnectionSpeedChangeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 5: { _LIT(KName, "EPacketContextNotifyStatusChangeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 6: { _LIT(KName, "EPacketEnumerateContextsCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 7: { _LIT(KName, "EPacketEnumerateNifsCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 8: { _LIT(KName, "EPacketGetAttachModeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 9: { _LIT(KName, "EPacketGetContextInfoCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 10: { _LIT(KName, "EPacketGetCurrentReleaseModeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 12: { _LIT(KName, "EPacketGetMSClassCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 13: { _LIT(KName, "EPacketGetNtwkRegStatusCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 14: { _LIT(KName, "EPacketGetPrefBearerCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 17: { _LIT(KName, "EPacketNotifyAttachModeChangeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 18: { _LIT(KName, "EPacketNotifyChangeOfNtwkRegStatusCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 19: { _LIT(KName, "EPacketNotifyContextActivationRequestedCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 20: { _LIT(KName, "EPacketNotifyContextAddedCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 21: { _LIT(KName, "EPacketNotifyDynamicCapsChangeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 22: { _LIT(KName, "EPacketNotifyMSClassChangeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 23: { _LIT(KName, "EPacketNotifyReleaseModeChangeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 24: { _LIT(KName, "EPacketNotifyStatusChangeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset0 + EMobileCancelOffset + 26: { _LIT(KName, "EPacketQoSGetProfileCapsCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1: { _LIT(KName, "EPacketAttach"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 1: { _LIT(KName, "EPacketContextActivate"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 2: { _LIT(KName, "EPacketContextCreateNewTFT"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 3: { _LIT(KName, "EPacketContextDeactivate"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 4: { _LIT(KName, "EPacketContextDeleteTFT"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 5: { _LIT(KName, "EPacketContextInitialiseContext"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 6: { _LIT(KName, "EPacketContextLoanCommPort"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 7: { _LIT(KName, "EPacketContextModifyActiveContext"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 8: { _LIT(KName, "EPacketContextRecoverCommPort"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 9: { _LIT(KName, "EPacketDeactivateNIF"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 10: { _LIT(KName, "EPacketDetach"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + 11: { _LIT(KName, "EPacketRejectActivationRequest"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset: { _LIT(KName, "EPacketAttachCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 1: { _LIT(KName, "EPacketContextActivateCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 2: { _LIT(KName, "EPacketContextCreateNewTFTCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 3: { _LIT(KName, "EPacketContextDeactivateCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 4: { _LIT(KName, "EPacketContextDeleteTFTCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 5: { _LIT(KName, "EPacketContextInitialiseContextCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 6: { _LIT(KName, "EPacketContextLoanCommPortCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 7: { _LIT(KName, "EPacketContextModifyActiveContextCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 8: { _LIT(KName, "EPacketContextRecoverCommPortCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 9: { _LIT(KName, "EPacketDeactivateNIFCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 10: { _LIT(KName, "EPacketDetachCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset1 + EMobileCancelOffset + 11: { _LIT(KName, "EPacketRejectActivationRequestCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2: { _LIT(KName, "EPacketContextEnumeratePacketFilters"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 1: { _LIT(KName, "EPacketContextGetConfig"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 2: { _LIT(KName, "EPacketContextGetDNSInfo"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 3: { _LIT(KName, "EPacketContextNotifyConfigChanged"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 4: { _LIT(KName, "EPacketEnumerateContextsInNif"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 5: { _LIT(KName, "EPacketGetContextNameInNif"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 6: { _LIT(KName, "EPacketGetDefaultContextParams"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 7: { _LIT(KName, "EPacketGetNifInfo"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 8: { _LIT(KName, "EPacketQoSGetProfileParams"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + 9: { _LIT(KName, "EPacketQoSNotifyProfileChanged"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset: { _LIT(KName, "EPacketContextEnumeratePacketFiltersCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 1: { _LIT(KName, "EPacketContextGetConfigCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 2: { _LIT(KName, "EPacketContextGetDNSInfoCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 3: { _LIT(KName, "EPacketContextNotifyConfigChangedCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 4: { _LIT(KName, "EPacketEnumerateContextsInNifCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 5: { _LIT(KName, "EPacketGetContextNameInNifCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 6: { _LIT(KName, "EPacketGetDefaultContextParamsCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 7: { _LIT(KName, "EPacketGetNifInfoCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 8: { _LIT(KName, "EPacketQoSGetProfileParamsCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset2 + EMobileCancelOffset + 9: { _LIT(KName, "EPacketQoSNotifyProfileChangedCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset3: { _LIT(KName, "EPacketContextDelete"); name=&KName; break; }
     case EEtelPacketIpcOffset3 + 1: { _LIT(KName, "EPacketContextSetConfig"); name=&KName; break; }
     case EEtelPacketIpcOffset3 + 2: { _LIT(KName, "EPacketSetAttachMode"); name=&KName; break; }
     case EEtelPacketIpcOffset3 + 3: { _LIT(KName, "EPacketSetDefaultContextParams"); name=&KName; break; }
     case EEtelPacketIpcOffset3 + EMobileCancelOffset: { _LIT(KName, "EPacketContextDeleteCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset3 + EMobileCancelOffset + 1: { _LIT(KName, "EPacketContextSetConfigCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset3 + EMobileCancelOffset + 2: { _LIT(KName, "EPacketSetAttachModeCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset3 + EMobileCancelOffset + 3: { _LIT(KName, "EPacketSetDefaultContextParamsCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset4: { _LIT(KName, "EPacketContextGetDataVolumeTransferred"); name=&KName; break; }
     case EEtelPacketIpcOffset4 + 1: { _LIT(KName, "EPacketContextNotifyDataTransferred"); name=&KName; break; }
     case EEtelPacketIpcOffset4 + EMobileCancelOffset: { _LIT(KName, "EPacketContextGetDataVolumeTransferredCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset4 + EMobileCancelOffset + 1: { _LIT(KName, "EPacketContextNotifyDataTransferredCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset5: { _LIT(KName, "EPacketContextGetPacketFilterInfo"); name=&KName; break; }
     case EEtelPacketIpcOffset5 + EMobileCancelOffset: { _LIT(KName, "EPacketContextGetPacketFilterInfoCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset6: { _LIT(KName, "EPacketContextAddPacketFilter"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + 1: { _LIT(KName, "EPacketContextRemovePacketFilter"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + 2: { _LIT(KName, "EPacketQoSSetProfileParams"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + 3: { _LIT(KName, "EPacketSetMSClass"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + 4: { _LIT(KName, "EPacketSetPrefBearer"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + 5: { _LIT(KName, "EPacketAddMediaAuthorization"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + 6: { _LIT(KName, "EPacketRemoveMediaAuthorization"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + EMobileCancelOffset: { _LIT(KName, "EPacketContextAddPacketFilterCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + EMobileCancelOffset + 1: { _LIT(KName, "EPacketContextRemovePacketFilterCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + EMobileCancelOffset + 2: { _LIT(KName, "EPacketQoSSetProfileParamsCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + EMobileCancelOffset + 3: { _LIT(KName, "EPacketSetMSClassCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + EMobileCancelOffset + 4: { _LIT(KName, "EPacketSetPrefBearerCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + EMobileCancelOffset + 5: { _LIT(KName, "EPacketAddMediaAuthorizationCancel"); name=&KName; break; }
     case EEtelPacketIpcOffset6 + EMobileCancelOffset + 6: { _LIT(KName, "EPacketRemoveMediaAuthorizationCancel"); name=&KName; break; }

     // Next IPC values are defined in satcs.h
     case EEtelSatIpcOffset0: { _LIT(KName, "ESatGetClut"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 1: { _LIT(KName, "ESatGetIcon"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 2: { _LIT(KName, "ESatGetImageInstance"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 3: { _LIT(KName, "ESatGetMeSideSatProfile"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 4: { _LIT(KName, "ESatNotifyCloseChannelPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 5: { _LIT(KName, "ESatNotifyGetChannelStatusPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 6: { _LIT(KName, "ESatNotifyGetInkeyPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 7: { _LIT(KName, "ESatNotifyGetInputPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 8: { _LIT(KName, "ESatNotifyGetReaderStatusPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 9: { _LIT(KName, "ESatNotifyLanguageNotificationPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 10: { _LIT(KName, "ESatNotifyPlayTonePCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 11: { _LIT(KName, "ESatNotifyPollingIntervalPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 12: { _LIT(KName, "ESatNotifyPollingOffPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 13: { _LIT(KName, "ESatNotifyPowerOffCardPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 14: { _LIT(KName, "ESatNotifyPowerOnCardPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 15: { _LIT(KName, "ESatNotifyProactiveSimSessionEnd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 16: { _LIT(KName, "ESatNotifyProactiveSimSessionStart"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 17: { _LIT(KName, "ESatNotifyReceiveDataPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 18: { _LIT(KName, "ESatNotifyRefreshPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 19: { _LIT(KName, "ESatNotifyRefreshRequired"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 20: { _LIT(KName, "ESatNotifySelectItemPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 21: { _LIT(KName, "ESatNotifySetUpEventListPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 22: { _LIT(KName, "ESatNotifySetUpMenuPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 23: { _LIT(KName, "ESatNotifyTimerExpiration"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 24: { _LIT(KName, "ESatNotifyTimerMgmtPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 25: { _LIT(KName, "ESatNotifyTsyStateUpdated"); name=&KName; break; }
     case EEtelSatIpcOffset0 + 26: { _LIT(KName, "ESatNotifyRefreshRequiredParam"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset: { _LIT(KName, "ESatGetClutCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 1: { _LIT(KName, "ESatGetIconCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 2: { _LIT(KName, "ESatGetImageInstanceCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 3: { _LIT(KName, "ESatGetMeSideSatProfileCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 4: { _LIT(KName, "ESatNotifyCloseChannelPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 5: { _LIT(KName, "ESatNotifyGetChannelStatusPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 6: { _LIT(KName, "ESatNotifyGetInkeyPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 7: { _LIT(KName, "ESatNotifyGetInputPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 8: { _LIT(KName, "ESatNotifyGetReaderStatusPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 9: { _LIT(KName, "ESatNotifyLanguageNotificationPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 10: { _LIT(KName, "ESatNotifyPlayTonePCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 11: { _LIT(KName, "ESatNotifyPollingIntervalPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 12: { _LIT(KName, "ESatNotifyPollingOffPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 13: { _LIT(KName, "ESatNotifyPowerOffCardPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 14: { _LIT(KName, "ESatNotifyPowerOnCardPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 15: { _LIT(KName, "ESatNotifyProactiveSimSessionEndCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 16: { _LIT(KName, "ESatNotifyProactiveSimSessionStartCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 17: { _LIT(KName, "ESatNotifyReceiveDataPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 18: { _LIT(KName, "ESatNotifyRefreshPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 19: { _LIT(KName, "ESatNotifyRefreshRequiredCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 20: { _LIT(KName, "ESatNotifySelectItemPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 21: { _LIT(KName, "ESatNotifySetUpEventListPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 22: { _LIT(KName, "ESatNotifySetUpMenuPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 23: { _LIT(KName, "ESatNotifyTimerExpirationCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 24: { _LIT(KName, "ESatNotifyTimerMgmtPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 25: { _LIT(KName, "ESatNotifyTsyStateUpdatedCancel"); name=&KName; break; }
     case EEtelSatIpcOffset0 + EMobileCancelOffset + 26: { _LIT(KName, "ESatNotifyRefreshRequiredParamCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1: { _LIT(KName, "ESatGetProvisioningRefFile"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 1: { _LIT(KName, "ESatNotifyDeclareServicePCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 2: { _LIT(KName, "ESatNotifyDisplayTextPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 3: { _LIT(KName, "ESatNotifyGetServiceInfoPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 4: { _LIT(KName, "ESatNotifyLaunchBrowserPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 5: { _LIT(KName, "ESatNotifyLocalInfoPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 6: { _LIT(KName, "ESatNotifyOpenChannelPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 7: { _LIT(KName, "ESatNotifyPerformCardApduPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 8: { _LIT(KName, "ESatNotifyRunAtCommandPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 9: { _LIT(KName, "ESatNotifySendDataPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 10: { _LIT(KName, "ESatNotifySendDtmfPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 11: { _LIT(KName, "ESatNotifySendSmPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 12: { _LIT(KName, "ESatNotifySendSsPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 13: { _LIT(KName, "ESatNotifySendUssdPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 14: { _LIT(KName, "ESatNotifyServiceSearchPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 15: { _LIT(KName, "ESatNotifySetUpCallPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 16: { _LIT(KName, "ESatNotifySetUpIdleModeTextPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + 17: { _LIT(KName, "ESatGetOpenChannelPCmd"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset: { _LIT(KName, "ESatGetProvisioningRefFileCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 1: { _LIT(KName, "ESatNotifyDeclareServicePCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 2: { _LIT(KName, "ESatNotifyDisplayTextPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 3: { _LIT(KName, "ESatNotifyGetServiceInfoPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 4: { _LIT(KName, "ESatNotifyLaunchBrowserPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 5: { _LIT(KName, "ESatNotifyLocalInfoPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 6: { _LIT(KName, "ESatNotifyOpenChannelPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 7: { _LIT(KName, "ESatNotifyPerformCardApduPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 8: { _LIT(KName, "ESatNotifyRunAtCommandPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 9: { _LIT(KName, "ESatNotifySendDataPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 10: { _LIT(KName, "ESatNotifySendDtmfPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 11: { _LIT(KName, "ESatNotifySendSmPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 12: { _LIT(KName, "ESatNotifySendSsPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 13: { _LIT(KName, "ESatNotifySendUssdPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 14: { _LIT(KName, "ESatNotifyServiceSearchPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 15: { _LIT(KName, "ESatNotifySetUpCallPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 16: { _LIT(KName, "ESatNotifySetUpIdleModeTextPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset1 + EMobileCancelOffset + 17: { _LIT(KName, "ESatGetOpenChannelPCmdCancel"); name=&KName; break; }
     case EEtelSatIpcOffset2: { _LIT(KName, "ESatRefreshAllowed"); name=&KName; break; }
     case EEtelSatIpcOffset2 + EMobileCancelOffset: { _LIT(KName, "ESatRefreshAllowedCancel"); name=&KName; break; }
     case EEtelSatIpcOffset3: { _LIT(KName, "ESatTerminalRsp"); name=&KName; break; }
     case EEtelSatIpcOffset3 + 1: { _LIT(KName, "ESatUsatClientReadyIndication"); name=&KName; break; }
     case EEtelSatIpcOffset3 + EMobileCancelOffset: { _LIT(KName, "ESatTerminalRspCancel"); name=&KName; break; }
     case EEtelSatIpcOffset4: { _LIT(KName, "ESatMenuSelection"); name=&KName; break; }
     case EEtelSatIpcOffset4 + 1: { _LIT(KName, "ESatNotifyCallControlRequest"); name=&KName; break; }
     case EEtelSatIpcOffset4 + 2: { _LIT(KName, "ESatNotifyCbDownload"); name=&KName; break; }
     case EEtelSatIpcOffset4 + 3: { _LIT(KName, "ESatNotifyMoSmControlRequest"); name=&KName; break; }
     case EEtelSatIpcOffset4 + 4: { _LIT(KName, "ESatNotifySmsPpDownload"); name=&KName; break; }
     case EEtelSatIpcOffset4 + EMobileCancelOffset: { _LIT(KName, "ESatMenuSelectionCancel"); name=&KName; break; }
     case EEtelSatIpcOffset4 + EMobileCancelOffset + 1: { _LIT(KName, "ESatNotifyCallControlRequestCancel"); name=&KName; break; }
     case EEtelSatIpcOffset4 + EMobileCancelOffset + 2: { _LIT(KName, "ESatNotifyCbDownloadCancel"); name=&KName; break; }
     case EEtelSatIpcOffset4 + EMobileCancelOffset + 3: { _LIT(KName, "ESatNotifyMoSmControlRequestCancel"); name=&KName; break; }
     case EEtelSatIpcOffset4 + EMobileCancelOffset + 4: { _LIT(KName, "ESatNotifySmsPpDownloadCancel"); name=&KName; break; }
     case EEtelSatIpcOffset5: { _LIT(KName, "ESatClientSatProfileIndication"); name=&KName; break; }
     case EEtelSatIpcOffset5 + 1: { _LIT(KName, "ESatEventDownload"); name=&KName; break; }
     case EEtelSatIpcOffset5 + EMobileCancelOffset + 1: { _LIT(KName, "ESatEventDownloadCancel"); name=&KName; break; }
     case EEtelSatIpcOffset6: { _LIT(KName, "ESatSendMessageNoLogging"); name=&KName; break; }
     case EEtelSatIpcOffset6 + EMobileCancelOffset: { _LIT(KName, "ESatSendMessageNoLoggingCancel"); name=&KName; break; }


     // Next IPC values are defined in \commontsy\CustomAPI\inc\RMmCustomAPI.h
     case KIpcCustomExt: { _LIT(KName, "ECustomSsAdditionalInfoNotificationIPC"); name=&KName; break; }
     case KIpcCustomExt + 1: { _LIT(KName, "ECustomNotifySsRequestCompleteIPC"); name=&KName; break; }
     case KIpcCustomExt + 2: { _LIT(KName, "ECustomCancelUssdSessionIPC"); name=&KName; break; }
     case KIpcCustomExt + 3: { _LIT(KName, "ECustomStartSimCbTopicBrowsingIPC"); name=&KName; break; }
     case KIpcCustomExt + 4: { _LIT(KName, "ECustomGetNextSimCbTopicIPC"); name=&KName; break; }
     case KIpcCustomExt + 5: { _LIT(KName, "ECustomDeleteSimCbTopicIPC"); name=&KName; break; }
     case KIpcCustomExt + 6: { _LIT(KName, "ECustomReadSimFileIPC"); name=&KName; break; }
     case KIpcCustomExt + 7: { _LIT(KName, "ECustomGetSimAuthenticationDataIPC"); name=&KName; break; }
     case KIpcCustomExt + 8: { _LIT(KName, "ECustomSetSimMessageStatusReadIPC"); name=&KName; break; }
     case KIpcCustomExt + 9: { _LIT(KName, "ECustomNotifyDtmfEventIPC"); name=&KName; break; }
     case KIpcCustomExt + 10: { _LIT(KName, "ECustomGetDiagnosticOctetsIPC"); name=&KName; break; }
     case KIpcCustomExt + 11: { _LIT(KName, "ECustomSetAlsBlockedIPC"); name=&KName; break; }
     case KIpcCustomExt + 12: { _LIT(KName, "ECustomGetAlsBlockedIPC"); name=&KName; break; }
     case KIpcCustomExt + 13: { _LIT(KName, "ECustomCheckAlsPpSupportIPC"); name=&KName; break; }
     case KIpcCustomExt + 14: { _LIT(KName, "ECustomGetRemoteAlertingToneStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 15: { _LIT(KName, "ECustomCallOriginIPC"); name=&KName; break; }
     case KIpcCustomExt + 16: { _LIT(KName, "ECustomNotifyAlsBlockedChangedIPC"); name=&KName; break; }
     case KIpcCustomExt + 17: { _LIT(KName, "ECustomTerminateCallIPC"); name=&KName; break; }
     case KIpcCustomExt + 18: { _LIT(KName, "ECustomClearCallBlackListIPC"); name=&KName; break; }
     case KIpcCustomExt + 19: { _LIT(KName, "ECustomReadHSxPAStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 20: { _LIT(KName, "ECustomWriteHSxPAStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 21: { _LIT(KName, "ECustomNotifyHSxPAStatusIPC,"); name=&KName; break; }
     case KIpcCustomExt + 22: { _LIT(KName, "ECustomGetCellInfoIPC"); name=&KName; break; }
     case KIpcCustomExt + 23: { _LIT(KName, "ECustomNotifyCellInfoChangeIPC"); name=&KName; break; }
     case KIpcCustomExt + 24: { _LIT(KName, "ECustomSimLockActivateIPC"); name=&KName; break; }
     case KIpcCustomExt + 25: { _LIT(KName, "ECustomSimLockDeActivateIPC"); name=&KName; break; }
     case KIpcCustomExt + 26: { _LIT(KName, "EReadViagHomeZoneParamsIPC"); name=&KName; break; }
     case KIpcCustomExt + 27: { _LIT(KName, "EReadViagHomeZoneCacheIPC"); name=&KName; break; }
     case KIpcCustomExt + 28: { _LIT(KName, "EWriteViagHomeZoneCacheIPC"); name=&KName; break; }
     case KIpcCustomExt + 29: { _LIT(KName, "ECustomGetCipheringInfoIPC"); name=&KName; break; }
     case KIpcCustomExt + 30: { _LIT(KName, "ECustomNotifyCipheringInfoChangeIPC"); name=&KName; break; }
     case KIpcCustomExt + 31: { _LIT(KName, "ECustomNotifyNSPSStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 32: { _LIT(KName, "ECustomNetWakeupIPC"); name=&KName; break; }
     case KIpcCustomExt + 33: { _LIT(KName, "ECustomGetNetworkProviderNameIPC"); name=&KName; break; }
     case KIpcCustomExt + 34: { _LIT(KName, "ECustomGetOperatorNameIPC"); name=&KName; break; }
     case KIpcCustomExt + 35: { _LIT(KName, "ECustomGetProgrammableOperatorLogoIPC"); name=&KName; break; }
     case KIpcCustomExt + 36: { _LIT(KName, "ECustomResetNetServerIPC"); name=&KName; break; }
     case KIpcCustomExt + 37: { _LIT(KName, "ECustomNotifyProgrammableOperatorLogoChangeIPC"); name=&KName; break; }
     case KIpcCustomExt + 38: { _LIT(KName, "ECustomNotifyNetworkConnectionFailureIPC"); name=&KName; break; }
     case KIpcCustomExt + 39: { _LIT(KName, "ECustomGetAirTimeDurationIPC"); name=&KName; break; }
     case KIpcCustomExt + 40: { _LIT(KName, "ECustomNotifyPndCacheReadyIPC"); name=&KName; break; }
     case KIpcCustomExt + 41: { _LIT(KName, "ECustomGetPndCacheStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 42: { _LIT(KName, "ECustomNotifySsNetworkEventIPC"); name=&KName; break; }
     case KIpcCustomExt + 43: { _LIT(KName, "ECustomCheckTwoDigitDialSupportIPC"); name=&KName; break; }
     case KIpcCustomExt + 44: { _LIT(KName, "ECustomSendAPDUReqIPC"); name=&KName; break; }
     case KIpcCustomExt + 45: { _LIT(KName, "ECustomGetLifeTimeIPC"); name=&KName; break; }
     case KIpcCustomExt + 46: { _LIT(KName, "ECustomPowerSimOnIPC"); name=&KName; break; }
     case KIpcCustomExt + 47: { _LIT(KName, "ECustomPowerSimOffIPC"); name=&KName; break; }
     case KIpcCustomExt + 48: { _LIT(KName, "ECustomSimWarmResetIPC"); name=&KName; break; }
     case KIpcCustomExt + 49: { _LIT(KName, "ECustomGetATRIPC"); name=&KName; break; }
     case KIpcCustomExt + 50: { _LIT(KName, "ECustomSendAPDUReqV2IPC"); name=&KName; break; }
     case KIpcCustomExt + 51: { _LIT(KName, "ECustomGetSimCardReaderStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 52: { _LIT(KName, "ECustomNotifySimCardStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 53: { _LIT(KName, "ECustomSetDriveModeIPC"); name=&KName; break; }
     case KIpcCustomExt + 54: { _LIT(KName, "ECustomIsBlockedIPC"); name=&KName; break; }
     case KIpcCustomExt + 55: { _LIT(KName, "ECustomCheckSecurityCodeIPC"); name=&KName; break; }
     case KIpcCustomExt + 56: { _LIT(KName, "ECustomCheckEmergencyNumberIPC"); name=&KName; break; }
     case KIpcCustomExt + 57: { _LIT(KName, "ECustomSatRefreshCompleteNotificationIPC"); name=&KName; break; }
     case KIpcCustomExt + 58: { _LIT(KName, "ECustomDisablePhoneLockIPC"); name=&KName; break; }
     case KIpcCustomExt + 59: { _LIT(KName, "ECustomGetActivePinIPC"); name=&KName; break; }
     case KIpcCustomExt + 60: { _LIT(KName, "ECustomReleaseFileIPC"); name=&KName; break; }
     case KIpcCustomExt + 61: { _LIT(KName, "ECustomRestartFileIPC"); name=&KName; break; }
     case KIpcCustomExt + 62: { _LIT(KName, "ECustomNotifyEGprsInfoChange"); name=&KName; break; }
     case KIpcCustomExt + 63: { _LIT(KName, "ECustomGetEGprsInfo"); name=&KName; break; }
     case KIpcCustomExt + 64: { _LIT(KName, "EGet3GPBInfoIPC"); name=&KName; break; }
     case KIpcCustomExt + 65: { _LIT(KName, "ECustomGetSystemNetworkModesIPC"); name=&KName; break; }
     case KIpcCustomExt + 66: { _LIT(KName, "ECustomSetSystemNetworkModeIPC"); name=&KName; break; }
     case KIpcCustomExt + 67: { _LIT(KName, "ECustomGetCurrentSystemNetworkModesIPC"); name=&KName; break; }
     case KIpcCustomExt + 68: { _LIT(KName, "EWriteViagHomeZoneUHZIUESettingsIPC"); name=&KName; break; }
     case KIpcCustomExt + 69: { _LIT(KName, "ECustomSetAlwaysOnMode"); name=&KName; break; }
     case KIpcCustomExt + 70: { _LIT(KName, "ECustomNotifyRauEventIPC"); name=&KName; break; }
     case KIpcCustomExt + 71: { _LIT(KName, "ECustomGetIccCallForwardingStatusIPC"); name=&KName; break; }
     case KIpcCustomExt + 72: { _LIT(KName, "ECustomNotifyIccCallForwardingStatusChangeIPC"); name=&KName; break; }
     case KIpcCustomExt + 73: { _LIT(KName, "ECustomGetBandSelectionIPC"); name=&KName; break; }
     case KIpcCustomExt + 74: { _LIT(KName, "ECustomSetBandSelectionIPC"); name=&KName; break; }
     case KIpcCustomExt + 75: { _LIT(KName, "ECustomGetServiceTableSupportbyApplicationIPC"); name=&KName; break; }

     // Next IPC values are defined in \commontsy\ctsy\inc\mmtsy\MmTsy_IPCdefs.h
     case IPC_TSY_PRIVATE_EXT: { _LIT(KName, "EMmTsyPhoneBookStoreReadIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 1: { _LIT(KName, "EMmTsyPhoneBookStoreReadCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 2: { _LIT(KName, "EMmTsyPhoneBookStoreWriteIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 3: { _LIT(KName, "EMmTsyPhoneBookStoreWriteCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 4: { _LIT(KName, "EMmTsyPhoneBookStoreGetInfoIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 5: { _LIT(KName, "EMmTsyPhoneBookStoreGetInfoCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 6: { _LIT(KName, "EMmTsyPhoneBookStoreDeleteIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 7: { _LIT(KName, "EMmTsyPhoneBookStoreDeleteCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 8: { _LIT(KName, "EMmTsyPhoneBookStoreDeleteAllIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 9: { _LIT(KName, "EMmTsyPhoneBookStoreDeleteAllCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 10: { _LIT(KName, "EMmTsyPhoneBookStoreNotifyStoreEventIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 11: { _LIT(KName, "EMmTsyPhoneBookStoreNotifyStoreEventCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 12: { _LIT(KName, "EMmTsyPhoneBookStoreReadAllPhase1IPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 13: { _LIT(KName, "EMmTsyPhoneBookStoreReadAllPhase2IPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 14: { _LIT(KName, "EMmTsyPhoneBookStoreReadAllCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 15: { _LIT(KName, "EMmTsyPhoneBookStoreCacheIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 16: { _LIT(KName, "EMmTsyPhoneBookStoreInitIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 17: { _LIT(KName, "EMmTsyPhoneBookStoreResetCacheIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 18: { _LIT(KName, "EMmTsyPhoneBookStoreFdnInfoIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 19: { _LIT(KName, "EMmTsyPhoneBookStoreMaxIpcLimit"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 20: { _LIT(KName, "EMmTsyENStoreReadIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 21: { _LIT(KName, "EMmTsyENStoreReadCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 22: { _LIT(KName, "EMmTsyENStoreWriteIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 23: { _LIT(KName, "EMmTsyENStoreWriteCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 24: { _LIT(KName, "EMmTsyENStoreGetInfoIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 25: { _LIT(KName, "EMmTsyENStoreGetInfoCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 26: { _LIT(KName, "EMmTsyENStoreDeleteIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 27: { _LIT(KName, "EMmTsyENStoreDeleteCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 28: { _LIT(KName, "EMmTsyENStoreDeleteAllIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 29: { _LIT(KName, "EMmTsyENStoreDeleteAllCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 30: { _LIT(KName, "EMmTsyENStoreNotifyStoreEventIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 31: { _LIT(KName, "EMmTsyENStoreNotifyStoreEventCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 32: { _LIT(KName, "EMmTsyENStoreReadAllPhase1IPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 33: { _LIT(KName, "EMmTsyENStoreReadAllPhase2IPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 34: { _LIT(KName, "EMmTsyENStoreReadAllCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 35: { _LIT(KName, "EMmTsyENStoreMaxIpcLimit"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 36: { _LIT(KName, "EMmTsyONStoreReadIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 37: { _LIT(KName, "EMmTsyONStoreReadCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 38: { _LIT(KName, "EMmTsyONStoreWriteIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 39: { _LIT(KName, "EMmTsyONStoreWriteCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 40: { _LIT(KName, "EMmTsyONStoreGetInfoIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 41: { _LIT(KName, "EMmTsyONStoreGetInfoCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 42: { _LIT(KName, "EMmTsyONStoreDeleteIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 43: { _LIT(KName, "EMmTsyONStoreDeleteCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 44: { _LIT(KName, "EMmTsyONStoreDeleteAllIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 45: { _LIT(KName, "EMmTsyONStoreDeleteAllCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 46: { _LIT(KName, "EMmTsyONStoreNotifyStoreEventIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 47: { _LIT(KName, "EMmTsyONStoreNotifyStoreEventCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 48: { _LIT(KName, "EMmTsyONStoreReadAllPhase1IPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 49: { _LIT(KName, "EMmTsyONStoreReadAllPhase2IPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 50: { _LIT(KName, "EMmTsyONStoreReadAllCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 51: { _LIT(KName, "EMmTsyONStoreStoreAllIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 52: { _LIT(KName, "EMmTsyONStoreStoreAllCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 53: { _LIT(KName, "EMmTsyONStoreCountEntriesIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 54: { _LIT(KName, "EMmTsyONStoreMaxIpcLimit"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 55: { _LIT(KName, "EPacketInitProxiesIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 56: { _LIT(KName, "EMmTsyGsmBroadcastNotifyMessageReceived"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 57: { _LIT(KName, "EMmTsyWcdmaBroadcastNotifyMessageReceived"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 58: { _LIT(KName, "EMmTsyBootNotifyModemStatusReadyIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 59: { _LIT(KName, "EMmTsyBootNotifySimStatusReadyIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 60: { _LIT(KName, "EMmTsyBootNotifySecurityReadyIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 61: { _LIT(KName, "EMmTsyBootGetRFStatusIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 62: { _LIT(KName, "EMmTsySmsSendSatMessage"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 63: { _LIT(KName, "EMmTsyActivateSmsRouting"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 64: { _LIT(KName, "EMmTsyDeactivateSmsRouting"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 65: { _LIT(KName, "EMmTsySimGetICCType"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 66: { _LIT(KName, "EMmTsySimRefreshRegisterIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 67: { _LIT(KName, "EMmTsySimRefreshNowIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 68: { _LIT(KName, "EMmTsySimRefreshDoneIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 69: { _LIT(KName, "ECustomCheckSecurityCodeCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 70: { _LIT(KName, "EMmTsySecurityGetSimActivePinStateIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 71: { _LIT(KName, "ECustomSecurityDeliverCodeIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 72: { _LIT(KName, "EMmTsyPhoneGetPin1DisableSupportedIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 73: { _LIT(KName, "EMmTsyNotifyEGprsInfoChangeIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 74: { _LIT(KName, "EMmTsyGetCustomVendorExtPtrIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 75: { _LIT(KName, "EMmTsyONStoreReadEntryIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 76: { _LIT(KName, "EMmTsyONStoreReadSizeIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 77: { _LIT(KName, "EMmTsyONStoreWriteEntryIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 78: { _LIT(KName, "EMmTsyONStoreWriteSizeIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 79: { _LIT(KName, "EMmTsyGetCallForwardingNumberIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 80: { _LIT(KName, "EPacketNotifyTransferCapsIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 81: { _LIT(KName, "EMmTsyPhoneBookStoreCacheCancelIPC"); name=&KName; break; }
     case IPC_TSY_PRIVATE_EXT + 82: { _LIT(KName, "EMmTsyUpdateLifeTimeIPC"); name=&KName; break; }
     default: { _LIT(KName, "UNKNOWN"); name=&KName; break; }
     }

    return *name;
}
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCore::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CCore::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CCore::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCore::RunMethodL(
    CStifItemParser& /*aItem*/ )
    {

    return( KErrNone );

    }


// -----------------------------------------------------------------------------
// CCore::Notify
// Calls correct Notify
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::Notify( TNotifyList& aNotifyList )
    {
    _LIT8(KCoreNotify, "CORE: Notify");
    iCoreLog->Log((TDesC8)KCoreNotify);

    switch( ObjectType() )
        {
        case CCore::ECore:
            {
            break;
            }
        case CCore::ENWE:
            {
            iNWE->NotifyNWE( aNotifyList );
            break;
            }
        default:
            {
            // Should never enter this block
            _LIT8(KDefault, "Default block in CCore::Notify");
            iCoreLog->Log((TDesC8)KDefault);
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CCore::HandleRequestL
// Forwards a request to DOS layer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::HandleRequestL
        (
        const TInt aIpc,        //IPC number
        const CMmDataPackage* aDataPackage
        )
    {

    TFLOGSTRING3("TSY: CCore::HandleRequestL. IPC = %d (%S)", aIpc, &MapIpcToName(aIpc));

    return iMessageRouter->ExtFuncL( aIpc, aDataPackage );
    }


// -----------------------------------------------------------------------------
// CCore::HandleRequestL
// Forwards a request to DOS layer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::HandleRequestL
        (
        const TInt aIpc        //IPC number
        )
    {
    TFLOGSTRING("TSY: CCore::HandleRequestL");
    return HandleRequestL( aIpc, reinterpret_cast<const CMmDataPackage*>(NULL) );
    }


// -----------------------------------------------------------------------------
// CCore::HandleRequestL
// Forwards a request to DOS layer
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::HandleRequestL
        (
        const TInt aIpc,        //IPC number
        const TDataPackage* aDataPackage //Data package,
        )
    {
    TFLOGSTRING("TSY: CCore::HandleRequestL");

    // map the TDataPackage into a CMmDataPackage
    // Non-core Methods from the SOS layer using the DOS layer will perforn
    // always this mapping.
    // TSY core methods will call directly the overloaded HandleRequest method which
    // uses a CMmDataPackage in the method's signature.
    iDataPackage.SetPacketData ( aDataPackage );

    //call the overloaded HandleRequest of CMmMessageManager
    return HandleRequestL( aIpc, &iDataPackage);

    }


// -----------------------------------------------------------------------------
// CCore::SetObjectType
// SetObjectType method function.
// -----------------------------------------------------------------------------
//
EXPORT_C void CCore::SetObjectType ( TObjectType aType, TAny* aModule )
    {

    _LIT8(KSetObjectType, "CCore::SetObjectType");
    iCoreLog->Log((TDesC8)KSetObjectType);
    // Stores object type
    iObjectType = aType;

    // Stores pointer to object ( this must cast to right type )
    iObject = aModule;

    switch( iObjectType )
        {
        case EVoice:
            {
            iVoice = reinterpret_cast <CVoice*>( aModule );
            break;
            }
        case ENWE:
            {
            iNWE = reinterpret_cast <CNWE*>( aModule );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCore::ObjectType
// ObjectType method function.
// returns derived object type
// -----------------------------------------------------------------------------
//
TInt CCore::ObjectType()
    {
    _LIT8(KObjectType, "CCore::ObjectType %d");
    iCoreLog->Log((TDesC8)KObjectType, iObjectType);
    return iObjectType;

    }


// -----------------------------------------------------------------------------
// CCore::RunIscTestCase
// Start ISC Test Control test case
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::RunIscTestCase( CStifItemParser& aItem )
    {
    TInt testCaseId(0);
    TInt ret = aItem.GetNextInt(testCaseId);

    // Create Message Manager, LTSY Factory and Get Message Router before
    // running test cases.
    CreateMMAndLTSYfactoryAndGetMessageRouter();

    if (KErrNone == ret)
        {
        RunIscControlTestCase(1, testCaseId);
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CCore::SyncIscTestCase
// Syncronize with ISC Test Control
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::SyncIscTestCase( void )
    {
    SyncIscControl();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::RunIscTestCaseStartNTSYAfterTestToolStart
// Start ISC Test Control test case.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::RunIscTestCaseStartNTSYAfterTestToolStart( CStifItemParser& aItem )
    {
    TInt testCaseId(0);
    TInt ret = aItem.GetNextInt(testCaseId);

    // Create Message Manager, LTSY Factory and Get Message Router is done
    // in SyncIscTestCaseStartNTSYAfterTestToolStart

    if (KErrNone == ret)
        {
        RunIscControlTestCase(1, testCaseId);
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CCore::SyncIscTestCase
// Syncronize with ISC Test Control
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::SyncIscTestCaseStartNTSYAfterTestToolStart( void )
    {
    SyncIscControl();
    // Create Message Manager, LTSY Factory and Get Message Router before
    // running test cases.
    CreateMMAndLTSYfactoryAndGetMessageRouter();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined
// Start ISC Test Control test case when REAL_CELLMO_IN_USE is defined.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined( CStifItemParser& aItem )
    {
#ifdef REAL_CELLMO_IN_USE
    CStifItemParser& aItemForCompilation = aItem; // Get rid of compiler warning

    return KErrNone;
#else
    // Real CellMo is not in use - Run Isc Test Case.
    TInt testCaseId(0);
    TInt ret = aItem.GetNextInt(testCaseId);
    if (KErrNone == ret)
        {
        RunIscControlTestCase(1, testCaseId);
        }
    return ret;
#endif // REAL_CELLMO_IN_USE
    }


// -----------------------------------------------------------------------------
// CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined
// Syncronize with ISC Test Control when REAL_CELLMO_IN_USE is not defined.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined( void )
    {
#ifdef REAL_CELLMO_IN_USE
    return KErrNone;
#else
    // Real CellMo is not in use - Sync Isc Control.
    SyncIscControl();
    return KErrNone;
#endif // REAL_CELLMO_IN_USE
    }

// -----------------------------------------------------------------------------
// CCore::HandleSpecifiedRequests
// Sets iHandleSpecifiedRequests EFalse/ETrue
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::HandleSpecifiedRequests( CStifItemParser& aItem )
    {
    _LIT8 (KHandleSpecifiedRequests, "CCore::HandleSpecifiedRequests" );
    iCoreLog->Log((TDesC8)KHandleSpecifiedRequests);

    iHandleSpecifiedRequests = EFalse;

    TPtrC string;
    aItem.GetNextString ( string );
    if ( _L("YES") == string )
        {
        iHandleSpecifiedRequests = ETrue;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::InitializeIPCRequestTable
// Initializes iSavedIPCNameForComparisonTable and iSavedIPCNameForComparison
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCore::InitializeIPCRequestTable( void )
    {
    _LIT8 (KInitializeIPCRequestTable, "CCore::InitializeIPCRequestTable" );
    iCoreLog->Log((TDesC8)KInitializeIPCRequestTable);

    TInt counter;

    iSavedIPCNameForComparison.Zero();

    for ( counter = 0; counter < 150; counter++ )
        {
        iSavedIPCNameForComparisonTable[ counter ].Zero();
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::SetExpectedCompleteIPC
// Sets expected complete IPCs into table iSavedIPCNameForComparisonTable
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt CCore::SetExpectedCompleteIPC( CStifItemParser& aItem )
    {
    _LIT8 (KSetExpectedCompleteIPC, "CCore::SetExpectedCompleteIPC" );
    iCoreLog->Log((TDesC8)KSetExpectedCompleteIPC);

    TInt counter = 0;
    TPtrC string;
    aItem.GetNextString ( string );

    TPtrC string2;
    TBool mustBeThisIPC = EFalse;
    aItem.GetNextString ( string2 );
    if ( _L("MUST_BE_NEXT_IPC") == string2 )
        {
        mustBeThisIPC = ETrue;
        }

    for ( counter = 0; counter < 150; counter++ )
        {
        // Save the first string if it not saved yet
        if ( ( 0 == iSavedIPCNameForComparison.Compare ( _L( "" ) ) ) &&
             ( 0 == counter ) )
            {
            iSavedIPCNameForComparison.Append( string );
            iNextIPCMustBeSameAsNextExpected = mustBeThisIPC;
            }

        // Check the next free position and store the string into it.
        if ( 0 == iSavedIPCNameForComparisonTable[ counter ].Compare ( _L( "" ) ) )
            {
            iSavedIPCNameForComparisonTable[ counter ].Append( string );
            iNextIPCMustBeSameAsNextExpectedTable[ counter ] = mustBeThisIPC;
            counter = 150;
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::GetNextExpectedCompleteIPC
// Gets expected complete IPCs from table iSavedIPCNameForComparisonTable
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt CCore::GetNextExpectedCompleteIPC( void )
    {
    _LIT8 (KGetNextExpectedCompleteIPC, "CCore::GetNextExpectedCompleteIPC" );
    iCoreLog->Log((TDesC8)KGetNextExpectedCompleteIPC);

    TInt counter = 0;
    TPtrC string;

    for ( counter = 0; counter < 150; counter++ )
        {
        //
        if ( iSavedIPCNameForComparison == iSavedIPCNameForComparisonTable[ counter ] )
            {
            iSavedIPCNameForComparisonTable[ counter ].Zero();
            // Get next IPC
            counter++;
            if ( 0 != iSavedIPCNameForComparisonTable[ counter ].Compare ( _L( "" ) ) )
                {
                iSavedIPCNameForComparison = iSavedIPCNameForComparisonTable[ counter ];
                iNextIPCMustBeSameAsNextExpected = iNextIPCMustBeSameAsNextExpectedTable[ counter ];
                }
            counter = 150;
            }
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCore::SetOperInfoAPAC
// Setting Nokia_TSY's operator info IsOperinfoApac EFalse/ETrue
// -----------------------------------------------------------------------------
//
EXPORT_C  TInt CCore::SetOperInfoAPAC( CStifItemParser& aItem )
    {
    _LIT8 (KSetOperInfoAPAC, "CCore::SetOperInfoAPAC" );
    iCoreLog->Log((TDesC8)KSetOperInfoAPAC);

    TPtrC string;
    aItem.GetNextString ( string );

    if ( _L("OperInfoAPAC") == string )
        {
        CMmStaticUtility::SetOperinfoApac ( ETrue );
        }
    else
        {
        CMmStaticUtility::SetOperinfoApac ( EFalse );
        }
    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File

