/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef DPDEF_H
#define DPDEF_H

// INCLUDES
#include <pn_const.h> // phonet constants
#include <pipeisi.h>  // pipe constants
#include <c32comm.h>  // c32 comms server
#include <flogger.h>

#ifdef _DEBUG
#include <e32svr.h>
#endif

#if ( defined USE_FILE_DEBUG || defined USE_SERIAL_DEBUG || defined USE_TRUST_DEBUG )
    #define USE_FILE_SERIAL_TRUST_DEBUG
#endif

// LOCAL CONSTANTS
const TUint KDpMaxMsgReceiveSize = 30;
const TUint KDpMaxMsgSendSize = 30;

// Filler byte for ISI messages
const TUint8 KDpPadding = 0x00;

// Supported dataport unit numbers
const TUint KDataPortCommLowUnit = 0;
const TUint KDataPortCommHighUnit = 6;

// Should be always least KDpMaximumTxReservationSize
const TInt KDpMaximumEchoDataSize = 2048;

const TInt KDpRxBufferInitialSize = 8192;

const TInt KDpTxBufferInitialSize = 4096;
// This is not any calculated value.
const TInt KDpMaximumSetReceiveBufferLength = 20480;

// Prosents of rx-buffer
const TInt KDpFlowCtrlWaterMarkLowProsOfBuff = 20;
const TInt KDpFlowCtrlWaterMarkHighProsOfBuff = 50;

// Active object Priorities
const TInt KDpDte2TxPriority = 15;
const TInt KDpTx2PnPriority = 12;
const TInt KDpPn2RxPriority = 15;
const TInt KDpRx2DtePriority = 12;
const TInt KDpEscDetectPriority = 11;
const TInt KDpPifPriority = 13;
const TInt KDpMifPriority = 15;
const TInt KDpFlowCtrlPriority = 16;

// Parity table
const TInt KDpParityTableSize = 128;
const TInt KDpParityTableWidth = 4;

// From PhoNet Comm PEP Spec
const TInt KDpEscapeTimeMultiplier = 20;
// 1 sec = 1000ms
const TInt KDpEscapeSecondAsMilliseconds = 1000;
const TInt KDpEscapeSeqLength = 3;

// Dataport unit numbers (to open dataport)
// CSD port (internal connection)
const TUint KDpCsdPort = 0;
// Dial-up port (external connections, CSD & PSD)
const TUint KDpDialUpPort = 1;
// Bluetooth handsfree
const TUint KDpBtAccHfPort = 2;
// Bluetooth headset
const TUint KDpBtAccHsPort = 3;
// Additional dial-up port
const TUint KDpPort4 = 4;
// Additional dial-up port
const TUint KDpPort5 = 5;
// Additional internal port
const TUint KDpPort6 = 6;

const TUint8 KDpFillChar = 'A';

const TUint8 KDcsDefaultXonChar = 0x11;
const TUint8 KDcsDefaultXoffChar = 0x13;

const TUint KEscTime50millisec = 50;
const TUint8 KEscChar = '+';

const TUint KDTEDeltaInput = ( KDSRChanged | KDCDChanged | KRNGChanged );
const TUint KDTESignalAll = ( KSignalDTEOutputs | KSignalDTEInputs );
const TUint KDCEDeltaInput = ( KRTSChanged | KDTRChanged );
const TUint KDCESignalAll = ( KSignalDCEOutputs | KSignalDCEInputs );

// To include pep_comm_types.h
#define byte char

#ifdef  USE_FILE_DEBUG
_LIT( KDataPortFileName, "dataport.txt" );
#endif

_LIT( KDataPortModule, "DATAPORT" );
_LIT( KDataPortDescription, "DataPort" );

// DATA TYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION
// none

#endif  // DPDEF_H

//  End of File
