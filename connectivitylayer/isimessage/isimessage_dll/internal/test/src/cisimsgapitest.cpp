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


// INCLUDES             
#include <f32file.h>                    // For RDebug

#include "tisimsghexstest.h"            // For 0x00 etc..
#include "cisimsgapitest.h"
#include "cisimsgbasetest.h"            // For CIsiMsgBaseTest

#include <phonetisi.h>
#include <netisi.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS

_LIT( KIsiMsgSet8BitTest0, "MOD_0000.00 ISIMESSAGE [TIsiSend::Set8Bit Last offset]" );
_LIT( KIsiMsgSet8BitTest1, "MOD_0000.01 ISIMESSAGE [TIsiSend::Set8Bit First offset]" );
_LIT( KIsiMsgSet8BitTest2, "MOD_0000.02 ISIMESSAGE [TIsiSend::Set8Bit Msg header]" );
_LIT( KIsiMsgSet8BitTest3, "MOD_0000.03 ISIMESSAGE [TIsiSend::Set8Bit Middle offsets]" );
_LIT( KIsiMsgSet8BitTest4, "MOD_0000.04 ISIMESSAGE [TIsiSend::Set8Bit Negative offset]" );
_LIT( KIsiMsgSet8BitTest5, "MOD_0000.05 ISIMESSAGE [TIsiSend::Set8Bit Overflow offset]" );

_LIT( KIsiMsgSet16BitTest0, "MOD_0000.06 ISIMESSAGE [TIsiSend::Set16Bit Last offset]" );
_LIT( KIsiMsgSet16BitTest1, "MOD_0000.07 ISIMESSAGE [TIsiSend::Set16Bit First offset]" );
_LIT( KIsiMsgSet16BitTest2, "MOD_0000.08 ISIMESSAGE [TIsiSend::Set16Bit Msg header]" );
_LIT( KIsiMsgSet16BitTest3, "MOD_0000.09 ISIMESSAGE [TIsiSend::Set16Bit Middle offsets]" );
_LIT( KIsiMsgSet16BitTest4, "MOD_0000.10 ISIMESSAGE [TIsiSend::Set16Bit Negative offset]" );
_LIT( KIsiMsgSet16BitTest5, "MOD_0000.11 ISIMESSAGE [TIsiSend::Set16Bit Overflow offset]" );

_LIT( KIsiMsgSet32BitTest0, "MOD_0000.12 ISIMESSAGE [TIsiSend::Set32Bit Last offset]" );
_LIT( KIsiMsgSet32BitTest1, "MOD_0000.13 ISIMESSAGE [TIsiSend::Set32Bit First offset]" );
_LIT( KIsiMsgSet32BitTest2, "MOD_0000.14 ISIMESSAGE [TIsiSend::Set32Bit Msg header]" );
_LIT( KIsiMsgSet32BitTest3, "MOD_0000.15 ISIMESSAGE [TIsiSend::Set32Bit Middle offsets]" );
_LIT( KIsiMsgSet32BitTest4, "MOD_0000.16 ISIMESSAGE [TIsiSend::Set32Bit Negative offset]" );
_LIT( KIsiMsgSet32BitTest5, "MOD_0000.17 ISIMESSAGE [TIsiSend::Set32Bit Overflow offset]" );

_LIT( KIsiMsgCopyData, "MOD_0000.18 ISIMESSAGE [TIsiSend::CopyData]" );

_LIT( KIsiMsgGet8BitTest0, "MOD_0001.00 ISIMESSAGE [TIsiReceiveC::Get8Bit Last offset]" );
_LIT( KIsiMsgGet8BitTest1, "MOD_0001.01 ISIMESSAGE [TIsiReceiveC::Get8Bit First offset]" );
_LIT( KIsiMsgGet8BitTest2, "MOD_0001.02 ISIMESSAGE [TIsiReceiveC::Get8Bit Msg header]" );
_LIT( KIsiMsgGet8BitTest3, "MOD_0001.03 ISIMESSAGE [TIsiReceiveC::Get8Bit Middle offsets]" );
_LIT( KIsiMsgGet8BitTest4, "MOD_0001.04 ISIMESSAGE [TIsiReceiveC::Get8Bit Negative offset]" );
_LIT( KIsiMsgGet8BitTest5, "MOD_0001.05 ISIMESSAGE [TIsiReceiveC::Get8Bit Overflow offset]" );

_LIT( KIsiMsgGet16BitTest0, "MOD_0001.06 ISIMESSAGE [TIsiReceiveC::Get16Bit Last offset]" );
_LIT( KIsiMsgGet16BitTest1, "MOD_0001.07 ISIMESSAGE [TIsiReceiveC::Get16Bit First offset]" );
_LIT( KIsiMsgGet16BitTest2, "MOD_0001.08 ISIMESSAGE [TIsiReceiveC::Get16Bit Msg header]" );
_LIT( KIsiMsgGet16BitTest3, "MOD_0001.09 ISIMESSAGE [TIsiReceiveC::Get16Bit Middle offsets]" );
_LIT( KIsiMsgGet16BitTest4, "MOD_0001.10 ISIMESSAGE [TIsiReceiveC::Get16Bit Negative offset]" );
_LIT( KIsiMsgGet16BitTest5, "MOD_0001.11 ISIMESSAGE [TIsiReceiveC::Get16Bit Overflow offset]" );

_LIT( KIsiMsgGet32BitTest0, "MOD_0001.12 ISIMESSAGE [TIsiReceiveC::Get32Bit Last offset]" );
_LIT( KIsiMsgGet32BitTest1, "MOD_0001.13 ISIMESSAGE [TIsiReceiveC::Get23Bit First offset]" );
_LIT( KIsiMsgGet32BitTest2, "MOD_0001.14 ISIMESSAGE [TIsiReceiveC::Get32Bit Msg header]" );
_LIT( KIsiMsgGet32BitTest3, "MOD_0001.15 ISIMESSAGE [TIsiReceiveC::Get32Bit Middle offsets]" );
_LIT( KIsiMsgGet32BitTest4, "MOD_0001.16 ISIMESSAGE [TIsiReceiveC::Get32Bit Negative offset]" );
_LIT( KIsiMsgGet32BitTest5, "MOD_0001.17 ISIMESSAGE [TIsiReceiveC::Get32Bit Overflow offset]" );

_LIT( KIsiMsgGetData0, "MOD_0001.18 ISIMESSAGE [TIsiReceiveC::GetData]" );
_LIT( KIsiMsgGetData1, "MOD_0001.19 ISIMESSAGE [TIsiReceiveC::GetData Negative offset]" );
_LIT( KIsiMsgGetData2, "MOD_0001.20 ISIMESSAGE [TIsiReceiveC::GetData Overflow offset]" );
_LIT( KIsiMsgGetData3, "MOD_0001.21 ISIMESSAGE [TIsiReceiveC::GetData length 0]" );

_LIT( KIsiMsgFindSubBlockOffsetById0, "MOD_0002.00 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById1, "MOD_0002.01 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById2, "MOD_0002.02 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById3, "MOD_0002.03 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById4, "MOD_0002.04 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]" );

_LIT( KIsiMsgFindSubBlockOffsetById5, "MOD_0002.05 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById6, "MOD_0002.06 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById7, "MOD_0002.07 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById8, "MOD_0002.08 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById9, "MOD_0002.09 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]" );

_LIT( KIsiMsgFindSubBlockOffsetById10, "MOD_0002.10 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById11, "MOD_0002.11 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById12, "MOD_0002.12 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById13, "MOD_0002.13 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById14, "MOD_0002.14 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]" );

_LIT( KIsiMsgFindSubBlockOffsetById15, "MOD_0002.15 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById16, "MOD_0002.16 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById17, "MOD_0002.17 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetById18, "MOD_0002.18 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetById19, "MOD_0002.19 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]" );
_LIT( KIsiMsgFindSubBlockOffsetById20, "MOD_0002.20 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById sb_len == 0 ]" );
_LIT( KIsiMsgFindSubBlockOffsetById21, "MOD_0002.21 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById sb not found ]" );
_LIT( KIsiMsgFindSubBlockOffsetById22, "MOD_0002.22 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById sb doesn't exist ]" );

_LIT( KIsiMsgFindSubBlockOffsetByIndex0, "MOD_0003.00 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex1, "MOD_0003.01 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex2, "MOD_0003.02 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex3, "MOD_0003.03 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex4, "MOD_0003.04 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]" );

_LIT( KIsiMsgFindSubBlockOffsetByIndex5, "MOD_0003.05 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex6, "MOD_0003.06 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex7, "MOD_0003.07 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex8, "MOD_0003.08 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex9, "MOD_0003.09 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]" );

_LIT( KIsiMsgFindSubBlockOffsetByIndex10, "MOD_0003.10 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex11, "MOD_0003.11 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex12, "MOD_0003.12 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex13, "MOD_0003.13 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex14, "MOD_0003.14 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]" );

_LIT( KIsiMsgFindSubBlockOffsetByIndex15, "MOD_0003.15 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex16, "MOD_0003.16 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex17, "MOD_0003.17 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex18, "MOD_0003.18 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex19, "MOD_0003.19 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex20, "MOD_0003.20 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex sb_len == 0 ]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex21, "MOD_0003.21 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex msglen not %4 ]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex22, "MOD_0003.22 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex sb not found ]" );
_LIT( KIsiMsgFindSubBlockOffsetByIndex23, "MOD_0003.23 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex sb doesn't exist ]" );


_LIT( KIsiMsgTIsiSendTest, "MOD_0004.00 ISIMESSAGE [TIsiSend::Set8Bit with new constructor]" );
_LIT( KIsiMsgTIsiSendTest1, "MOD_0004.01 ISIMESSAGE [TIsiSend::Set16Bit with new constructor]" );
_LIT( KIsiMsgTIsiSendTest2, "MOD_0004.02 ISIMESSAGE [TIsiSend::Set32Bit with new constructor]" );
_LIT( KIsiMsgTIsiSendTest3, "MOD_0004.03 ISIMESSAGE [TIsiSend::Complete finallength ]" );

_LIT( KTIsiSubBlockTest, "MOD_0005.00 ISIMESSAGE [TIsiSubBlock]" );
_LIT( KTIsiSubBlockTest1, "MOD_0005.01 ISIMESSAGE [TIsiSubBlock1]" );
_LIT( KTIsiSubBlockTest2, "MOD_0005.02 ISIMESSAGE [TIsiSubBlock2]" );
_LIT( KTIsiSubBlockTest3, "MOD_0005.03 ISIMESSAGE [TIsiSubBlock3]" );
_LIT( KTIsiSubBlockTest4, "MOD_0005.04 ISIMESSAGE [TIsiSubBlock4]" );
_LIT( KTIsiSubBlockTest5, "MOD_0005.05 ISIMESSAGE [TIsiSubBlock error case]" );

_LIT( KGetBufferTest, "MOD_0003.23 ISIMESSAGE [TIsiReceiveC::GetBuffer use case]" );

_LIT( KCopyFromBigEndianTest, "MOD_0006.00 ISIMESSAGE [TIsiUtility::CopyFromBigEndian]" );
_LIT( KCopyToBigEndianTest, "MOD_0006.01 ISIMESSAGE [TIsiUtility::CopyToBigEndian]" );
_LIT( KCopyFromBigEndianTest1, "MOD_0006.02 ISIMESSAGE [TIsiUtility::CopyFromBigEndian1]" );
_LIT( KCopyToBigEndianTest1, "MOD_0006.03 ISIMESSAGE [TIsiUtility::CopyToBigEndian1]" );


const TInt KMsg10Length( 10 );
const TInt KMsg11Length( 11 );
const TInt KMsg12Length( 12 );
const TInt KMsg16Length( 16 );
const TInt KMsg20Length( 20 );
const TInt KMsg28Length( 28 );
const TInt KMsgMaxLength( 1024 ); // This just for tests not actually a max length.

const TInt KMinusOne( ( -1 ) );
const TUint KOffsetMinusOne( KMinusOne );
const TInt KZero( 0 );
const TInt KOne( 1 );
const TInt KThree( 3 );
const TInt KFour( 4 );
const TInt KFive( 5 );
const TInt KSeven( 7 );
const TInt KEight( 8 );
const TInt KNine( 9 );
const TInt KTen( 10 );
const TInt KEleven( 11 );
const TInt KTwelve( 12 );
const TInt KThirteen( 13 );
const TInt KFourteen( 14 );
const TInt KFifteen( 15 );
const TInt KSixteen( 16 );
const TInt KSeventeen ( 17 );
const TInt KEightteen( 18 );
const TInt KNineteen( 19 );
const TInt KTwenty( 20 );
const TInt KTwentyOne( 21 );
const TInt KTwentyFour( 24 );
//const TInt KThirty ( 30 );
const TInt KThirtyTwo ( 32 );

const TUint8 KEightBitNull( 0x00 );
const TUint16 KSixteenBitNull( 0x0000 );
const TUint32 KThirtytwoBitNull( 0x00000000 );
TUint aSubBlockOffset ( 0x00 ) ;


// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================
// None

// ============================ MGpdsBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::NewL
// Second phase static constuctor.
// -----------------------------------------------------------------------------
//
CIsiMsgApiTest* CIsiMsgApiTest::NewL
        (
        // None
        )
    {

    // Construct new CIsiMsgApiTest instance.
    CIsiMsgApiTest* self = new ( ELeave ) CIsiMsgApiTest();    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
  
    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::~CIsiMsgApiTest
// Destructor
// -----------------------------------------------------------------------------
//    
CIsiMsgApiTest::~CIsiMsgApiTest
        (
        // None
        )
    {

    // None

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase00L
//
// MOD_0000.00 ISIMESSAGE [TIsiSend::Set8Bit Last offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase00L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTwenty );

        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL2, 0x01 );
        msg.Set8bit( KNineteen, 0x0f );
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[ KNineteen ] != 0x0f || testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KNineteen );

        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg2.Set8bit( KEightteen, 0x0f );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();
        
        if( testMsg2[ KEightteen ] != 0x0f || testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

      TBuf8<21> testMsg3;
        TIsiSend msg3( testMsg3 );

        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg3.Set8bit( KSeventeen, 0x0f );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[ KSeventeen ] != 0x0f || testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }
          
   /*
   test that tisisend constructor doesn't work correctly if buffer taken from heap
        HBufC8* msgBuf = HBufC8::NewLC( 21 );
        TPtr8 msgPtr( msgBuf->Des() );
        TIsiSend isimessage( msgPtr );        
        isimessage.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
*/


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase01L
//
// MOD_0000.01 ISIMESSAGE [TIsiSend::Set8Bit First offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase01L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KEleven );

        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );
        msg.Set8bit( KZero, 0x0f );
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[ KZero ] != 0x0f || testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KEleven );

        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );
        msg2.Set8bit( KZero, 0x0f );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();
        
        if( testMsg2[ KZero ] != 0x0f || testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KEleven );

        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );
        msg3.Set8bit( KZero, 0x0f );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[ KZero ] != 0x0f || testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase02L
//
// MOD_0000.02 ISIMESSAGE [TIsiSend::Set8Bit Msg header]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase02L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTen );
        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, TEST_MSG_RECEIVERDEVICE );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, TEST_MSG_SENDERDEVICE );
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );

        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, TEST_MSG_RECEIVEROBJECT );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, TEST_MSG_SENDEROBJECT );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );

        TBuf8<KTwenty> readyMsg = msg.Complete(); //complete sets length = buffer length - kmsgheaderlen(6)
        
            if( testMsg[KFour] != 0x00 || testMsg[KFive]!= 0x04)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
               }
            if( testMsg[ KZero ] != TEST_MSG_MEDIA || testMsg[ KOne ] != TEST_MSG_RECEIVERDEVICE
             || testMsg[ KTwo ] != TEST_MSG_SENDERDEVICE || testMsg[ KThree ] != TEST_MSG_RESOURCEID
             || testMsg[ KSix ] != TEST_MSG_RECEIVEROBJECT
             || testMsg[ KSeven ] != TEST_MSG_SENDEROBJECT || testMsg[ KEight ] != TEST_MSG_TRANSID
             || testMsg[ KNine ] != TEST_MSG_REQ || testMsg.Compare( readyMsg ) != KErrNone )
            {
                testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }               
     

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KTen );
        msg2.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg2.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, TEST_MSG_RECEIVERDEVICE );
        msg2.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, TEST_MSG_SENDERDEVICE );
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
     
        msg2.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, TEST_MSG_RECEIVEROBJECT );
        msg2.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, TEST_MSG_SENDEROBJECT );
        msg2.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );

        HBufC8* readyMsg2 = msg2.Complete().AllocLC();
        
            if( testMsg2[KFour] != 0x00 || testMsg2[KFive]!= 0x04)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
               }
            if( testMsg2[ KZero ] != TEST_MSG_MEDIA || testMsg2[ KOne ] != TEST_MSG_RECEIVERDEVICE
             || testMsg2[ KTwo ] != TEST_MSG_SENDERDEVICE || testMsg2[ KThree ] != TEST_MSG_RESOURCEID
             || testMsg2[ KSix ] != TEST_MSG_RECEIVEROBJECT
             || testMsg2[ KSeven ] != TEST_MSG_SENDEROBJECT || testMsg2[ KEight ] != TEST_MSG_TRANSID
             || testMsg2[ KNine ] != TEST_MSG_REQ || testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
                testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }               

        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KTen );
        msg3.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg3.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, TEST_MSG_RECEIVERDEVICE );
        msg3.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, TEST_MSG_SENDERDEVICE );
        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );

        msg3.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, TEST_MSG_RECEIVEROBJECT );
        msg3.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, TEST_MSG_SENDEROBJECT );
        msg3.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

           if( testMsg3[KFour] != 0x00 || testMsg3[KFive]!= 0x04)
               {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
               }
               if( testMsg3[ KZero ] != TEST_MSG_MEDIA || testMsg3[ KOne ] != TEST_MSG_RECEIVERDEVICE
             || testMsg3[ KTwo ] != TEST_MSG_SENDERDEVICE || testMsg3[ KThree ] != TEST_MSG_RESOURCEID
             || testMsg3[ KSix ] != TEST_MSG_RECEIVEROBJECT
             || testMsg3[ KSeven ] != TEST_MSG_SENDEROBJECT || testMsg3[ KEight ] != TEST_MSG_TRANSID
             || testMsg3[ KNine ] != TEST_MSG_REQ || testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
                testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }               


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase03L
//
// MOD_0000.03 ISIMESSAGE [TIsiSend::Set8Bit Middle offsets]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase03L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  Set8Bit with %2, %4 and %any and values and L == MaxL
    //  0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
    TIsiSend msg( testMsg, KTwenty );

    for( TInt k(KZero); k < KTwenty; k++)
    {
      msg.Set8bit( k, k);
    }
    msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
    msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
    TBuf8<KTwenty> readyMsg = msg.Complete();
    if( testMsg.Compare( readyMsg ) != KErrNone )
        {
         testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
        }    
    else {
        for( TInt k(KZero); k < KTwenty; k++)
            {
            if( k == KFour || k == KFive || k == KEight || k == KNine) 
                {
                if(k == KEight && testMsg[ KEight ] != TEST_MSG_TRANSID ) testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
                if(k== KNine && testMsg[ KNine ] != TEST_MSG_REQ ) testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
                }
            else{ 
                if(testMsg[ k ] != k ){ testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );}
                }
            }
        }

    TBuf8<KTwenty> testMsg2;
    TIsiSend msg2( testMsg2, KNineteen );

        for( TInt y = KZero; y < KNineteen; y++)
        {
        msg2.Set8bit( y, y ); 
         }
    msg2.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
    msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
    HBufC8* readyMsg2 = msg2.Complete().AllocLC();
    if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
        {
         testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
        }    
    else {
        for( TInt k(KZero); k < KNineteen; k++)
            {
            if( k == KFour || k == KFive || k == KEight || k == KNine) 
                {
                if(k == KEight && testMsg2[ KEight ] != TEST_MSG_TRANSID ) testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
                if(k== KNine && testMsg2[ KNine ] != TEST_MSG_REQ ) testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
                }
            else{ 
                if(testMsg2[ k ] != k ){ testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );}
                }
            }
        }
    CleanupStack::PopAndDestroy( readyMsg2 );

    TBuf8<KTwenty> testMsg3;
    TIsiSend msg3( testMsg3, KEightteen );

        for( TInt x = KZero; x < KEightteen; x++)
        {
        msg3.Set8bit( x, x ); 
         }
    msg3.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
    msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
    
    TBuf8<KTwenty> readyMsg3 = msg3.Complete();
    if( testMsg3.Compare( readyMsg3 ) != KErrNone )
        {
         testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
        }    
    else {
        for( TInt k(KZero); k < KEightteen; k++)
            {
            if( k == KFour || k == KFive || k == KEight || k == KNine) 
                {
                if(k == KEight && testMsg3[ KEight ] != TEST_MSG_TRANSID ) testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
                if(k == KNine && testMsg3[ KNine ] != TEST_MSG_REQ ) testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
                }
            else{ 
                if(testMsg3[ k ] != k ){ testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );}
                }
            }
        }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase04L
//
// MOD_0000.04 ISIMESSAGE [TIsiSend::Set8bit Negative offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase04L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set8Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
    TIsiSend msg( testMsg, KTwenty );

    // Try negative offset
    msg.Set8bit( KOffsetMinusOne, 0x00 ); //=>PANIC :)
   
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase05L
//
// MOD_0000.05 ISIMESSAGE [TIsi::Set8bit Overflow offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase05L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set8Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
    TIsiSend msg( testMsg, KTwenty );

    // Try overflow offset
    msg.Set8bit( KTwentyOne, 0x00 );
  
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase06L
//
// MOD_0000.06 ISIMESSAGE [TIsiSend::Set16Bit Last offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase06L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();


        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTwenty );    //write a message for sending.

        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.Set16bit( KEightteen, 0x1122 );
        
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[ KEightteen ] != 0x11 || testMsg[ KNineteen ] != 0x22 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
  
        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KNineteen );

        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg2.Set16bit( KSixteen, 0x1223 );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

        if( testMsg2[ KSixteen ] != 0x12 || testMsg2[ KSeventeen ] != 0x23 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        else if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KEightteen );

        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg3.Set16bit( KSixteen, 0x1324 );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[ KSixteen ] != 0x13 || testMsg3[ KSeventeen ] != 0x24 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
      
        else if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase07L
//
// MOD_0000.07 ISIMESSAGE [TIsiSend::Set16Bit First offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase07L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTen );    //write a message for sending.

        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.Set16bit( 0, 0x1122 );
        
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[KZero] != 0x11 || testMsg[KOne] != 0x22 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    
        else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
  
        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KTen );

        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg2.Set16bit( 0, 0x1223 );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

        if( testMsg2[KZero] != 0x12 || testMsg2[KOne] != 0x23 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        else if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KTen );

        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg3.Set16bit( 0, 0x1324 );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[KZero] != 0x13 || testMsg3[KOne] != 0x24 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        else if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }

      //  }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase08L
//
// MOD_0000.08 ISIMESSAGE [TIsiSend::Set16Bit Msg header ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase08L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
    TIsiSend msg( testMsg, KTen );

    msg.Set16bit( ISI_HEADER_OFFSET_MEDIA, 0x0102 );
    msg.Set16bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x0103 );
    msg.Set16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x0104 );
    msg.Set16bit( ISI_HEADER_OFFSET_TRANSID, 0x0201 );

    TBuf8<KTwenty> readyMsg = msg.Complete();

            if( testMsg[ KZero ] != 0x01 || testMsg[ KOne ] != 0x02 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
            else if( testMsg[ KTwo ] != 0x01 || testMsg[ KThree ] != 0x03 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg[ KSix ] != 0x01 || testMsg[ KSeven ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg[ KEight ] != 0x02 || testMsg[ KNine ] != 0x01 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }    
            else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }                               



       TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KNineteen );

        msg2.Set16bit( ISI_HEADER_OFFSET_MEDIA, 0x0102 );
        msg2.Set16bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x0103 );
 
        msg2.Set16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x0104 );
        msg2.Set16bit( ISI_HEADER_OFFSET_TRANSID, 0x0201 );

        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

            if( testMsg2[ KZero ] != 0x01 || testMsg2[ KOne ] != 0x02 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
            else if( testMsg2[ KTwo ] != 0x01 || testMsg[ KThree ] != 0x03 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg2[ KSix ] != 0x01 || testMsg2[ KSeven ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg2[ KEight ] != 0x02 || testMsg2[ KNine ] != 0x01 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }    
            else if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }                                                                   

        CleanupStack::PopAndDestroy( readyMsg2 );        

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KTen );

        msg3.Set16bit( ISI_HEADER_OFFSET_MEDIA, 0x0102 );
        msg3.Set16bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x0103 );
        msg3.Set16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x0104 );
        msg3.Set16bit( ISI_HEADER_OFFSET_TRANSID, 0x0201 );

        TBuf8<KTwenty> readyMsg3 = msg3.Complete();        

            if( testMsg3[ KZero ] != 0x01 || testMsg3[ KOne ] != 0x02 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
            else if( testMsg3[ KTwo ] != 0x01 || testMsg3[ KThree ] != 0x03 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg3[ KSix ] != 0x01 || testMsg3[ KSeven ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg3[ KEight ] != 0x02 || testMsg3[ KNine ] != 0x01 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }    
            else if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }  
            
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
    

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase09L
//
// MOD_0000.09 ISIMESSAGE [TIsiSend::Set16Bit Middle offsets]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase09L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {
    //
    //  Set16Bit with %2, %4 and %any and values and L == MaxL
    //  0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
    TIsiSend msg( testMsg, KTwenty );

    msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
    msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
    msg.Set16bit( KTen, 0x1122 );
    msg.Set16bit( KTwelve, 0x1133 );
    msg.Set16bit( KFourteen, 0x3344 );
    msg.Set16bit( KSixteen, 0x1144 );
    msg.Set16bit( KEightteen, 0x3322 );
    
    TBuf8<KTwenty> readyMsg = msg.Complete();

    if( testMsg[ KEightteen ] != 0x33 || testMsg[ KNineteen ] != 0x22 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg[ KSixteen ] != 0x11 || testMsg[ KSeventeen ] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    
    if( testMsg[ KFourteen ] != 0x33 || testMsg[ KFifteen ] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    
    if( testMsg[ KTwelve ] != 0x11 || testMsg[ KThirteen ] != 0x33 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg[ KTen ] != 0x11 || testMsg[ KEleven ] != 0x22 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }

    TBuf8<KTwenty> testMsg2;
    TIsiSend msg2( testMsg2, KNineteen );

    msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
    msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
    msg2.Set16bit( KTen, 0x1122 );
    msg2.Set16bit( KTwelve, 0x1133 );
    msg2.Set16bit( KFourteen, 0x3344 );
    msg2.Set16bit( KSixteen, 0x1144 );
    
    HBufC8* readyMsg2 = msg2.Complete().AllocLC();

    if( testMsg2[ KSixteen ] != 0x11 || testMsg2[ KSeventeen ] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg2[ KFourteen ] != 0x33 || testMsg2[ KFifteen ] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg2[ KTwelve ] != 0x11 || testMsg2[ KThirteen ] != 0x33 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }

    if( testMsg2[ KTen ] != 0x11 || testMsg2[ KEleven ] != 0x22 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

    TBuf8<KTwenty> testMsg3;
    TIsiSend msg3( testMsg3, KEightteen );

    msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
    msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
    msg3.Set16bit( KTen, 0x1122 );
    msg3.Set16bit( KTwelve, 0x1133 );
    msg3.Set16bit( KFourteen, 0x3344 );
    msg3.Set16bit( KSixteen, 0x1144 );
    
    TBuf8<KTwenty> readyMsg3 = msg3.Complete();

    if( testMsg3[ KSixteen ] != 0x11 || testMsg3[ KSeventeen ] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg3[ KFourteen ] != 0x33 || testMsg3[ KFifteen ] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg3[ KTwelve ] != 0x11 || testMsg3[ KThirteen ] != 0x33 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg3[ KTen ] != 0x11 || testMsg3[ KEleven ] != 0x22 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
    if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase10L
//
// MOD_0000.10 ISIMESSAGE [TIsiSend::Set16bit Negative offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase10L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set16Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTwenty );

        // Try negative offset
        msg.Set16bit( KOffsetMinusOne, 0x0011 ); // =>PANIC :)

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase11L
//
// MOD_0000.11 ISIMESSAGE [TIsiSend::Set16bit Overflow offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase11L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set16Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
    TIsiSend msg( testMsg, KTwenty );

    // Try overflow offset
     msg.Set16bit( KTwenty, 0x0011 );
   
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase12L
//
// MOD_0000.12 ISIMESSAGE [TIsiSend::Set32Bit Last offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase12L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set32Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KSixteen );

        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.Set32bit( 12, 0x11223344 );
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[ KTwelve ] != 0x11 || testMsg[ KThirteen ] != 0x22 ||
            testMsg[ KFourteen ] != 0x33 || testMsg[ KFifteen ] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KSixteen );
        
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg2.Set32bit( KTwelve, 0x12233445 );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

        if( testMsg2[ KTwelve ] != 0x12 || testMsg2[KThirteen] != 0x23 ||
            testMsg2[KFourteen] != 0x34 || testMsg2[ KFifteen ] != 0x45 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        
        if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KSixteen );
        
        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, 0x01 );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, 0x02 );
        msg3.Set32bit( KTwelve, 0x13243546 );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

       if( testMsg3[ KTwelve ] != 0x13 || testMsg3[KThirteen] != 0x24 ||
            testMsg3[KFourteen] != 0x35 || testMsg3[ KFifteen ] != 0x46 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
       if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase13L
//
// MOD_0000.13 ISIMESSAGE [TIsiSend::Set32Bit First offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase13L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set32Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTen );

        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.Set32bit( 0, 0x11223344 );
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[KZero] != 0x11 || testMsg[KOne] != 0x22 ||
            testMsg[KTwo] != 0x33 || testMsg[KThree] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KNineteen );
        
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg2.Set32bit( 0, 0x12233445 );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

        if( testMsg2[KZero] != 0x12 || testMsg2[KOne] != 0x23 ||
            testMsg2[KTwo] != 0x34 || testMsg2[KThree] != 0x45 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KTen );
        
        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, 0x01 );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, MTC_STATE_QUERY_REQ );
        msg3.Set32bit( 0, 0x13243546 );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[KZero] != 0x13 || testMsg3[KOne] != 0x24 ||
            testMsg3[KTwo] != 0x35 || testMsg3[KThree] != 0x46 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }



    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase14L
//
// MOD_0000.14 ISIMESSAGE [TIsiSend::Set32Bit Msg header ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase14L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set32Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTwelve );

        msg.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x01020304 );
        msg.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x05060708);

        TBuf8<KTwenty> readyMsg = msg.Complete();

            if( testMsg[ KZero ] != 0x01 || testMsg[ KOne ] != 0x02 ||
            testMsg[ KTwo ] != 0x03 || testMsg[ KThree ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }   
            else if ( testMsg[ KEight ] != 0x05 || testMsg[ KNine ] != 0x06 ||
            testMsg[ KTen ] != 0x07 || testMsg[ KEleven ] != 0x08 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }                
            else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }         

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KTwelve );

        msg2.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x09080304 );
        msg2.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x05060107);

        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

            if ( testMsg2[KZero] != 0x09 || testMsg2[KOne] != 0x08 ||
            testMsg2[KTwo] != 0x03 || testMsg2[KThree] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }    
            else if ( testMsg2[KEight] != 0x05 || testMsg2[KNine] != 0x06 ||
            testMsg2[ KTen] != 0x01 || testMsg2[KEleven] != 0x07 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }            
            else if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }        

        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KTwelve );

        msg3.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x01020304 );
        msg3.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x05060708);

        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

            if ( testMsg3[KZero] != 0x01 || testMsg3[KOne] != 0x02 ||
            testMsg3[KTwo] != 0x03 || testMsg3[KThree] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }  
            if ( testMsg3[KEight] != 0x05 || testMsg3[KNine] != 0x06 ||
            testMsg3[ KTen] != 0x07 || testMsg3[KEleven] != 0x08 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
                        
            else if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }          

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase15L
//
// MOD_0000.15 ISIMESSAGE [TIsiSend::Set32Bit Middle offsets]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase15L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {
    //
    //  Set32Bit with %2, %4 and %any and values and L == MaxL
    //  0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
    TIsiSend msg( testMsg, KTwenty );

    msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );

    msg.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x11223344 );
    msg.Set32bit( KTwelve, 0x22334455 );
    msg.Set32bit( KSixteen, 0x33445566 );
    TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[KEight] != 0x11 || testMsg[KNine] != 0x22 ||
            testMsg[ KTen] != 0x33 || testMsg[KEleven] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg[KTwelve] != 0x22 || testMsg[KThirteen] != 0x33 ||
            testMsg[KFourteen] != 0x44 || testMsg[ KFifteen ] != 0x55 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg[KSixteen] != 0x33 || testMsg[ KSeventeen ] != 0x44 ||
            testMsg[ KEightteen ] != 0x55 || testMsg[ KNineteen ] != 0x66 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KNineteen );
        
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );

        msg2.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x11223344 );
        msg2.Set32bit( KTwelve, 0x22334455 );

        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

        if( testMsg2[KEight] != 0x11 || testMsg2[KNine] != 0x22 ||
            testMsg2[ KTen] != 0x33 || testMsg2[KEleven] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg2[KTwelve] != 0x22 || testMsg2[KThirteen] != 0x33 ||
            testMsg2[KFourteen] != 0x44 || testMsg2[ KFifteen ] != 0x55 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }        
        if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KEightteen );
        
        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );

        msg3.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x11223344 );
        msg3.Set32bit( KTwelve, 0x22334455 );

        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[KEight] != 0x11 || testMsg3[KNine] != 0x22 ||
            testMsg3[ KTen] != 0x33 || testMsg3[KEleven] != 0x44 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg3[KTwelve] != 0x22 || testMsg3[KThirteen] != 0x33 ||
            testMsg3[KFourteen] != 0x44 || testMsg3[ KFifteen ] != 0x55 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
        if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase16L
//
// MOD_0000.16 ISIMESSAGE [TIsiSend::Set32bit Negative offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase16L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set32Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTwenty );

        // Try negative offset
        msg.Set32bit( KOffsetMinusOne, 0x001122 );
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase17L
//
// MOD_0000.17 ISIMESSAGE [TIsi::Set32bit Overflow offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase17L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set32Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTwenty );
        
        // Try overflow offset
        msg.Set32bit( KTwenty, 0x001122 );
   
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase18L
//
// MOD_0000.18 ISIMESSAGE [TIsiSend::CopyData]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase18L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    CopyData with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<8> temp;
    temp.Append( 0x11 );
    temp.Append( 0x12 );
    temp.Append( 0x13 );
    temp.Append( 0x14 );
    temp.Append( 0x15 );
    temp.Append( 0x16 );
    temp.Append( 0x17 );
    temp.Append( 0x18 );

        TBuf8<20> testMsg;
        TIsiSend msg( testMsg, 18 );

        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg.CopyData( 10, temp );
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[ KTen] != 0x11 || testMsg[KEleven] != 0x12 ||
            testMsg[KTwelve] != 0x13 || testMsg[KThirteen] != 0x14 ||
            testMsg[KFourteen] != 0x15 || testMsg[ KFifteen ] != 0x16 ||
            testMsg[KSixteen] != 0x17 || testMsg[ KSeventeen ] != 0x18
           )
            {
            testBase->PrintCaseProgress( KErrNotFound, KCopyDataError );
            }
        if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KCopyDataError );
            }

        TBuf8<8> temp2;
        temp2.Append( 0x21 );
        temp2.Append( 0x22 );
        temp2.Append( 0x23 );
        temp2.Append( 0x24 );
        temp2.Append( 0x25 );
        temp2.Append( 0x26 );
        temp2.Append( 0x27 );
        temp2.Append( 0x28 );

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2, KNineteen );
        
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg2.CopyData( 10, temp2 );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

        if( testMsg2[ KTen] != 0x21 || testMsg2[KEleven] != 0x22 ||
            testMsg2[KTwelve] != 0x23 || testMsg2[KThirteen] != 0x24 ||
            testMsg2[KFourteen] != 0x25 || testMsg2[ KFifteen ] != 0x26 ||
            testMsg2[KSixteen] != 0x27 || testMsg2[ KSeventeen ] != 0x28 
           )
            {
            testBase->PrintCaseProgress( KErrNotFound, KCopyDataError );
            }
        if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<8> temp3;
        temp3.Append( 0x31 );
        temp3.Append( 0x32 );
        temp3.Append( 0x33 );
        temp3.Append( 0x34 );
        temp3.Append( 0x35 );
        temp3.Append( 0x36 );
        temp3.Append( 0x37 );
        temp3.Append( 0x38 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3, KEightteen );
        
        msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ );
        msg3.CopyData( 10, temp3 );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[ KTen] != 0x31 || testMsg3[KEleven] != 0x32 ||
            testMsg3[KTwelve] != 0x33 || testMsg3[KThirteen] != 0x34 ||
            testMsg3[KFourteen] != 0x35 || testMsg3[ KFifteen ] != 0x36 ||
            testMsg3[KSixteen] != 0x37 || testMsg3[ KSeventeen ] != 0x38
           )
            {
            testBase->PrintCaseProgress( KErrNotFound, KCopyDataError );
            }
        if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KCopyDataError );
            }

        

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase19L
//
// MOD_0001.00 ISIMESSAGE [TIsiReceiveC::Get8Bit Last offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase19L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );

        TUint8 byteNine = msg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );
        TUint8 byteEightteen  = msg.Get8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL9 );
        TUint8 byteNineteen = msg.Get8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL10 );

        if( testMsg[ KNine ] != byteNine || testMsg[ KEightteen  ] != byteEightteen  || 
            testMsg[ KNineteen ] != byteNineteen )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );
        TIsiReceiveC msg2( *testMsg2And3 );

        TUint8 byteNine2 = msg2.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );
        TUint8 byteEightteen2 = msg2.Get8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL9 );

        if( testMsgPtr[ KNine ] != byteNine2 || testMsgPtr[ KEightteen ] != byteEightteen2  )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );

        TUint8 byteNine3 = msg3.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );
        TUint8 byteSeventeen3 = msg3.Get8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL8 );


        if( testMsgPtr[ KNine ] != byteNine3 || testMsgPtr[ KSeventeen ] != byteSeventeen3  )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase20L
//
// MOD_0001.01 ISIMESSAGE [TIsiReceiveC::Get8Bit First offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase20L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );

        TUint8 byteZero = msg.Get8bit( KZero );
        TUint8 byteNine = msg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );

        if( testMsg[ KZero ] != byteZero || testMsg[ KNine ] != byteNine )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );
        TIsiReceiveC msg2( *testMsg2And3 );

        TUint8 byteZero2 = msg2.Get8bit( KZero );
        TUint8 byteNine2 = msg2.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );

        if( testMsgPtr[ KZero ] != byteZero2 || testMsgPtr[ KNine ] != byteNine2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );

        TUint8 byteZero3 = msg3.Get8bit( KZero );
        TUint8 byteNine3 = msg3.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );

        if( testMsgPtr[ KZero ] != byteZero3 || testMsgPtr[ KNine ] != byteNine3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase21L
//
// MOD_0001.02 ISIMESSAGE [TIsiReceiveC::Get8Bit Msg header ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase21L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );

        TUint8 byteZero = msg.Get8bit( ISI_HEADER_OFFSET_MEDIA );
        TUint8 byteOne = msg.Get8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE );
        TUint8 byteTwo = msg.Get8bit( ISI_HEADER_OFFSET_SENDERDEVICE );
        TUint8 byteThree = msg.Get8bit( ISI_HEADER_OFFSET_RESOURCEID );
        TUint8 byteFour = msg.Get8bit( ISI_HEADER_OFFSET_LENGTH );
        TUint8 byteFive = msg.Get8bit( KFive );
        TUint8 byteSix = msg.Get8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT );
        TUint8 byteSeven = msg.Get8bit( ISI_HEADER_OFFSET_SENDEROBJECT );
        TUint8 byteEight = msg.Get8bit( ISI_HEADER_OFFSET_TRANSID );
        TUint8 byteNine = msg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );

        if( testMsg[ KZero ] != byteZero || testMsg[ KOne ] != byteOne || 
            testMsg[ KTwo ] != byteTwo || testMsg[ KThree ] != byteThree || 
            testMsg[ KFour ] != byteFour || testMsg[ KFive] != byteFive || 
            testMsg[ KSix ] != byteSix || testMsg[ KSeven ] != byteSeven || 
            testMsg[ KEight ] != byteEight || testMsg[ KNine ] != byteNine )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );
        TIsiReceiveC msg2( *testMsg2And3 );

        TUint8 byteZero2 = msg2.Get8bit( ISI_HEADER_OFFSET_MEDIA );
        TUint8 byteOne2 = msg2.Get8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE );
        TUint8 byteTwo2 = msg2.Get8bit( ISI_HEADER_OFFSET_SENDERDEVICE );
        TUint8 byteThree2 = msg2.Get8bit( ISI_HEADER_OFFSET_RESOURCEID );
        TUint8 byteFour2 = msg2.Get8bit( ISI_HEADER_OFFSET_LENGTH );
        TUint8 byteFive2 = msg2.Get8bit( KFive );
        TUint8 byteSix2 = msg2.Get8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT );
        TUint8 byteSeven2 = msg2.Get8bit( ISI_HEADER_OFFSET_SENDEROBJECT );
        TUint8 byteEight2 = msg2.Get8bit( ISI_HEADER_OFFSET_TRANSID );
        TUint8 byteNine2 = msg2.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );

        if( testMsgPtr[ KZero ] != byteZero2 || testMsgPtr[ KOne ] != byteOne2 || 
            testMsgPtr[ KTwo ] != byteTwo2 || testMsgPtr[ KThree ] != byteThree2 || 
            testMsgPtr[ KFour ] != byteFour2 || testMsgPtr[ KFive] != byteFive2 || 
            testMsgPtr[ KSix ] != byteSix2 || testMsgPtr[ KSeven ] != byteSeven2 || 
            testMsgPtr[ KEight ] != byteEight2 || testMsgPtr[ KNine ] != byteNine2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );

        TUint8 byteZero3 = msg3.Get8bit( ISI_HEADER_OFFSET_MEDIA );
        TUint8 byteOne3 = msg3.Get8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE );
        TUint8 byteTwo3 = msg3.Get8bit( ISI_HEADER_OFFSET_SENDERDEVICE );
        TUint8 byteThree3 = msg3.Get8bit( ISI_HEADER_OFFSET_RESOURCEID );
        TUint8 byteFour3 = msg3.Get8bit( ISI_HEADER_OFFSET_LENGTH );
        TUint8 byteFive3 = msg3.Get8bit( KFive );
        TUint8 byteSix3 = msg3.Get8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT );
        TUint8 byteSeven3 = msg3.Get8bit( ISI_HEADER_OFFSET_SENDEROBJECT );
        TUint8 byteEight3 = msg3.Get8bit( ISI_HEADER_OFFSET_TRANSID );
        TUint8 byteNine3 = msg3.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );

        if( testMsgPtr[ KZero ] != byteZero3 || testMsgPtr[ KOne ] != byteOne3 || 
            testMsgPtr[ KTwo ] != byteTwo3 || testMsgPtr[ KThree ] != byteThree3 || 
            testMsgPtr[ KFour ] != byteFour3 || testMsgPtr[ KFive] != byteFive3 || 
            testMsgPtr[ KSix ] != byteSix3 || testMsgPtr[ KSeven ] != byteSeven3 || 
            testMsgPtr[ KEight ] != byteEight3 || testMsgPtr[ KNine ] != byteNine3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase22L
//
// MOD_0001.03 ISIMESSAGE [TIsiReceiveC::Get8Bit Middle offsets]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase22L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );

        TUint8 byteTen = msg.Get8bit( KTen);
        TUint8 byteEleven = msg.Get8bit(KEleven);
        TUint8 byteTwelve = msg.Get8bit(KTwelve);
        TUint8 byteThirteen = msg.Get8bit(KThirteen);
        TUint8 byteFourteen = msg.Get8bit(KFourteen);
        TUint8 byteFifteen = msg.Get8bit( KFifteen );
        TUint8 byteSixteen = msg.Get8bit(KSixteen);
        TUint8 byteSeventeen = msg.Get8bit( KSeventeen );
        TUint8 byteEighteen = msg.Get8bit( KEightteen );
        TUint8 byteNineteen = msg.Get8bit( KNineteen );

        if( testMsg[ KTen ] != byteTen || testMsg[ KEleven ] != byteEleven || 
            testMsg[ KTwelve ] != byteTwelve || testMsg[ KThirteen ] != byteThirteen || 
            testMsg[ KFourteen ] != byteFourteen || testMsg[ KFifteen] != byteFifteen || 
            testMsg[ KSixteen ] != byteSixteen || testMsg[ KSeventeen ] != byteSeventeen || 
            testMsg[ KEightteen ] != byteEighteen || testMsg[ KNineteen ] != byteNineteen )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );
        TIsiReceiveC msg2( *testMsg2And3 );

        TUint8 byteTen2 = msg2.Get8bit( KTen);
        TUint8 byteEleven2 = msg2.Get8bit(KEleven);
        TUint8 byteTwelve2 = msg2.Get8bit(KTwelve);
        TUint8 byteThirteen2 = msg2.Get8bit(KThirteen);
        TUint8 byteFourteen2 = msg2.Get8bit(KFourteen);
        TUint8 byteFifteen2 = msg2.Get8bit( KFifteen );
        TUint8 byteSixteen2 = msg2.Get8bit(KSixteen);
        TUint8 byteSeventeen2 = msg2.Get8bit( KSeventeen );
        TUint8 byteEighteen2 = msg2.Get8bit( KEightteen );

        if( testMsgPtr[ KTen ] != byteTen2 || testMsgPtr[ KEleven ] != byteEleven2 || 
            testMsgPtr[ KTwelve ] != byteTwelve2 || testMsgPtr[ KThirteen ] != byteThirteen2 || 
            testMsgPtr[ KFourteen ] != byteFourteen2 || testMsgPtr[ KFifteen] != byteFifteen2 || 
            testMsgPtr[ KSixteen ] != byteSixteen2 || testMsgPtr[ KSeventeen ] != byteSeventeen2 || 
            testMsgPtr[ KEightteen ] != byteEighteen2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );
    
        TUint8 byteTen3 = msg3.Get8bit( KTen);
        TUint8 byteEleven3 = msg3.Get8bit(KEleven);
        TUint8 byteTwelve3 = msg3.Get8bit(KTwelve);
        TUint8 byteThirteen3 = msg3.Get8bit(KThirteen);
        TUint8 byteFourteen3 = msg3.Get8bit(KFourteen);
        TUint8 byteFifteen3 = msg3.Get8bit( KFifteen );
        TUint8 byteSixteen3 = msg3.Get8bit(KSixteen);
        TUint8 byteSeventeen3 = msg3.Get8bit( KSeventeen );

        if( testMsgPtr[ KTen ] != byteTen3 || testMsgPtr[ KEleven ] != byteEleven3 || 
            testMsgPtr[ KTwelve ] != byteTwelve3 || testMsgPtr[ KThirteen ] != byteThirteen3 || 
            testMsgPtr[ KFourteen ] != byteFourteen3 || testMsgPtr[ KFifteen] != byteFifteen3 || 
            testMsgPtr[ KSixteen ] != byteSixteen3 || testMsgPtr[ KSeventeen ] != byteSeventeen3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase23L
//
// MOD_0001.04 ISIMESSAGE [TIsiReceiveC::Get8bit Negative offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase23L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<20> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
                
        // Get negative offset
        TUint8 byteMinus = msg.Get8bit( KOffsetMinusOne );
        if( testMsg[ KZero ] != byteMinus) //Won't reach this point
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase24L
//
// MOD_0001.05 ISIMESSAGE [TIsiReceiveC::Get8bit Overflow offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase24L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<20> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
        
        // Get overflow offset
        TUint8 byteOverFlow = msg.Get8bit( KTwentyOne );
        if( testMsg[ KZero ] != byteOverFlow) //shouldn't reach this point
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet8BitError );
            }
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase25L
//
// MOD_0001.06 ISIMESSAGE [TIsiReceiveC::Get16Bit Last offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase25L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );

        TUint16 byteEighteen = msg.Get16bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL9 );
        TUint16 byteEighteenComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KEightteen );
        if( byteEighteenComp != byteEighteen )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );

        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );

        TUint16 byteSixteen2 = msg2.Get16bit( KSixteen );
        TUint16 byteSixteenComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KSixteen );
        if( byteSixteenComp2 != byteSixteen2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg3( *testMsg2And3 );
        
        TUint16 byteSixteen3 = msg3.Get16bit( KSixteen );
        TUint16 byteSixteenComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KSixteen );
        if( byteSixteenComp3 != byteSixteen3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase26L
//
// MOD_0001.07 ISIMESSAGE [TIsiReceiveC::Get16Bit First offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase26L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {
    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
        
        TUint16 byteZero = msg.Get16bit( ISI_HEADER_OFFSET_MEDIA  );
        TUint16 byteZeroComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KZero );
        if( byteZeroComp != byteZero )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );

        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );
        
        TUint16 byteZero2 = msg2.Get16bit( KZero );
        TUint16 byteZeroComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KZero );
        if( byteZeroComp2 != byteZero2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );
        
        TUint16 byteZero3 = msg3.Get16bit( KZero );
        TUint16 byteZeroComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KZero );
        if( byteZeroComp3 != byteZero3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase27L
//
// MOD_0001.08 ISIMESSAGE [TIsiReceiveC::Get16Bit Msg header ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase27L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );
        
        TIsiReceiveC msg( testMsg );

        TUint16 byteZero = msg.Get16bit( ISI_HEADER_OFFSET_MEDIA );
        TUint16 byteTwo = msg.Get16bit( ISI_HEADER_OFFSET_SENDERDEVICE );
        TUint16 byteFour = msg.Get16bit( ISI_HEADER_OFFSET_LENGTH );
        TUint16 byteSix = msg.Get16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT );
        TUint16 byteEight = msg.Get16bit( ISI_HEADER_OFFSET_TRANSID );
        
        TUint16 byteZeroComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KZero );
        TUint16 byteTwoComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KTwo );
        TUint16 byteFourComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KFour );
        TUint16 byteSixComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KSix );
        TUint16 byteEightComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KEight );

        if( byteZeroComp != byteZero || byteTwoComp != byteTwo ||
            byteFourComp != byteFour || byteSixComp != byteSix ||
            byteEightComp != byteEight )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );

        TUint16 byteZero2 = msg2.Get16bit( ISI_HEADER_OFFSET_MEDIA );
        TUint16 byteTwo2 = msg2.Get16bit( ISI_HEADER_OFFSET_SENDERDEVICE );
        TUint16 byteFour2 = msg2.Get16bit( ISI_HEADER_OFFSET_LENGTH );
        TUint16 byteSix2 = msg2.Get16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT );
        TUint16 byteEight2 = msg2.Get16bit( ISI_HEADER_OFFSET_TRANSID );

        TUint16 byteZeroComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KZero );
        TUint16 byteTwoComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KTwo );
        TUint16 byteFourComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KFour );
        TUint16 byteSixComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KSix );
        TUint16 byteEightComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KEight );

        if( byteZeroComp2 != byteZero2 || byteTwoComp2 != byteTwo2 ||
            byteFourComp2 != byteFour2 || byteSixComp2 != byteSix2 ||
            byteEightComp2 != byteEight2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );
        
        TUint16 byteZero3 = msg3.Get16bit( ISI_HEADER_OFFSET_MEDIA );
        TUint16 byteTwo3 = msg3.Get16bit( ISI_HEADER_OFFSET_SENDERDEVICE );
        TUint16 byteFour3 = msg3.Get16bit( ISI_HEADER_OFFSET_LENGTH );
        TUint16 byteSix3 = msg3.Get16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT );
        TUint16 byteEight3 = msg3.Get16bit( ISI_HEADER_OFFSET_TRANSID );

        TUint16 byteZeroComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KZero );
        TUint16 byteTwoComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KTwo );
        TUint16 byteFourComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KFour );
        TUint16 byteSixComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KSix );
        TUint16 byteEightComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KEight );

        if( byteZeroComp3 != byteZero3 || byteTwoComp3 != byteTwo3 ||
            byteFourComp3 != byteFour3 || byteSixComp3 != byteSix3 ||
            byteEightComp3 != byteEight3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase28L
//
// MOD_0001.09 ISIMESSAGE [TIsiReceiveC::Get16Bit Middle offsets]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase28L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );
        
        TIsiReceiveC msg( testMsg );

        TUint16 byteTen = msg.Get16bit( KTen);
        TUint16 byteTwelve = msg.Get16bit(KTwelve);
        TUint16 byteFourteen = msg.Get16bit(KFourteen);
        TUint16 byteSixteen = msg.Get16bit(KSixteen);
        TUint16 byteEighteen = msg.Get16bit( KEightteen );
        
        TUint16 byteTenComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KTen );
        TUint16 byteTwelveComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KTwelve );
        TUint16 byteFourteenComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KFourteen );
        TUint16 byteSixteenComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KSixteen );
        TUint16 byteEighteenComp = testBase->Get16BitFromTestMessage( testMsg.Ptr(), KEightteen );

        if( byteTenComp != byteTen || byteTwelveComp != byteTwelve ||
            byteFourteenComp != byteFourteen || byteSixteenComp != byteSixteen ||
            byteEighteenComp != byteEighteen )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );
        
        TUint16 byteTen2 = msg2.Get16bit( KTen);
        TUint16 byteTwelve2 = msg2.Get16bit(KTwelve);
        TUint16 byteFourteen2 = msg2.Get16bit(KFourteen);
        TUint16 byteSixteen2 = msg2.Get16bit(KSixteen);

        TUint16 byteTenComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KTen );
        TUint16 byteTwelveComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KTwelve );
        TUint16 byteFourteenComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KFourteen );
        TUint16 byteSixteenComp2 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KSixteen );

        if( byteTenComp2 != byteTen2 || byteTwelveComp2 != byteTwelve2 ||
            byteFourteenComp2 != byteFourteen2 || byteSixteenComp2 != byteSixteen2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );
        
        TUint16 byteTen3 = msg3.Get16bit( KTen);
        TUint16 byteTwelve3 = msg3.Get16bit(KTwelve);
        TUint16 byteFourteen3 = msg3.Get16bit(KFourteen);
        TUint16 byteSixteen3 = msg3.Get16bit(KSixteen);

        TUint16 byteTenComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KTen );
        TUint16 byteTwelveComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KTwelve );
        TUint16 byteFourteenComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KFourteen );
        TUint16 byteSixteenComp3 = testBase->Get16BitFromTestMessage( testMsgPtr.Ptr(), KSixteen );

        if( byteTenComp3 != byteTen3 || byteTwelveComp3 != byteTwelve3 ||
            byteFourteenComp3 != byteFourteen3 || byteSixteenComp3 != byteSixteen3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );



    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase29L
//
// MOD_0001.10 ISIMESSAGE [TIsiReceiveC::Get16bit Negative offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase29L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
        
        // Get negative offset
        TUint8 byteMinus = msg.Get16bit( KOffsetMinusOne );
        if( testMsg[ KZero ] != byteMinus) //Won't reach this point
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase30L
//
// MOD_0001.11 ISIMESSAGE [TIsiReceiveC::Get16bit Overflow offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase30L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
        
        // Get overflow offset
        TUint8 byteOverFlow = msg.Get16bit( KTwenty );
        if( testMsg[ KZero ] != byteOverFlow) //Won't reach this point
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet16BitError );
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase31L
//
// MOD_0001.12 ISIMESSAGE [TIsiReceiveC::Get32Bit Last offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase31L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );

        TUint32 byteSixteen = msg.Get32bit( KSixteen );

        TUint32 byteSixteenComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KSixteen );
        if( byteSixteenComp != byteSixteen )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );

        TUint32 byteTwelve2 = msg2.Get32bit( KTwelve );
        TUint32 byteTwelveComp2 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KTwelve );
        
        if( byteTwelveComp2 != byteTwelve2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );
        
        TUint32 byteTwelve3 = msg3.Get32bit( KTwelve );
        TUint32 byteTwelveComp3 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KTwelve );

        if( byteTwelveComp3 != byteTwelve3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase32L
//
// MOD_0001.13 ISIMESSAGE [TIsiReceiveC::Get32Bit First offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase32L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
        
        TUint32 byteZero = msg.Get32bit( KZero );
        TUint32 byteZeroComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KZero );
        if( byteZeroComp != byteZero )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x91 );
        testMsgPtr.Append( 0x87 );
        testMsgPtr.Append( 0x76 );
        testMsgPtr.Append( 0x54 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );
        
        TUint32 byteZero2 = msg2.Get32bit( KZero );
        TUint32 byteZeroComp2 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KZero );
        
        if( byteZeroComp2 != byteZero2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );

        TUint32 byteZero3 = msg3.Get32bit( KZero );
        TUint32 byteZeroComp3 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KZero );

        if( byteZeroComp3 != byteZero3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase33L
//
// MOD_0001.14 ISIMESSAGE [TIsiReceiveC::Get32Bit Msg header ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase33L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get32Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );
        
        TIsiReceiveC msg( testMsg );

        TUint32 byteZero = msg.Get32bit( ISI_HEADER_OFFSET_MEDIA );
        TUint32 byteFour = msg.Get32bit( ISI_HEADER_OFFSET_LENGTH );
        TUint32 byteEight = msg.Get32bit( ISI_HEADER_OFFSET_TRANSID );
        
        TUint32 byteZeroComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KZero );
        TUint32 byteFourComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KFour );
        TUint32 byteEightComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KEight );

        if( byteZeroComp != byteZero || byteFourComp != byteFour || 
            byteEightComp != byteEight )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );
        
        TUint32 byteZero2 = msg2.Get32bit( ISI_HEADER_OFFSET_MEDIA );
        TUint32 byteFour2 = msg2.Get32bit( ISI_HEADER_OFFSET_LENGTH );
        TUint32 byteEight2 = msg2.Get32bit( ISI_HEADER_OFFSET_TRANSID );

        TUint32 byteZeroComp2 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KZero );
        TUint32 byteFourComp2 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KFour );
        TUint32 byteEightComp2 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KEight );

        if( byteZeroComp2 != byteZero2 || byteFourComp2 != byteFour2 ||
            byteEightComp2 != byteEight2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );
        
        TUint32 byteZero3 = msg3.Get32bit( ISI_HEADER_OFFSET_MEDIA );
        TUint32 byteFour3 = msg3.Get32bit( ISI_HEADER_OFFSET_LENGTH );
        TUint32 byteEight3 = msg3.Get32bit( ISI_HEADER_OFFSET_TRANSID );

        TUint32 byteZeroComp3 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KZero );
        TUint32 byteFourComp3 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KFour );
        TUint32 byteEightComp3 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KEight );

        if( byteZeroComp3 != byteZero3 || byteFourComp3 != byteFour3 || 
            byteEightComp3 != byteEight3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase34L
//
// MOD_0001.15 ISIMESSAGE [TIsiReceiveC::Get32Bit Middle offsets]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase34L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get32Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );
        
        TIsiReceiveC msg( testMsg );

        TUint32 byteEight = msg.Get32bit(KEight);
        TUint32 byteTwelve = msg.Get32bit(KTwelve);
        TUint32 byteSixteen = msg.Get32bit(KSixteen);
        
        TUint32 byteEightComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KEight );
        TUint32 byteTwelveComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KTwelve );
        TUint32 byteSixteenComp = testBase->Get32BitFromTestMessage( testMsg.Ptr(), KSixteen );

        if( byteEight != byteEightComp ||byteTwelveComp != byteTwelve 
            || byteSixteenComp != byteSixteen )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );
        testMsgPtr.Append( 0x13 );

        TIsiReceiveC msg2( *testMsg2And3 );
        
        TUint32 byteEight2 = msg2.Get32bit(KEight);
        TUint32 byteTwelve2 = msg2.Get32bit(KTwelve);

        TUint32 byteEightComp2 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KEight );
        TUint32 byteTwelveComp2 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KTwelve );
        
        if( byteEightComp2 != byteEight2 || byteTwelveComp2 != byteTwelve2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg3( *testMsg2And3 );
        
        TUint32 byteEight3 = msg3.Get32bit(KEight);
        TUint32 byteTwelve3 = msg3.Get32bit(KTwelve);

        TUint32 byteEightComp3 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KEight );
        TUint32 byteTwelveComp3 = testBase->Get32BitFromTestMessage( testMsgPtr.Ptr(), KTwelve );

        if( byteEightComp3 != byteEight3 || byteTwelveComp3 != byteTwelve3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase35L
//
// MOD_0001.16 ISIMESSAGE [TIsiReceiveC::Get32bit Negative offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase35L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get32Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
        
        // Get negative offset
        TUint8 byteMinus = msg.Get32bit( KOffsetMinusOne );
        if( testMsg[ KZero ] != byteMinus) //Won't reach this point
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase36L
//
// MOD_0001.17 ISIMESSAGE [TIsiReceiveC::Get32bit Overflow offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase36L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get32Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
        
        // Get overflow offset
        TUint8 byteOverFlow = msg.Get32bit( KTwenty );
        if( testMsg[ KZero ] != byteOverFlow) //Won't reach this point
            {
            testBase->PrintCaseProgress( KErrNotFound, KGet32BitError );
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase37L
//
// MOD_0001.18 ISIMESSAGE [TIsiReceiveC::GetData]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase37L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );

        TIsiReceiveC msg( testMsg );

        TPtrC8 byteZero = msg.GetData( KZero, KTwenty );
        TPtrC8 byteTwelve = msg.GetData( KTwelve, KEight );

        TPtrC8 byteZeroComp = testMsg.Mid( KZero, KTwenty );
        TPtrC8 byteTenComp = testMsg.Mid( KTwelve, KEight );
        if( byteZeroComp != byteZero ||  byteTenComp != byteTwelve )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGetDataError );
            }

        HBufC8* testMsg2And3 = HBufC8::NewLC( KTwenty );
        TPtr8 testMsgPtr( testMsg2And3->Des() );
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x00 );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );
        testMsgPtr.Append( 0x12 );

        TIsiReceiveC msg2( testMsgPtr );

        TPtrC8 byteZero2 = msg2.GetData( KZero, KNineteen );
        TPtrC8 byteTwelve2 = msg2.GetData( KTwelve, KSeven );

        TPtrC8 byteZeroComp2 = testMsgPtr.Mid( KZero, KNineteen );
        TPtrC8 byteTenComp2 = testMsgPtr.Mid( KTwelve, KSeven );
        if( byteZeroComp2 != byteZero2 ||  byteTenComp2 != byteTwelve2 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGetDataError );
            }

        testMsgPtr.FillZ();
        testMsgPtr.Zero();
        testMsgPtr.Append( 0x01 );
        testMsgPtr.Append( 0x02 );
        testMsgPtr.Append( 0x03 );
        testMsgPtr.Append( 0x04 );
        testMsgPtr.Append( 0x00 );
        testMsgPtr.Append( 0x0b );
        testMsgPtr.Append( 0x05 );
        testMsgPtr.Append( 0x06 );
        testMsgPtr.Append( 0x07 );
        testMsgPtr.Append( 0x08 );
        testMsgPtr.Append( 0x09 );
        testMsgPtr.Append( 0x0a );
        testMsgPtr.Append( 0x0c );
        testMsgPtr.Append( 0x0d );
        testMsgPtr.Append( 0x0e );
        testMsgPtr.Append( 0x0f );
        testMsgPtr.Append( 0x10 );
        testMsgPtr.Append( 0x11 );

        TIsiReceiveC msg3( testMsgPtr );

        TPtrC8 byteZero3 = msg3.GetData( KZero, KEightteen );
        TPtrC8 byteTwelve3 = msg3.GetData( KTwelve, KSix );

        TPtrC8 byteZeroComp3 = testMsgPtr.Mid( KZero, KEightteen );
        TPtrC8 byteTenComp3 = testMsgPtr.Mid( KTwelve, KSix );
        if( byteZeroComp3 != byteZero3 ||  byteTenComp3 != byteTwelve3 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGetDataError );
            }

        CleanupStack::PopAndDestroy( testMsg2And3 );

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

}

// ------------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase38L
//
// MOD_0001.19 ISIMESSAGE [TIsiReceiveC::GetData Negative offset]
// -----------------------------------------------------------------------------

TInt CIsiMsgApiTest::RunClassCase38L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
                
        // Get negative offset
        TPtrC8 byteNegative = msg.GetData( KOffsetMinusOne, KTwenty );
        TPtrC8 byteNegativeComp = testMsg.Mid( KZero, KTwenty ); //should not reach this line
        if( byteNegative != byteNegativeComp )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGetDataError );
            }
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase39L
//
// MOD_0001.20 ISIMESSAGE [TIsiReceiveC::GetData Overflow offset]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase39L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get16Bit with %2, %4 and %any and values and L == MaxL
    //    OFFSET < 0 & length < OFFSET
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiReceiveC msg( testMsg );
            
        // Get overflow offset
        TPtrC8 byteOverflow = msg.GetData( KTwenty, KTwenty );
        TPtrC8  byteOverflowComp = testMsg.Mid( KNineteen, KTwenty ); //should not reach this line
        if( byteOverflow != byteOverflowComp )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGetDataError );
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase96L
//
// MOD_0001.21 ISIMESSAGE [TIsiReceiveC::GetData with length = 0]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase96L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Get8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );

        TIsiReceiveC msg( testMsg );
        TPtrC8 byteZero = msg.GetData( KZero, KZero );
        TPtrC8 byteTwelve = msg.GetData( KTwelve, KZero );

        TPtrC8 byteZeroComp = testMsg.Mid( KZero, KTwenty );
        TPtrC8 byteTenComp = testMsg.Mid( KTwelve, KEight );

    //compared bytes can't be same, check trace that asserts about length = 0
        if( byteZeroComp == byteZero &&  byteTenComp == byteTwelve )
            {
            testBase->PrintCaseProgress( KErrNotFound, KGetDataError );
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

}

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase40L
//
// MOD_0002.00 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase40L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  One subblock per message.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<16> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x08 ); //len 4bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );

        //testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp = msg.FindSubBlockOffsetById( 0x0c, 0x05, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_8L );
            }

        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset] != 0x05 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
      
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase41L
//
// MOD_0002.01 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase41L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/16 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x05, EIsiSubBlockTypeId8Len16, aSubBlockOffset );

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset] != 0x05 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase42L
//
// MOD_0002.02 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase42L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 16/8 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0201, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_8L );
            }
         else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }            
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase43L
//
// MOD_0002.03 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase43L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 16/16 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0102, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x01 || testMsg[aSubBlockOffset+1] != 0x02 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase44L
//
// MOD_0002.04 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase44L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/32 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x01 ); //data
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 ); //data

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x05, EIsiSubBlockTypeId8Len32, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x05 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase45L
//
// MOD_0002.05 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase45L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  One subblock per message.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x05, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x05 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase46L
//
// MOD_0002.06 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase46L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/16 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x05, EIsiSubBlockTypeId8Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x05 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase47L
//
// MOD_0002.07 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase47L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 16/8 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0201, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_8L );
            }
        else if( temp == KErrNone )
            {
            if (  testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01  ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase48L
//
// MOD_0002.08 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase48L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 16/16 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0102, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x01 || testMsg[aSubBlockOffset+1] != 0x02 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase49L
//
// MOD_0002.09 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase49L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/32 bit.
    //  One subblock per message.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x01 ); //data
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 ); //data

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x05, EIsiSubBlockTypeId8Len32, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x05 ) //aSubBlockOffset should be 12
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase50L
//
// MOD_0002.10 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase50L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L == MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start1 8bit, offset 0x0c = 12
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x02 );

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 16 = 0x10
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x06, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x06 ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase51L
//
// MOD_0002.11 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase51L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L == MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start1 8bit, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x04 ); //len 16bit

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 16 = 0x10
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x06, EIsiSubBlockTypeId8Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x06 )//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase52L
//
// MOD_0002.12 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase52L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L == MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start2 16bit, offset 12 = 0x0c
        testMsg.Append( 0x05 ); //sb_start2
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x01 ); //data

        testMsg.Append( 0x06 ); //sb_start2 16bit, offset 16 = 0x10
        testMsg.Append( 0x07 ); //sb_start2
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0607, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x06 || testMsg[aSubBlockOffset+1] != 0x07 ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase53L
//
// MOD_0002.13 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase53L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L == MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start2 16bit, offset 12 = 0x0c
        testMsg.Append( 0x05 ); //sb_start2
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len 

        testMsg.Append( 0x06 ); //sb_start2 16bit, offset 16 = 0x10
        testMsg.Append( 0x07 ); //sb_start2
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0607, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x06 || testMsg[aSubBlockOffset+1] != 0x07 )//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                //offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase54L
//
// MOD_0002.14 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase54L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L == MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<32> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a);
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start2 8bit, offset 12 = 0x0c
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 20 = 0x14
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0a ); //len 32bit
        testMsg.Append( 0x01 ); //data
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 ); //data

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x06, EIsiSubBlockTypeId8Len32, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x06 ) //aSubBlockOffset should be 20
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }    
    

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase55L
//
// MOD_0002.15 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase55L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start1 8bit, offset 0x0c = 12
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x02 );

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 16 = 0x10
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x06, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x06 ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase56L
//
// MOD_0002.16 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase56L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start1 8bit, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x04 ); //len 16bit

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 16 = 0x10
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x04 ); //len 16bit
        //testMsg.Append( 0x03 ); 
        //testMsg.Append( 0x04 );
        //testMsg.Append( 0x05 ); 
        //testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x06, EIsiSubBlockTypeId8Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x06 ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase57L
//
// MOD_0002.17 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase57L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start2 16bit, offset 12 = 0x0c
        testMsg.Append( 0x05 ); //sb_start2
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x01 ); //data

        testMsg.Append( 0x06 ); //sb_start2 16bit, offset 16 = 0x10
        testMsg.Append( 0x07 ); //sb_start2
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0607, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_8L );
            }
        else if( temp == KErrNone )
            {
            if (  testMsg[aSubBlockOffset] != 0x06 || testMsg[aSubBlockOffset+1] != 0x07 ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase58L
//
// MOD_0002.18 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase58L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start2 16bit, offset 12 = 0x0c
        testMsg.Append( 0x05 ); //sb_start2
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len 

        testMsg.Append( 0x06 ); //sb_start2 16bit, offset 16 = 0x10
        testMsg.Append( 0x07 ); //sb_start2
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x0607, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x06 || testMsg[aSubBlockOffset+1] != 0x07 ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }


    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase59L
//
// MOD_0002.19 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase59L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a);
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start2 8bit, offset 12 = 0x0c
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 20 = 0x14
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len 32bit

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x06, EIsiSubBlockTypeId8Len32,aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset ] != 0x06 ) //aSubBlockOffset should be 20
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
                }
            else
                {
                // offset ok
                }
            }


    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }    

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase99L
//
// MOD_0002.21 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById sb not found]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase99L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a);
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start2 8bit, offset 12 = 0x0c
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 20 = 0x14
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len 32bit

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x07, EIsiSubBlockTypeId8Len32,aSubBlockOffset);

        if( temp != KErrNotFound )
            {
             testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
            }
            
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }    


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase101L
//
// MOD_0002.22 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById sb doesn't exist]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase101L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<12> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a);
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x00 ); //sb_count

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x10, 0x07, EIsiSubBlockTypeId8Len32,aSubBlockOffset);

        if( temp == KErrNone )  //temp should be KErrNotFound
            {
             testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById);
            }
            
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }    


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase60L
//
// MOD_0003.00 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase60L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Two subblocks per message with same sb_id.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        //muutettu 2->0 indexiksi
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KSixteen ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex);
                }
            else
                {
                // offset ok
                }
            }

        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase61L
//
// MOD_0003.01 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase61L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/16 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId8Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KSixteen ) //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase62L
//
// MOD_0003.02 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase62L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 16/8 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 || aSubBlockOffset != KSixteen ) 
                //aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase63L
//
// MOD_0003.03 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase63L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 16/16 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 
            || aSubBlockOffset != KSixteen )//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase64L
//
// MOD_0003.04 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase64L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/32 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<32> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId8Len32, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || aSubBlockOffset != KTwenty )//aSubBlockOffset should be 20
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase65L
//
// MOD_0003.05 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase65L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Two subblocks per message with same sb_id.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KSixteen )//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase66L
//
// MOD_0003.06 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase66L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/16 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId8Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KSixteen)//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase67L
//
// MOD_0003.07 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase67L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 16/8 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 || aSubBlockOffset != KSixteen)//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase68L
//
// MOD_0003.08 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase68L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 16/16 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 
            || aSubBlockOffset != KSixteen )//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase69L
//
// MOD_0003.09 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase69L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/32 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId8Len32, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || aSubBlockOffset != KTwenty )//aSubBlockOffset should be 20
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase70L
//
// MOD_0003.10 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase70L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Three subblocks per message with same sb_id.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<32> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 24
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KTwentyFour )//aSubBlockOffset should be 24
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase71L
//
// MOD_0003.11 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase71L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/16 bit.
    //  Three subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 24
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId8Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KTwentyFour)//aSubBlockOffset should be 24
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase72L
//
// MOD_0003.12 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase72L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 16/8 bit.
    //  Three subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x08 ); //data

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 || aSubBlockOffset != KTwentyFour) //aSubBlockOffset should be 24
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase73L
//
// MOD_0003.13 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase73L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 16/16 bit.
    //  Two subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 
            || aSubBlockOffset != KTwentyFour ) //aSubBlockOffset should be 24
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase74L
//
// MOD_0003.14 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase74L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/32 bit.
    //  Three subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<44> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );

        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 ); //data
        testMsg.Append( 0x06 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId8Len32, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || aSubBlockOffset != KThirtyTwo )//aSubBlockOffset should be 32
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase75L
//
// MOD_0003.15 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase75L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Three subblocks per message with same sb_id.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 24
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId8Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KTwentyFour )//aSubBlockOffset should be 24
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase76L
//
// MOD_0003.16 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN16 ]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase76L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/16 bit.
    //  Three subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId8Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != KTwentyFour)//aSubBlockOffset should be 24
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase77L
//
// MOD_0003.17 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN8]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase77L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 16/8 bit.
    //  Three subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x08 ); //data


        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x08 ); //data        
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );


        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId16Len8, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_8L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 
            || aSubBlockOffset != 16)//aSubBlockOffset should be 16
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }
    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase78L
//
// MOD_0003.18 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID16_LEN16]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase78L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 16/16 bit.
    //  Three subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );

        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //le
        
        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId16Len16, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex16_16L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 
            || aSubBlockOffset != KTwentyFour )//aSubBlockOffset should be 24
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase79L
//
// MOD_0003.19 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN32]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase79L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //  FindSubBlockOffsetById with %4 and values and L != MaxL
    //  FindSubBlockOffsetById 8/32 bit.
    //  Three subblocks per message with same sb_id.
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );

        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId8Len32, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_32L );
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || aSubBlockOffset != KThirtyTwo )//aSubBlockOffset should be 32
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }
            else
                {
                // offset ok
                }
            }

    
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase97L
//
// MOD_0003.020 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex when msg length not %4]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase97L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Two subblocks per message with same sb_id.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();


        TBuf8<29> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0A );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0F );
        testMsg.Append( 0x7B );
        testMsg.Append( 0x3E );
        testMsg.Append( 0xDC );
        testMsg.Append( 0x37 );

        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        
        testMsg.Append( 0x2c ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x01 );
        testMsg.Append( 0x00 ); 

        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 ); 
        testMsg.Append( 0x00 );
        
        
        testMsg.Append( 0x2c ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x01 );
        testMsg.Append( 0x00 ); 

        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 ); 
        testMsg.Append( 0x00 );

        testMsg.Append( 0x00 ); //extra filler

        TIsiReceiveC msg( testMsg );

        TUint8 subBlockIndex( 2 );
        TUint sbStartOffSet( 0 );
 
        // NET_RAT_INFO sub block
        while ( KErrNone == msg.FindSubBlockOffsetByIndex( ISI_HEADER_SIZE + SIZE_NET_RAT_RESP, subBlockIndex, EIsiSubBlockTypeId8Len8, sbStartOffSet ) )
            {
            //get ratName
            TUint8 ratName( msg.Get8bit( sbStartOffSet + NET_RAT_INFO_OFFSET_RATNAME ) );
            if ( ratName != 0x01 )
                {
                testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex );
                }            
             subBlockIndex++;
            }


        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase100L
//
// MOD_0003.022 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex when sb not found]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase100L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Two subblocks per message with same sb_id.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();


        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0A );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0F );
        testMsg.Append( 0x7B );
        testMsg.Append( 0x3E );
        testMsg.Append( 0xDC );
        testMsg.Append( 0x37 );

        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        
        testMsg.Append( 0x0c ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x01 );
        testMsg.Append( 0x00 ); 
        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 ); 
        testMsg.Append( 0x00 );
        
        
        testMsg.Append( 0x18 ); //sb_start = sb_id, offset 0x18 = 24
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x01 ); 
        testMsg.Append( 0x01 );
        testMsg.Append( 0x00 ); 
        testMsg.Append( 0x00 );
        testMsg.Append( 0x00 ); 
        testMsg.Append( 0x00 );


        TIsiReceiveC msg( testMsg );

        TInt temp=msg.FindSubBlockOffsetByIndex( 0x0c, 3, EIsiSubBlockTypeId8Len8,aSubBlockOffset);

        if( temp != KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex);
            }
            


        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }    


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase102L
//
// MOD_0002.23 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetByIndex sb doesn't exist]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase102L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<12> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a);
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x00 ); //sb_count

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0d, 0x01, EIsiSubBlockTypeId8Len32,aSubBlockOffset);

        if( temp != KErrNotFound )
            {
             testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex);
            }
            
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }  

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase80L
//
// MOD_0004.00 ISIMESSAGE [TIsiSend::Set8Bit new constructor test]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase80L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set8Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg ); //TIsiSend msg( testMsg, KTwenty ); 
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL2, 0x01 );
        msg.Set8bit( KNineteen, 0x0f );
        TBuf8<KTwenty> readyMsg = msg.Complete();

        if( testMsg[ KNineteen ] != 0x0f || testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }

        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2 ); //TIsiSend msg2( testMsg2, KNineteen );       
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg2.Set8bit( KEightteen, 0x0f );
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();
        
        if( testMsg2[ KEightteen ] != 0x0f || testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3 ); // TIsiSend msg3( testMsg3, KEightteen );
          msg3.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        msg3.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg3.Set8bit( KSeventeen, 0x0f );
        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

        if( testMsg3[ KSeventeen ] != 0x0f || testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }
    
        

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase81L
//
// MOD_0004.01 ISIMESSAGE [TIsiSend::Set16Bit new constructor test]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase81L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set16Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg );  
        msg.Set16bit( ISI_HEADER_OFFSET_MEDIA, 0x0102 );
        msg.Set16bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x0103 );
 
        msg.Set16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x0104 );
        msg.Set16bit( ISI_HEADER_OFFSET_TRANSID, 0x0201 );

        TBuf8<KTwenty> readyMsg = msg.Complete();
        

            if( testMsg[ KZero ] != 0x01 || testMsg[ KOne ] != 0x02 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
            else if( testMsg[ KTwo ] != 0x01 || testMsg[ KThree ] != 0x03 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg[ KSix ] != 0x01 || testMsg[ KSeven ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg[ KEight ] != 0x02 || testMsg[ KNine ] != 0x01 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }    
            else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }                                         

            if( testMsg[KFour] != 0x00 || testMsg[KFive]!= 0x04)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
            }                                     
        


       TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2 ); //TIsiSend msg2( testMsg2, KNineteen );
        msg2.Set16bit( ISI_HEADER_OFFSET_MEDIA, 0x0102 );
        msg2.Set16bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x0103 );
        msg2.Set16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x0104 );
        msg2.Set16bit( ISI_HEADER_OFFSET_TRANSID, 0x0201 );

        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

            if( testMsg2[ KZero ] != 0x01 || testMsg2[ KOne ] != 0x02 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
            else if( testMsg2[ KTwo ] != 0x01 || testMsg[ KThree ] != 0x03 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg2[ KSix ] != 0x01 || testMsg2[ KSeven ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg2[ KEight ] != 0x02 || testMsg2[ KNine ] != 0x01 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }    
            else if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }                               
            
            if( testMsg2[KFour] != 0x00 || testMsg2[KFive]!= 0x04)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
            }                                     
        
        CleanupStack::PopAndDestroy( readyMsg2 );        

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3 ); //TIsiSend msg3( testMsg3, KEightteen );
        msg3.Set16bit( ISI_HEADER_OFFSET_MEDIA, 0x0102 );
        msg3.Set16bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x0103 );
        msg3.Set16bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x0104 );
        msg3.Set16bit( ISI_HEADER_OFFSET_TRANSID, 0x0201 );
        msg3.Set16bit( 10, 0x0202 );

        TBuf8<KTwenty> readyMsg3 = msg3.Complete();        

            if( testMsg3[ KZero ] != 0x01 || testMsg3[ KOne ] != 0x02 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
            else if( testMsg3[ KTwo ] != 0x01 || testMsg3[ KThree ] != 0x03 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg3[ KSix ] != 0x01 || testMsg3[ KSeven ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }            
            else if( testMsg3[ KEight ] != 0x02 || testMsg3[ KNine ] != 0x01 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }    
            else if( testMsg3[ KTen ] != 0x02 || testMsg3[ KEleven ] != 0x02 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }             
            else if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }                               
 
            if( testMsg3[KFour] != 0x00 || testMsg3[KFive]!= 0x06)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
            }                                     
      

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase82L
//
// MOD_0004.02 ISIMESSAGE [TIsiSend::Set32Bit new constructor test]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase82L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set32Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg );

        msg.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x01020304 );
        msg.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x05060708);

        TBuf8<KTwenty> readyMsg = msg.Complete();

            if( testMsg[ KZero ] != 0x01 || testMsg[ KOne ] != 0x02 ||
            testMsg[ KTwo ] != 0x03 || testMsg[ KThree ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }   
            else if ( testMsg[ KEight ] != 0x05 || testMsg[ KNine ] != 0x06 ||
            testMsg[ KTen ] != 0x07 || testMsg[ KEleven ] != 0x08 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }                
            else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }         
 
            if( testMsg[KFour] != 0x00 || testMsg[KFive]!= 0x06)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
            }         
        
        
        TBuf8<KTwenty> testMsg2;
        TIsiSend msg2( testMsg2 );
 
        msg2.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x09080304 );
        msg2.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x05060107);
        msg2.Set16bit( 12, 0x0506);
        
        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

            if ( testMsg2[KZero] != 0x09 || testMsg2[KOne] != 0x08 ||
            testMsg2[KTwo] != 0x03 || testMsg2[KThree] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }    
            else if ( testMsg2[KEight] != 0x05 || testMsg2[KNine] != 0x06 ||
            testMsg2[ KTen] != 0x01 || testMsg2[KEleven] != 0x07 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }            
            else if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }        
 
            if( testMsg2[KFour] != 0x00 || testMsg2[KFive]!= 0x08)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
            }         
 
        CleanupStack::PopAndDestroy( readyMsg2 );

        TBuf8<KTwenty> testMsg3;
        TIsiSend msg3( testMsg3 );

        msg3.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x01020304 );
        msg3.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x05060708);

        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

            if ( testMsg3[KZero] != 0x01 || testMsg3[KOne] != 0x02 ||
            testMsg3[KTwo] != 0x03 || testMsg3[KThree] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }  
            if ( testMsg3[KEight] != 0x05 || testMsg3[KNine] != 0x06 ||
            testMsg3[ KTen] != 0x07 || testMsg3[KEleven] != 0x08 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }
                        
            else if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }          

            if( testMsg3[KFour] != 0x00 || testMsg3[KFive]!= 0x06)
            {
                testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );
            }                             


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase98L
//
// MOD_0004.03 ISIMESSAGE [TIsiSend::Complete finallength test]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase98L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //
    //    Set32Bit with %2, %4 and %any and values and L == MaxL
    //    0 OFFSET length
    //

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KTwenty> testMsg;
        TIsiSend msg( testMsg, KTwenty );

        msg.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x01020304 );
        msg.Set32bit( ISI_HEADER_OFFSET_TRANSID, 0x05060708);

        TBuf8<KTwenty> readyMsg = msg.Complete();

            if( testMsg[ KZero ] != 0x01 || testMsg[ KOne ] != 0x02 ||
            testMsg[ KTwo ] != 0x03 || testMsg[ KThree ] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }   
            else if ( testMsg[ KEight ] != 0x05 || testMsg[ KNine ] != 0x06 ||
            testMsg[ KTen ] != 0x07 || testMsg[ KEleven ] != 0x08 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }                
            else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }  
            
            if( readyMsg.Length() != KTwenty ) testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );       

        
        TBuf8<KSixteen> testMsg2;
        TIsiSend msg2( testMsg2, KSixteen );

        msg2.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x09080304 );
        msg2.Set8bit( ISI_HEADER_OFFSET_TRANSID, 0x05);

        HBufC8* readyMsg2 = msg2.Complete().AllocLC();

            if ( testMsg2[KZero] != 0x09 || testMsg2[KOne] != 0x08 ||
            testMsg2[KTwo] != 0x03 || testMsg2[KThree] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }    
            else if ( testMsg2[KEight] != 0x05 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet8BitError );
            }            
            else if( testMsg2.Compare( readyMsg2->Des() ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }  
            
            if( readyMsg2->Length() != KSixteen ) testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );             
        CleanupStack::PopAndDestroy( readyMsg2 );


        TBuf8<KEightteen> testMsg3;
        TIsiSend msg3( testMsg3, KEightteen );

        msg3.Set32bit( ISI_HEADER_OFFSET_MEDIA, 0x01020304 );
        msg3.Set16bit( ISI_HEADER_OFFSET_TRANSID, 0x0506);

        TBuf8<KTwenty> readyMsg3 = msg3.Complete();

            if ( testMsg3[KZero] != 0x01 || testMsg3[KOne] != 0x02 ||
            testMsg3[KTwo] != 0x03 || testMsg3[KThree] != 0x04 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }  
            if ( testMsg3[KEight] != 0x05 || testMsg3[KNine] != 0x06 ) 
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet16BitError );
            }
                        
            else if( testMsg3.Compare( readyMsg3 ) != KErrNone )
            {
            testBase->PrintCaseProgress( KErrNotFound, KSet32BitError );
            }          
            if( readyMsg3.Length() != KEightteen ) testBase->PrintCaseProgress( KErrNotFound, KTisiEndianError );             

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase83L
//
// MOD_0005.00 ISIMESSAGE [TIsiSubBlock test]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase83L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<25> testMsg;
        TIsiSend msg( testMsg, 21 );
        msg.Set8bit( 0, 0x01 );
        msg.Set8bit( 1, 0x02 );        
        msg.Set8bit( 2,  0x03 );
        msg.Set8bit( 3,  0x04 );        
        msg.Set8bit( 4,  0x05 );
        msg.Set8bit( 5, 0x06 );    
        msg.Set8bit( 6, 0x07 );
        msg.Set8bit( 7, 0x08 );
        msg.Set8bit( 8,  0x09 );
        msg.Set8bit( 9,  0x0A );        
        msg.Set8bit( 10,  0x0B );
        msg.Set8bit( 11,  0x0C );
        msg.Set8bit( 12,  0x0D );   
        TIsiSubBlock subBlock(testMsg, 0x0102, EIsiSubBlockTypeId16Len8 );  //take 3 bytes
        msg.Set8bit( 16, 0x11);  //imaginary sb data
        msg.Set8bit( 17,  0x12 );   
        msg.Set8bit( 18,  0x13 );   
        msg.Set8bit( 19,  0x14 );   
        subBlock.CompleteSubBlock();

        msg.Complete(); //msg length is bufferLen - 6

        if ( testMsg[5] != 0x0F )
        {
            testBase->PrintCaseProgress( KErrNotFound, KSubblockError );
        }  

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase84L
//
// MOD_0005.01 ISIMESSAGE [TIsiSubBlock test1]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase84L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<25> testMsg;
        TIsiSend msg( testMsg, 20 );
        msg.Set8bit( 0, 0x01 );
        msg.Set8bit( 1, 0x02 );        
        msg.Set8bit( 2,  0x03 );
        msg.Set8bit( 3,  0x04 );        
        msg.Set8bit( 4,  0x05 );
        msg.Set8bit( 5, 0x06 );    
        msg.Set8bit( 6, 0x07 );
        msg.Set8bit( 7, 0x08 );
        msg.Set8bit( 8,  0x09 );
        msg.Set8bit( 9,  0x0A );        
        msg.Set8bit( 10,  0x0B );
        msg.Set8bit( 11,  0x0C );
 
        TIsiSubBlock subBlock(testMsg, 0x01, EIsiSubBlockTypeId8Len8 );  //take 2 bytes
        msg.Set8bit( 14, 0x11);  //imaginary sb data
        msg.Set8bit( 15,  0x11 );   
        msg.Set8bit( 16,  0x11 );   
        msg.Set8bit( 17,  0x11 );   
        subBlock.CompleteSubBlock();

        msg.Complete(); //msg length is bufferLen - 6

        if ( testMsg[5] != 0x0E )
        {
            testBase->PrintCaseProgress( KErrNotFound, KSubblockError );
        }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase85L
//
// MOD_0005.02 ISIMESSAGE [TIsiSubBlock test2]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase85L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();


    TBuf8<30> testMsg;
        TIsiSend msg( testMsg, 20 );
        msg.Set8bit( 0, 0x01 );
        msg.Set8bit( 1, 0x02 );        
        msg.Set8bit( 2,  0x03 );
        msg.Set8bit( 3,  0x04 );        
        msg.Set8bit( 4,  0x05 );
        msg.Set8bit( 5, 0x06 );    
        msg.Set8bit( 6, 0x07 );
        msg.Set8bit( 7, 0x08 );
        msg.Set8bit( 8,  0x09 );
        msg.Set8bit( 9,  0x0A );        
        msg.Set8bit( 10,  0x0B );
        msg.Set8bit( 11,  0x0C );
        TIsiSubBlock subBlock(testMsg, 0x0102, EIsiSubBlockTypeId16Len16 );  //take 4 bytes
        msg.Set8bit( 16, 0x11);  //imaginary sb data

        subBlock.CompleteSubBlock(); //adds 3 fillers to sb

        msg.Complete(); //msg length is bufferLen - 6

        if ( testMsg[5] != 0x0E )
        {
            testBase->PrintCaseProgress( KErrNotFound, KSubblockError );
        }


    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase86L
//
// MOD_0005.03 ISIMESSAGE [TIsiSubBlock test3]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase86L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<30> testMsg;
        TIsiSend msg( testMsg, 24 );
        msg.Set8bit( 0, 0x01 );
        msg.Set8bit( 1, 0x02 );        
        msg.Set8bit( 2,  0x03 );
        msg.Set8bit( 3,  0x04 );        
        msg.Set8bit( 4,  0x05 );
        msg.Set8bit( 5, 0x06 );    
        msg.Set8bit( 6,  0x07 );
        msg.Set8bit( 7, 0x08 );    
        msg.Set32bit( 8, 0x01020304 );
        msg.Set8bit( 12,  0x0D );   
        TIsiSubBlock subBlock(testMsg, 0x01, EIsiSubBlockTypeId8Len16 );  //take 4 bytes
        msg.Set8bit( 17, 0x11);  //imaginary sb data
        msg.Set8bit( 18,  0x11 );   
        subBlock.CompleteSubBlock(); //adds 2 fillers to sb
        msg.Set8bit( 21,  0x00 ); //filler to make message to be divisible by 4
        msg.Set8bit( 22,  0x00 ); //filler to make message to be divisible by 4
        msg.Set8bit( 23,  0x00 ); //filler to make message to be divisible by 4

        msg.Complete(); //msg length is bufferLen - 6

        if ( testMsg[5] != 0x12)
        {
            testBase->PrintCaseProgress( KErrNotFound, KSubblockError );
        }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase87L
//
// MOD_0005.04 ISIMESSAGE [TIsiSubBlock test4]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase87L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<30> testMsg;
        TIsiSend msg( testMsg, 25 );
        msg.Set8bit( 0, 0x01 );
        msg.Set8bit( 1, 0x02 );        
        msg.Set16bit( 2,  0x0304 );
        msg.Set8bit( 4,  0x05 );
        msg.Set8bit( 5, 0x06 );    
        msg.Set8bit( 6, 0x07 );
        msg.Set8bit( 7, 0x08 );
        msg.Set8bit( 8,  0x09 );
        msg.Set8bit( 9,  0x0A );        
        msg.Set8bit( 10,  0x0B );
        msg.Set8bit( 11,  0x0C );
        msg.Set8bit( 12,  0x0D );   
        TIsiSubBlock subBlock(testMsg, 0x01, EIsiSubBlockTypeId8Len32 );  //take 8 bytes
        msg.Set8bit( 21, 0x11);  //imaginary sb data
        msg.Set8bit( 22,  0x11 );   
        subBlock.CompleteSubBlock();

        msg.Complete(); //msg length is bufferLen - 6

        if ( testMsg[5] != 0x13)
        {
            testBase->PrintCaseProgress( KErrNotFound, KSubblockError );
        }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }    

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase88L
//
// MOD_0005.05 ISIMESSAGE [TIsiSubBlock test error case]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase88L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

    TBuf8<23> testMsg;
        TIsiSend msg( testMsg, 23 );
        msg.Set8bit( 0, 0x01 );
        msg.Set8bit( 1, 0x02 );        
        msg.Set8bit( 2,  0x03 );
        msg.Set8bit( 3,  0x04 );        
        msg.Set8bit( 4,  0x05 );
        msg.Set8bit( 5, 0x06 );    
        msg.Set8bit( 6, 0x07 );
        msg.Set8bit( 7, 0x08 );
        msg.Set8bit( 8,  0x09 );
        msg.Set8bit( 9,  0x0A );        
        msg.Set8bit( 10,  0x0B );
        msg.Set8bit( 11,  0x0C );
        msg.Set8bit( 12,  0x0D );   
        TIsiSubBlock subBlock(testMsg, 0x01, EIsiSubBlockTypeId8Len32 );  //take 8 bytes
        msg.Set8bit( 21, 0x11);  //imaginary sb data
        msg.Set8bit( 22,  0x11 );   
        subBlock.CompleteSubBlock(); //crashes here because fillers added at the end of sb and buffer overflows

        msg.Complete(); //msg length is bufferLen - 6

        if ( testMsg[5] != 19)
        {
            testBase->PrintCaseProgress( KErrNotFound, KSubblockError );
        }

    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    } 
        
       

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase89L
//
// MOD_0002.20 ISIMESSAGE [TIsiReceiveC::FindSubBlockOffsetById SB_TYPE_ID8_LEN32 when sb_len = 0]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase89L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetByIdL with %4 and values and L != MaxL
    //  FindSubBlockOffsetByIdL 8/8 bit, 8/16 bit, 16/8 bit, 16/16 bit, 8/32 bit.
    //  Two different subblocks per message we try to search the last one.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<KMsgMaxLength> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a);
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

        testMsg.Append( 0x05 ); //sb_start2 8bit, offset 12 = 0x0c
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len

        testMsg.Append( 0x06 ); //sb_start2 8bit, offset 20 = 0x14
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len 32bit

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetById( 0x0c, 0x06, EIsiSubBlockTypeId8Len32,aSubBlockOffset);


        if( temp != KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetById8_32L );
            }
       
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }    

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase90L
//
// MOD_0003.20 ISIMESSAGE [TIsiReceiveC::FindSubblockOffsetByIndex SB_TYPE_ID8_LEN8 sb_len==0]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase90L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Two subblocks per message with same sb_id.

    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<28> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );

        TIsiReceiveC msg( testMsg );
        TInt temp(KErrNone);
        aSubBlockOffset = 0;
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, EIsiSubBlockTypeId8Len8, aSubBlockOffset);
        
        if( temp != KErrNotFound )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFindSubblockOffsetByIndex8_8L );
            }
        
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase91L
//
// MOD_0005.21 ISIMESSAGE [TIsiReceiveC::GetBuffer use case]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase91L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {

    //  FindSubBlockOffsetById with %4 and values and L == MaxL
    //  FindSubBlockOffsetById 8/8 bit.
    //  Two subblocks per message with same sb_id.
    
    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();


    TBuf8<16> testMsg;
    testMsg.Append( 0x00 ); 
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x02 ); 
    testMsg.Append( 0x03 ); 
    testMsg.Append( 0x00 ); 
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x02 ); 
    testMsg.Append( 0x03 );
    TIsiReceiveC msg( testMsg);
    TPtrC8 pointer =  msg.GetBuffer();
    //check length
    TUint msgLength(0);
    msgLength = pointer.Length();

        if( msgLength != 8 )
            {
            testBase->PrintCaseProgress( KErrNotFound, KFail );
            }
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase92L
//
// MOD_0006.00 ISIMESSAGE [TIsiUtility::CopyFromBigEndian]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase92L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();


    
        TBuf8<8> aSource;
        TBuf16<18> aTarget;
        aSource.Append( 0x01 ); 
        aSource.Append( 0x02 ); 
        aSource.Append( 0x02 ); 
        aSource.Append( 0x03 ); 
        aSource.Append( 0x03 ); 
        aSource.Append( 0x04 ); 
        aSource.Append( 0x04 ); 
        aSource.Append( 0x05 );

        TIsiUtility utilTest;
        utilTest.CopyFromBigEndian ( aSource, aTarget);

        if( aTarget[0] != 0x0102 || aTarget[1] != 0x0203 || aTarget[2] != 0x0304 || aTarget[3] != 0x0405 ) 
            {
             testBase->PrintCaseProgress( KErrNotFound, KCopyFromBigEndianError );
            }
       
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
                


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase93L
//
// MOD_0006.01 ISIMESSAGE [TIsiUtility::CopyToBigEndian]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase93L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf16<8> aSource;
        TBuf8<18> aTarget;

        aSource.Append( 0x0102 ); 
        aSource.Append( 0x0304 ); 
        aSource.Append( 0x0506 ); 
        aSource.Append( 0x0708 ); 
        aSource.Append( 0x0910 ); 
        aSource.Append( 0x1112 ); 
        aSource.Append( 0x1314 ); 
        aSource.Append( 0x1516 );

        TIsiUtility utilTest;
        utilTest.CopyToBigEndian ( aSource, aTarget);

        if( aTarget[0] != 0x01 || aTarget[1] != 0x02 || aTarget[2] != 0x03 || aTarget[3] != 0x04 ||
            aTarget[4] != 0x05 || aTarget[5] != 0x06 || aTarget[6] != 0x07 || aTarget[7] != 0x08 ||
            aTarget[8] != 0x09 || aTarget[9] != 0x10 || aTarget[10] != 0x11 || aTarget[11] != 0x12 ||
            aTarget[12] != 0x13 || aTarget[13] != 0x14 || aTarget[14] != 0x15 || aTarget[15] != 0x16 ) 
            {
             testBase->PrintCaseProgress( KErrNotFound, KCopyToBigEndianError );
            }        
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase94L
//
// MOD_0006.02 ISIMESSAGE [TIsiUtility::CopyFromBigEndian1]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase94L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf8<18> aSource;
        TBuf16<5> aTarget;
        aSource.Append( 0x01 ); 
        aSource.Append( 0x02 ); 
        aSource.Append( 0x02 ); 
        aSource.Append( 0x03 ); 
        aSource.Append( 0x03 ); 
        aSource.Append( 0x04 ); 
        aSource.Append( 0x04 ); 
        aSource.Append( 0x05 );
        aSource.Append( 0x03 ); 
        aSource.Append( 0x03 ); 
        aSource.Append( 0x04 ); 

        TIsiUtility utilTest;
        utilTest.CopyFromBigEndian ( aSource, aTarget);

        if( aTarget[0] != 0x0102 || aTarget[1] != 0x0203 || aTarget[2] != 0x0304 || aTarget[3] != 0x0405 || aTarget[4] != 0x0303 ) 
            {
             testBase->PrintCaseProgress( KErrNotFound, KCopyFromBigEndianError );
            }
       
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }
                


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::RunClassCase95L
//
// MOD_0006.03 ISIMESSAGE [TIsiUtility::CopyToBigEndian1]
// -----------------------------------------------------------------------------
//
TInt CIsiMsgApiTest::RunClassCase95L
        (
        TExecutionAndDebugInfo& aInfo // For debug information etc..
        )
    {


    // Stores the error code.
    TInt result( KErrNone );

    // New test base. Leave the pointer to CleanupStack.
    // Handles the ISA Api Connection, sending and receiving of IsiMsg's
    // and writing to STIF, ATS and Logger.
    CIsiMsgBaseTest* testBase = CIsiMsgBaseTest::NewLC( aInfo, result );
    // Open ISA Api and get new Isi Msg.
    testBase->GetIsiMsgL();

        TBuf16<8> aSource;
        TBuf8<14> aTarget;

        //aSource.Size() > aTarget.MaxSize(

        aSource.Append( 0x0102 ); 
        aSource.Append( 0x0304 ); 
        aSource.Append( 0x0506 ); 
        aSource.Append( 0x0708 ); 
        aSource.Append( 0x0910 ); 
        aSource.Append( 0x1112 ); 
        aSource.Append( 0x1314 ); 
        aSource.Append( 0x1516 );

        TIsiUtility utilTest;
        utilTest.CopyToBigEndian ( aSource, aTarget);

        if( aTarget[0] != 0x01 || aTarget[1] != 0x02 || aTarget[2] != 0x03 || aTarget[3] != 0x04 ||
            aTarget[4] != 0x05 || aTarget[5] != 0x06 || aTarget[6] != 0x07 || aTarget[7] != 0x08 ||
            aTarget[8] != 0x09 || aTarget[9] != 0x10 || aTarget[10] != 0x11 || aTarget[11] != 0x12 ||
            aTarget[12] != 0x13 || aTarget[13] != 0x14  ) 
            {
             testBase->PrintCaseProgress( KErrNotFound, KCopyToBigEndianError );
            }        
        
    // Close the IsaApi.
    testBase->Close();
    // RGpdsove and delete from CleanupStack.
    CleanupStack::PopAndDestroy( testBase );

    // Case was executed.
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIsiMsgApiTest::CIsiMsgApiTest
// Default constructor.
// -----------------------------------------------------------------------------
//
CIsiMsgApiTest::CIsiMsgApiTest
        (
        // None
        )
    {

    // None

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::ConstructL
// Symbian OS second phase constructor.
// -----------------------------------------------------------------------------
//
void CIsiMsgApiTest::ConstructL
        (
        // None
        )
    {

    // This initializes the test cases
    InitializeCasesL();

    }

// -----------------------------------------------------------------------------
// CIsiMsgApiTest::InitializeCasesL
// Initializes the test cases.
// -----------------------------------------------------------------------------
//
void CIsiMsgApiTest::InitializeCasesL
        (
        // None
        )
    {

    AddTestCaseL( KIsiMsgSet8BitTest0, ( MTestFunction )GETPTR( RunClassCase00L ) );
    AddTestCaseL( KIsiMsgSet8BitTest1, ( MTestFunction )GETPTR( RunClassCase01L ) );
    AddTestCaseL( KIsiMsgSet8BitTest2, ( MTestFunction )GETPTR( RunClassCase02L ) );
    AddTestCaseL( KIsiMsgSet8BitTest3, ( MTestFunction )GETPTR( RunClassCase03L ) );
    AddTestCaseL( KIsiMsgSet8BitTest4, ( MTestFunction )GETPTR( RunClassCase04L ) );
    AddTestCaseL( KIsiMsgSet8BitTest5, ( MTestFunction )GETPTR( RunClassCase05L ) );

    AddTestCaseL( KIsiMsgSet16BitTest0, ( MTestFunction )GETPTR( RunClassCase06L ) );
    AddTestCaseL( KIsiMsgSet16BitTest1, ( MTestFunction )GETPTR( RunClassCase07L ) );
    AddTestCaseL( KIsiMsgSet16BitTest2, ( MTestFunction )GETPTR( RunClassCase08L ) );
    AddTestCaseL( KIsiMsgSet16BitTest3, ( MTestFunction )GETPTR( RunClassCase09L ) );
    AddTestCaseL( KIsiMsgSet16BitTest4, ( MTestFunction )GETPTR( RunClassCase10L ) );
    AddTestCaseL( KIsiMsgSet16BitTest5, ( MTestFunction )GETPTR( RunClassCase11L ) );

    AddTestCaseL( KIsiMsgSet32BitTest0, ( MTestFunction )GETPTR( RunClassCase12L ) );
    AddTestCaseL( KIsiMsgSet32BitTest1, ( MTestFunction )GETPTR( RunClassCase13L ) );
    AddTestCaseL( KIsiMsgSet32BitTest2, ( MTestFunction )GETPTR( RunClassCase14L ) );
    AddTestCaseL( KIsiMsgSet32BitTest3, ( MTestFunction )GETPTR( RunClassCase15L ) );
    AddTestCaseL( KIsiMsgSet32BitTest4, ( MTestFunction )GETPTR( RunClassCase16L ) );
    AddTestCaseL( KIsiMsgSet32BitTest5, ( MTestFunction )GETPTR( RunClassCase17L ) );

    AddTestCaseL( KIsiMsgCopyData, ( MTestFunction )GETPTR( RunClassCase18L ) );

    AddTestCaseL( KIsiMsgGet8BitTest0, ( MTestFunction )GETPTR( RunClassCase19L ) );
    AddTestCaseL( KIsiMsgGet8BitTest1, ( MTestFunction )GETPTR( RunClassCase20L ) );
    AddTestCaseL( KIsiMsgGet8BitTest2, ( MTestFunction )GETPTR( RunClassCase21L ) );
    AddTestCaseL( KIsiMsgGet8BitTest3, ( MTestFunction )GETPTR( RunClassCase22L ) );
    AddTestCaseL( KIsiMsgGet8BitTest4, ( MTestFunction )GETPTR( RunClassCase23L ) );
    AddTestCaseL( KIsiMsgGet8BitTest5, ( MTestFunction )GETPTR( RunClassCase24L ) );

    AddTestCaseL( KIsiMsgGet16BitTest0, ( MTestFunction )GETPTR( RunClassCase25L ) );
    AddTestCaseL( KIsiMsgGet16BitTest1, ( MTestFunction )GETPTR( RunClassCase26L ) );
    AddTestCaseL( KIsiMsgGet16BitTest2, ( MTestFunction )GETPTR( RunClassCase27L ) );
    AddTestCaseL( KIsiMsgGet16BitTest3, ( MTestFunction )GETPTR( RunClassCase28L ) );
    AddTestCaseL( KIsiMsgGet16BitTest4, ( MTestFunction )GETPTR( RunClassCase29L ) );
    AddTestCaseL( KIsiMsgGet16BitTest5, ( MTestFunction )GETPTR( RunClassCase30L ) );

    AddTestCaseL( KIsiMsgGet32BitTest0, ( MTestFunction )GETPTR( RunClassCase31L ) );
    AddTestCaseL( KIsiMsgGet32BitTest1, ( MTestFunction )GETPTR( RunClassCase32L ) );
    AddTestCaseL( KIsiMsgGet32BitTest2, ( MTestFunction )GETPTR( RunClassCase33L ) );
    AddTestCaseL( KIsiMsgGet32BitTest3, ( MTestFunction )GETPTR( RunClassCase34L ) );
    AddTestCaseL( KIsiMsgGet32BitTest4, ( MTestFunction )GETPTR( RunClassCase35L ) );
    AddTestCaseL( KIsiMsgGet32BitTest5, ( MTestFunction )GETPTR( RunClassCase36L ) );

    AddTestCaseL( KIsiMsgGetData0, ( MTestFunction )GETPTR( RunClassCase37L ) );
    AddTestCaseL( KIsiMsgGetData1, ( MTestFunction )GETPTR( RunClassCase38L ) );
    AddTestCaseL( KIsiMsgGetData2, ( MTestFunction )GETPTR( RunClassCase39L ) );
    AddTestCaseL( KIsiMsgGetData3, ( MTestFunction )GETPTR( RunClassCase96L ) );

    AddTestCaseL( KIsiMsgFindSubBlockOffsetById0, ( MTestFunction )GETPTR( RunClassCase40L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById1, ( MTestFunction )GETPTR( RunClassCase41L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById2, ( MTestFunction )GETPTR( RunClassCase42L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById3, ( MTestFunction )GETPTR( RunClassCase43L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById4, ( MTestFunction )GETPTR( RunClassCase44L ) );

    AddTestCaseL( KIsiMsgFindSubBlockOffsetById5, ( MTestFunction )GETPTR( RunClassCase45L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById6, ( MTestFunction )GETPTR( RunClassCase46L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById7, ( MTestFunction )GETPTR( RunClassCase47L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById8, ( MTestFunction )GETPTR( RunClassCase48L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById9, ( MTestFunction )GETPTR( RunClassCase49L ) );

    AddTestCaseL( KIsiMsgFindSubBlockOffsetById10, ( MTestFunction )GETPTR( RunClassCase50L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById11, ( MTestFunction )GETPTR( RunClassCase51L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById12, ( MTestFunction )GETPTR( RunClassCase52L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById13, ( MTestFunction )GETPTR( RunClassCase53L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById14, ( MTestFunction )GETPTR( RunClassCase54L ) );    

    AddTestCaseL( KIsiMsgFindSubBlockOffsetById15, ( MTestFunction )GETPTR( RunClassCase55L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById16, ( MTestFunction )GETPTR( RunClassCase56L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById17, ( MTestFunction )GETPTR( RunClassCase57L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById18, ( MTestFunction )GETPTR( RunClassCase58L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById19, ( MTestFunction )GETPTR( RunClassCase59L ) ); 
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById20, ( MTestFunction )GETPTR( RunClassCase89L ) );  
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById21, ( MTestFunction )GETPTR( RunClassCase99L ) ); 
    AddTestCaseL( KIsiMsgFindSubBlockOffsetById22, ( MTestFunction )GETPTR( RunClassCase101L ) );        

    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex0, ( MTestFunction )GETPTR( RunClassCase60L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex1, ( MTestFunction )GETPTR( RunClassCase61L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex2, ( MTestFunction )GETPTR( RunClassCase62L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex3, ( MTestFunction )GETPTR( RunClassCase63L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex4, ( MTestFunction )GETPTR( RunClassCase64L ) );

    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex5, ( MTestFunction )GETPTR( RunClassCase65L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex6, ( MTestFunction )GETPTR( RunClassCase66L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex7, ( MTestFunction )GETPTR( RunClassCase67L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex8, ( MTestFunction )GETPTR( RunClassCase68L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex9, ( MTestFunction )GETPTR( RunClassCase69L ) );

    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex10, ( MTestFunction )GETPTR( RunClassCase70L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex11, ( MTestFunction )GETPTR( RunClassCase71L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex12, ( MTestFunction )GETPTR( RunClassCase72L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex13, ( MTestFunction )GETPTR( RunClassCase73L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex14, ( MTestFunction )GETPTR( RunClassCase74L ) );

    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex15, ( MTestFunction )GETPTR( RunClassCase75L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex16, ( MTestFunction )GETPTR( RunClassCase76L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex17, ( MTestFunction )GETPTR( RunClassCase77L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex18, ( MTestFunction )GETPTR( RunClassCase78L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex19, ( MTestFunction )GETPTR( RunClassCase79L ) );
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex20, ( MTestFunction )GETPTR( RunClassCase90L ) );    
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex21, ( MTestFunction )GETPTR( RunClassCase97L ) );    
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex22, ( MTestFunction )GETPTR( RunClassCase100L ) );  
    AddTestCaseL( KIsiMsgFindSubBlockOffsetByIndex23, ( MTestFunction )GETPTR( RunClassCase102L ) );        

    AddTestCaseL( KGetBufferTest, ( MTestFunction )GETPTR( RunClassCase91L ) );

    AddTestCaseL( KIsiMsgTIsiSendTest, ( MTestFunction )GETPTR( RunClassCase80L ) );
    AddTestCaseL( KIsiMsgTIsiSendTest1, ( MTestFunction )GETPTR( RunClassCase81L ) );
    AddTestCaseL( KIsiMsgTIsiSendTest2, ( MTestFunction )GETPTR( RunClassCase82L ) );
    AddTestCaseL( KIsiMsgTIsiSendTest3, ( MTestFunction )GETPTR( RunClassCase98L ) );

    AddTestCaseL( KTIsiSubBlockTest, ( MTestFunction )GETPTR( RunClassCase83L ) );
    AddTestCaseL( KTIsiSubBlockTest1, ( MTestFunction )GETPTR( RunClassCase84L ) );
    AddTestCaseL( KTIsiSubBlockTest2, ( MTestFunction )GETPTR( RunClassCase85L ) );
    AddTestCaseL( KTIsiSubBlockTest3, ( MTestFunction )GETPTR( RunClassCase86L ) );
    AddTestCaseL( KTIsiSubBlockTest4, ( MTestFunction )GETPTR( RunClassCase87L ) );
    AddTestCaseL( KTIsiSubBlockTest5, ( MTestFunction )GETPTR( RunClassCase88L ) );
    
    
    AddTestCaseL( KCopyFromBigEndianTest, ( MTestFunction )GETPTR( RunClassCase92L ) );
    AddTestCaseL( KCopyToBigEndianTest, ( MTestFunction )GETPTR( RunClassCase93L ) );
    AddTestCaseL( KCopyFromBigEndianTest1, ( MTestFunction )GETPTR( RunClassCase94L ) );
    AddTestCaseL( KCopyToBigEndianTest1, ( MTestFunction )GETPTR( RunClassCase95L ) );
    
    

    }

