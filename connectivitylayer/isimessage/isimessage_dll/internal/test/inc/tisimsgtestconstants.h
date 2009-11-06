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

//#pragma warning( once : 4244 )

#ifndef ISIMSGCONSTANTS_H
#define ISIMSGCONSTANTS_H

#include <e32def.h>

const TUint KLengthOfMessage = 160;
_LIT( KReceiveFail, "IsiMsg Receiving FAILED!" );
_LIT( KSendFail, "IsiMsg Sending FAILED!" );
_LIT( KMmiSsCodeFail, "GetMmiSsCode FAILED!" );
_LIT( KUssdStringFail, "GetUssdString FAILED!" );
_LIT( KGet32InfoFail, "Get32Info FAILED!" );
_LIT( KMessageIdFail, "MessageId FAILED!" );
_LIT( KGetIndexFail, "GetIndex FAILED!" );
_LIT( KPduLengthFail, "PduLength FAILED!" );
_LIT( KServiceTypeFail, "ServiceType Failed!" );
_LIT( KGetAdcReturnValueFail, "GetAdcReturnValue FAILED!" );
_LIT( KStatusFail, "GetStatus FAILED!" );
_LIT( KSubBlockFail, "SubBlock Failed!" );
_LIT( KShortInfoFail, "GetShortInfo Failed!" );
_LIT( KLongInfoFail, "GetLongInfo Failed!" );
_LIT( KSubBlockIdFail, "SubBlock Id Failed!" );
_LIT( KSubBlockByIdFail, "SubBlockById Failed!" );
_LIT( KSubBlockByIndexFail, "SubBlockByIndex Failed!" );
_LIT( KSubBlocksPossibleFail, "SubBlocksPossible Failed!" );
_LIT( KSubBlockStartFail, "SubBlocksStart Failed!" );
_LIT( KSubBlockCountIndexFail, "SubBlockCountIndex Failed!" );
_LIT( KDataFail, "GetData Failed!" );
_LIT( KCalculateItemDataFail, "CalculateItemData Failed!" );
_LIT( KCcmValueFail, "GetCcmValue Failed!");
_LIT( KCcmValueDataFail, "GetCcmValueData Failed!");
_LIT( KIsaApiNotReady, "RIsaApi Already Exist! Trying again.." );
_LIT( KIsaApiPanic, "RIsaApi Panic!!" );
_LIT( KIsaApiMsgEmpty, "CPnMsg Initialization Failed!" );
_LIT( KTimeOut, "Response TimeOut" );
_LIT( KInTime, "Response In Time" );
_LIT( KPass, "PASSED" );
_LIT( KFail, "FAILED" );
_LIT( KStart, "STARTED" );
//_LIT( KHexPrintFail, "HEX print FAILED" );
const TInt KStartLength( 7 );
const TInt KPassFailLength( 6 );

const TInt KTwo( 2 );
const TInt KSix( 6 );
const TInt KPnMsgSize( 256 );
const TInt KThousand( 1000 ); 
const TInt KOneSecond( 1000000 ); 

_LIT( KGet8BitError, "Get8bitL FAILED" );
_LIT( KGet16BitError, "Get16bitL FAILED" );
_LIT( KGet32BitError, "Get32bitL FAILED" );
_LIT( KGetDataError, "GetDataL FAILED" );
_LIT( KSet8BitError, "Set8bitL FAILED" );
_LIT( KSet16BitError, "Set16bitL FAILED" );
_LIT( KSet32BitError, "Set32bitL FAILED" );
_LIT( KSetDataError, "SetDataL FAILED" );
_LIT( KTisiEndianError, "Complete length FAILED" );
_LIT( KFindSubblockOffsetById8_8L, "FindSubblockOffsetByIdL 8/8bit FAILED" );
_LIT( KFindSubblockOffsetById8_16L, "FindSubblockOffsetByIdL 8/16bit FAILED" );
_LIT( KFindSubblockOffsetById16_8L, "FindSubblockOffsetByIdL 16/8bit FAILED" );
_LIT( KFindSubblockOffsetById16_16L, "FindSubblockOffsetByIdL 16/16bit FAILED" );
_LIT( KFindSubblockOffsetById8_32L, "FindSubblockOffsetByIdL 8/32bit FAILED" );
_LIT( KFindSubblockOffsetById, "Returned offset FAILED" );

_LIT( KFindSubblockOffsetByIndex8_8L, "FindSubblockOffsetByIndex 8/8bit FAILED" );
_LIT( KFindSubblockOffsetByIndex8_16L, "FindSubblockOffsetByIndex 8/16bit FAILED" );
_LIT( KFindSubblockOffsetByIndex16_8L, "FindSubblockOffsetByIndex 16/8bit FAILED" );
_LIT( KFindSubblockOffsetByIndex16_16L, "FindSubblockOffsetByIndex 16/16bit FAILED" );
_LIT( KFindSubblockOffsetByIndex8_32L, "FindSubblockOffsetByIndex 8/32bit FAILED" );
_LIT( KFindSubblockOffsetByIndex, "Returned offset FAILED" );
_LIT( KSubblockError, "Subblock FAILED" );
_LIT( KCopyFromBigEndianError, "CopyFromBigEndian FAILED" );
_LIT( KCopyToBigEndianError, "CopyToBigEndian FAILED" );
_LIT( KCopyDataError, "CopyData FAILED" );

const TUint8 KHexffInit = 0xff;
const TUint16 KHexffffInit = 0xffff;
const TUint16 KHex1111Init = 0x1111;
const TUint32 KHexffffffffInit = 0xffffffff;
const TUint32 KHex11111111Init = 0x11111111;

#endif /* #ifndef WRAPPERCONSTANTS_H */