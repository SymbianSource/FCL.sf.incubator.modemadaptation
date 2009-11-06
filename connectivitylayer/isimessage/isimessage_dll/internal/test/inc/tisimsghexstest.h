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
#ifndef TIsiMsgHexsTest_H
#define TIsiMsgHexsTest_H

//INCLUDE
#include <e32def.h> 

const TUint8 KHexbaFakeReq = 0xba;
const TUint8 KHexbeFakeReq = 0xbe;
const TUint8 KHexe6FakeReq = 0xe6;
const TUint8 KHexe7FakeReq = 0xe7;
const TUint8 KHexe8FakeReq = 0xe8;
const TUint8 KHexe9FakeReq = 0xe9;
const TUint8 KHexeaFakeReq = 0xea;
const TUint8 KHexebFakeReq = 0xeb;
const TUint8 KHexecFakeReq = 0xec;
const TUint8 KHexedFakeReq = 0xed;
const TUint8 KHexeeFakeReq = 0xee;
const TUint8 KHexefFakeReq = 0xef;
const TUint8 KHexf0FakeReq = 0xf0;
const TUint8 KHexf1FakeReq = 0xf1;
const TUint8 KHexf2FakeReq = 0xf2;
const TUint8 KHexf3FakeReq = 0xf3;
const TUint8 KHexf4FakeReq = 0xf4;
const TUint8 KHexf5FakeReq = 0xf5;
const TUint8 KHexf6FakeReq = 0xf6;
const TUint8 KHexf7FakeReq = 0xf7;
const TUint8 KHexf8FakeReq = 0xf8;
const TUint8 KHexf9FakeReq = 0xf9;
const TUint8 KHexfaFakeReq = 0xfa;
const TUint8 KHexfbFakeReq = 0xfb;
const TUint8 KHexfcFakeReq = 0xfc;
const TUint8 KHexfdFakeReq = 0xfd;
const TUint8 KHexfeFakeReq = 0xfe;
const TUint8 KHexffFakeReq = 0xff;

const TUint8 KHexffFakeResp = 0xff;

#define TEST_MSG_MEDIA 0x01
#define TEST_MSG_RECEIVERDEVICE 0x02
#define TEST_MSG_SENDERDEVICE 0x03
#define TEST_MSG_RESOURCEID 0x04
#define TEST_MSG_LENGTH 0x0014
#define TEST_MSG_RECEIVEROBJECT 0x07
#define TEST_MSG_SENDEROBJECT 0x08
#define TEST_MSG_TRANSID 0x09


//---------------------------------------------------------------------- 
// TestMessage: TEST_MSG_REQ                                           
// -----------------------------------------------------------------------
#define TEST_MSG_REQ 0x0A
#define TEST_MSG_RESP 0x0B

#define TEST_MSG_REQ_OFFSET TRANSACTIONID 0
#define TEST_MSG_REQ_OFFSET_MESSAGE_ID 1
#define TEST_MSG_REQ_OFFSET_FILL1 2
#define TEST_MSG_REQ_OFFSET_FILL2 3
#define TEST_MSG_REQ_OFFSET_FILL3 4
#define TEST_MSG_REQ_OFFSET_FILL4 5
#define TEST_MSG_REQ_OFFSET_FILL5 6
#define TEST_MSG_REQ_OFFSET_FILL6 7
#define TEST_MSG_REQ_OFFSET_FILL7 8
#define TEST_MSG_REQ_OFFSET_FILL8 9
#define TEST_MSG_REQ_OFFSET_FILL9 10
#define TEST_MSG_REQ_OFFSET_FILL10 11


/*
#define TEST_MSG_REQ_OFFSET_NUMBER_OF_SUBBLOCKS 4
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_ID 5
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_SIZE 6
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_8BIT 7
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_16BIT 8
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_32BIT 10
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_DATA1 14
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_FILL1 18
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_FILL2 19
#define TEST_MSG_REQ_OFFSET_SUBBLOCK_FILL3 20
*/
#define SIZE_TEST_MSG_REQ 20



#endif