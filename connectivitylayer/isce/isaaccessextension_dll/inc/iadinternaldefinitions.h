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
*
*/



/************** NOTE ! *****************
*
* TODO:
*
* TO BE CHANGED ALL COMMON INC FILES TO
* ISAACCESSDRIVER\INC
*
************** NOTE ! *****************/


#ifndef ISAACCESSDRIVER_INTERNALDEFINITIONS_H
#define ISAACCESSDRIVER_INTERNALDEFINITIONS_H

//  INCLUDES
#ifndef __KERNEL_MODE__
#include <e32std.h>
#else
#include <kernel.h>
#endif // __KERNEL_MODE__

// CONSTANTS
_LIT( KIADDriverName, "IADDriver" );
_LIT( KNameOfIsaAccessDriverLdd, "isaaccessldd.ldd" );

// connection status enumeration
enum TIADConnectionStatus
    {
    EIADConnectionOk=0,
    EIADConnectionNotOk
    };

// flow control status enumeration
enum TIADFlowControlStatus
    {
    EIADFlowControlOff=0,
    EIADFlowControlOn,
    EIADTransmissionEnd
    };

const TUint16 KIADControlChannel = 0x0000;
const TUint16 KIADFrameReceiverNotFound = 0x500;
const TUint16 KIADAllChannels = 0x100;
const TUint16 KNotInitializedChannel( 0xffff );

// DFC Queue priorities inside IAD
const TInt KIADLddEmptyRxQueuePriori( 2 );
const TInt KIADLddEmptyDataRxQueuePriori( 2 );
const TInt KIADLddConnStatPriori( 3 );
const TInt KIADLddFlowStatPriori( 3 );
const TInt KIADLddCompleteChannelPriori( 2 );
const TInt KIADKernelChMsgQueDfcPriori( 1 );
const TInt KIADExtCommonRxPriori( 7 );
const TInt KIADExtInitCmtPriori( 7 );
const TInt KIADExtConnStatPriori( 7 );
const TInt KIADKernelEmptyRxQueuePriori( 0 );

/////////////// THESE CAN BE EDITED DEPENDING ON PRODUCT CONFIGURATIONS ///////////////
// 
// This kind of configurations are needed cause we must support legacy flowcontrol 
// protocoll too.
//
// Databuffer size in bytes.
const TInt KDataSizeInBytesWithPipeHeader( 2048 );// Re-used due to buffer configurations
const TInt KDataCalculateDataQueusWithDataSize( 1536 );

// DATA SPEED IN DOWNLINK AND UPLINK
// UPLINK:( NOT USED YET )
// CASW_ISC_UPLINK_DATA_RATE
// DOWNLINK:
// CASW_ISC_DOWNLINK_DATA_RATE
// Max delay from flowcontrol sending to air interface closing time in milliseconds.
const TInt KMaxDelayAfterFlowCtrlOnInMs( 150 );
// Max size of the data burst in bytes.
//const TInt KMaxDataBurstInBytes( 81920 );
// NW configured max RLC buffer size for HSDPA3.6 = 320*2047 
const TInt KMaxDataBurstInBytes( ( 320 * 2047 ) / 8 );

//////////////////////////////// DO NOT EDIT AFTER THIS ////////////////////////////////

// QUEUE CONFIGURATIONS
const TInt KMaxDataSpeedInBytes( CASW_ISC_DOWNLINK_DATA_RATE / 8 );
const TInt KMaxDataAfterDelay( KMaxDataSpeedInBytes / 1000 * KMaxDelayAfterFlowCtrlOnInMs );
// Plus one so we round upwards.
const TInt KMaxNumberOfDataMsgAfterDelay( KMaxDataAfterDelay / KDataCalculateDataQueusWithDataSize + 1 );// 45
const TInt KMaxNumberOfDataMsgInBurst( KMaxDataBurstInBytes / KDataCalculateDataQueusWithDataSize + 1 );// 54
const TInt KMaxDataSpaceAfterHW( KMaxNumberOfDataMsgAfterDelay + KMaxNumberOfDataMsgInBurst ); // 99
// NOTE!! This is for HSDPA where we need DL2UL1 messages. When HSUPA used modify this.
const TInt KMaxDataSpaceUntilULLW( 10 );

// Receive queue for PIPE messages.
const TUint16 KIADChDataRxQueueSize( KMaxDataSpaceAfterHW + KMaxNumberOfDataMsgInBurst + ( KMaxNumberOfDataMsgAfterDelay / 2 ) );
//const TInt KTotalDataSpace( KIADChannelDataReceiveQueueSize + KIADChannelSendQueueSize );// 225
// Here we must have space for maxburst and the data send during delay.
const TUint16 KIADChDataRxHighWaterMark( KIADChDataRxQueueSize - KMaxDataSpaceAfterHW );
// Low water mark can be half of delay. It's not crucial to set it on as fast as possible.
// Half of the time is enough.
const TUint16 KIADChDataRxLowWaterMark( KMaxNumberOfDataMsgAfterDelay / 2 );

#if (CASW_ISC_DOWNLINK_DATA_RATE == CASW_ISC_DATA_RATE_3600000_BITS) 
#define CREDIT_AMOUNT 10
#define CREDIT_THRESHOLD 7
// If we have lower bitrate we can exchange credits more often. So we can guarantee that we have enough memory also in situations when all
// the possible parallel datachannels are used.
#elif (CASW_ISC_DOWNLINK_DATA_RATE < CASW_ISC_DATA_RATE_3600000_BITS)
#define CREDIT_AMOUNT 5
#define CREDIT_THRESHOLD 2
#endif //  CASW_ISC_DOWNLINK_DATA_RATE < CASW_ISC_DATA_RATE_3600000_BITS

//////////////////////////////// DO NOT EDIT BEFORE THIS ////////////////////////////////

const TInt KIADMaxIsiMsgSize( 2048 ); // TODO: to be changed for MINI_OS could it be static?

// QUEUE CONFIGURATIONS
// TODO: configure properly
const TInt KIADExtensionRxQueueSize( 200);//50 );
const TInt KIADLddFlowCtrlRxQueuSize( 5 );
const TInt KIADLddRxQueuSize( 50 );
const TInt KIADLddDataRxQueuSize( KIADChDataRxQueueSize );
const TInt KIADKernelRxQueuSize( 50 );
const TInt KIADKernelDataRxQueuSize( 100 );
// Re-used due to buffer configurations TODO: how to ensure enough blocks without cbfc and knowing how big is the IST send que?
const TInt KTotalDataSpace( KIADLddDataRxQueuSize + 50 );

// DFC Thread names.
_LIT8( KIADLddDfc, "IADLddDFC" );
_LIT8( KIADExtensionDfc, "IADExtensionDFC" );

// MACROS

// DATA TYPES

// enumerations for asynchronous requests in IAD
enum TIADAsyncRequest
    {
    EIADAsyncOpen = 0,
    EIADAsyncClose,
    EIADAsyncSend,
    EIADAsyncReceive,
    EIADAsyncDataSend,
    EIADAsyncDataReceive,
    EIADAsyncNotifyConnectionStatus,
    EIADAsyncNotifyFlowControlStatus,
    EIADAsyncSubscribeIndications,
    EIADAsyncSendIndication,
    EIADAsyncSubscribeIndications32Bit,
    EIADAsyncFTDInformation,
    EIADAsyncLast
    };

// enumerations for synchronous requests in IAD
enum TIADRequest
    {
    EIADClose = EIADAsyncLast, // 12
    EIADSend,
    EIADDataSend,
    EIADGetConnectionStatus,
    EIADGetFlowControlStatus,
    EIADGetChannelInfo,
    EIADGetMaxDataSize,
    EIADSubscribeIndications,
    EIADSendIndication,
    EIADSubscribeIndications32Bit,
    EIADResetQueues,
    EIADAllocateBlock,
    EIADDeAllocateBlock,
    EIADAllocateDataBlock,
    EIADDeAllocateDataBlock,
    EIADDeAllocateBlockInternal,
    EIADDeAllocateDataBlockInternal,
#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
    EIADLoan,
    EIADReturnLoan,
#endif
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif      // ISAACCESSDRIVER_INTERNALDEFINITIONS_H

// End of File
