/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for NWE tests
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <smsisi.h>
#include "sms.h"


// Converts semioct-code to character, semioctet can be 0x00 - 0x0F
#define gsmSmsSemiOctetToChar( bcd )  gsmSmsSemiOctetToCharTable[bcd]

// This table is used when a 03.40 or 04.11 address is converted to Unicode.
const TUint gsmSmsSemiOctetToCharTable[16] =
   {
   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00
   };


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CSMS::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CSMS::Delete()
    {
    iSmspListArray->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSMS::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CSMS::RunMethodL(
    CStifItemParser& aItem )
    {
    _LIT8 (KLog, "SMS: RunMethodL" );
    iSMSLog->Log((TDesC8)KLog);
    //iSMSLog->Log(_L("SMS: RunMethodL"));
    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "RunIscTestCase", CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase", CCore::SyncIscTestCase),
        ENTRY( "RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined",
                CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined),
        ENTRY( "SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined",
                CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined),
        ENTRY( "SendSMSMessage", CSMS::SendSMSMessageL ),
        ENTRY( "SendTwoSMSMessages", CSMS::SendTwoSMSMessagesL ),
        ENTRY( "ReadSMSMessage", CSMS::ReadSMSMessageL ),
        ENTRY( "DeleteSMSMessage", CSMS::DeleteSMSMessageL ),
        ENTRY( "DeleteAllSMSMessages", CSMS::DeleteAllSMSMessagesL ),
        ENTRY( "SetMoSmsBearer", CSMS::SetMoSmsBearerL ),
        ENTRY( "WriteSMSMessage", CSMS::WriteSMSMessageL ),
        ENTRY( "ReceiveSMSMessage", CSMS::ReceiveSMSMessageL ),
        ENTRY( "ActivateSMSRouting", CSMS::ActivateSMSRoutingL ),
        ENTRY( "AckSMS", CSMS::AckSMSL ),
        ENTRY( "DeactivateSMSRouting", CSMS::DeactivateSMSRoutingL ),
        ENTRY( "ResumeSMS", CSMS::ResumeSMSL ),
        ENTRY( "ReadAllSMSMessage", CSMS::ReadAllSMSMessageL ),
        ENTRY( "NackSMS", CSMS::NackSMSL ),
        ENTRY( "SIMMsgStorageInfo", CSMS::SIMMsgStorageInfoL ),
        ENTRY( "ReadSmsParameters", CSMS::ReadSmsParametersL ),
        ENTRY( "StoreSmsParameters", CSMS::StoreSmsParametersL ),
        ENTRY( "SMSCacheInitialization", CSMS::SMSCacheInitializationL ),
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC", CCore::SetExpectedCompleteIPC),
        ENTRY( "SetSmsParameterCheck", CSMS::SetSmsParameterCheck),
        ENTRY( "SetSmsReferenceParameters", CSMS::SetSmsReferenceParameters),
        ENTRY( "SmsSettingsUpdate", CSMS::SmsSettingsUpdate),
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CSMS::SetSmsParameterCheck
// Sets SMS parameter checking and test case number
// -----------------------------------------------------------------------------
//
TInt CSMS::SetSmsParameterCheck( CStifItemParser& aItem )
    {
    TInt ret( KErrNone );
    _LIT8(KSetSmsParameterCheck, "SMS::SetSmsParameterCheck");
    iSMSLog->Log((TDesC8)KSetSmsParameterCheck);

    // Is parameter checking required
    TPtrC string;
    aItem.GetNextString ( string );

    if ( _L("ON") == string )
        {
        iSmsParameterCheck = ETrue;
        }
    else
        {
        iSmsParameterCheck = EFalse;
        }

    // Test case number, used in parameter checking
    TInt testCaseNumber;
    aItem.GetNextInt( testCaseNumber );
    iTestCaseNumber = (TTestCase) testCaseNumber;

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::SetSmsReferenceParameters
// Sets SMS parameter checking and test case number
// -----------------------------------------------------------------------------
//
TInt CSMS::SetSmsReferenceParameters( CStifItemParser& aItem )
    {
    TInt ret( KErrNone );
    _LIT8(KSetSmsReferenceParameters, "SMS::SetSmsReferenceParameters");
    iSMSLog->Log((TDesC8)KSetSmsReferenceParameters);

    // Set reference parameters depending on test case number
    if ( ETestCaseSms019 == iTestCaseNumber ||
         ETestCaseSms001 == iTestCaseNumber )
        {
        // Message reference
        TInt msgRef( 0 );
        aItem.GetNextInt( msgRef );
        iMsgRef = msgRef;
        // Length of the PDU string
        TInt count( 0 );
        aItem.GetNextInt( count );

        // PDU string
        TInt value( 0 );
        for ( TUint8 i( 0 ); i < count; i++ )
            {
            aItem.GetNextInt( value );
            iPdu.Append( value );
            }
        }
    else if ( ETestCaseSms008 == iTestCaseNumber )
        {
        TInt parameterIndicator;
        TInt protocolId;
        TInt dataCodingScheme;
        TInt validityPeriod;
        TPtrC destAddress;
        TPtrC serviceCenterAddress;
        TPtrC alphaTag;

        // Number of SMS parameter sets
        aItem.GetNextInt( iNumOfSmspSets );

        for ( TUint8 i( 0 ); i < iNumOfSmspSets; i++ )
            {
            TSmsParameters* smsParam = new ( ELeave ) TSmsParameters();
            aItem.GetNextInt( parameterIndicator );
            aItem.GetNextInt( protocolId );
            aItem.GetNextInt( dataCodingScheme );
            aItem.GetNextInt( validityPeriod );
            aItem.GetNextString( serviceCenterAddress );
            aItem.GetNextString( destAddress );
            aItem.GetNextString( alphaTag );

            smsParam->iParameterIndicator = parameterIndicator;
            smsParam->iProtocolId = protocolId;
            smsParam->iDataCodingScheme = dataCodingScheme;
            smsParam->iValidityPeriod = validityPeriod;
            smsParam->iServiceCenterAddress = serviceCenterAddress;
            smsParam->iDestinationAddress = destAddress;
            smsParam->iAlphaTagData = alphaTag;

            iSmspListArray->AppendL( smsParam );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::SmsSettingsUpdate
// Update SMS route settings
// -----------------------------------------------------------------------------
//
TInt CSMS::SmsSettingsUpdate( CStifItemParser& /*aItem*/ )
    {
    enum TMobileSmsBearer
        {
    /** SMS messages will only be sent over a packet-switched (GPRS) network. */
        ESmsBearerPacketOnly,
    /** SMS messages will only be sent over a circuit-switched (GSM) network. */
        ESmsBearerCircuitOnly,
    /** SMS messages will be sent over the packet-switched (GPRS) network if possible,
    otherwise over circuit-switched (GSM) network. */
        ESmsBearerPacketPreferred,
    /** SMS messages will be sent over the circuit-switched (GSM) network if possible,
    otherwise over packet-switched (GPRS) network. */
        ESmsBearerCircuitPreferred
        };

    TInt ret( KErrNone );
    _LIT8( KSmsSettingsUpdate, "SMS::SmsSettingsUpdate");
    iSMSLog->Log((TDesC8)KSmsSettingsUpdate);

    //Create package
    CMmDataPackage package;
    TMobileSmsBearer bearer( ESmsBearerPacketOnly );
    //Pack parameters
    package.PackData( &bearer );

    ret = HandleRequestL( EMobileSmsMessagingSetMoSmsBearer, &package );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::SendSMSMessage
// SendSMSMessage method function.
// -----------------------------------------------------------------------------
//
TInt CSMS::SendSMSMessageL(CStifItemParser& aItem)
    {
    _LIT8(KSendSMSMessage, "SMS::SendSMSMessage");
    iSMSLog->Log((TDesC8)KSendSMSMessage);

    TInt ret = KErrNotFound;
    TPtrC16  string;               // Name of the test case
    TPtrC  number;                 // Number of destination
    TPtrC  messagecenternumber;    // Number of MessageCenter
    TPtrC  satoriginated;          // SAT ORIGINATED SMS
    aItem.GetNextString(string);
    aItem.GetNextString(number);
    aItem.GetNextString(messagecenternumber);
    aItem.GetNextString(satoriginated);

    TBuf8 <260> msgdata;
    if (string == (_L("SendSMSMessage4")))
        {
        TUint MsgParameters( 0 );
        const TUint SmsMsgTypeSubmitOrSubmitR   = 0x01;
        const TUint SmsMsgVPabsolute            = 0x34;     // absolute format

        // TSY doesn't handle this information. Destination address goes in PDU.
        iMsgAttrib.iDestination.iTelNumber = number; //_L("0405431828");
        iMsgAttrib.iDestination.iTypeOfNumber = RMobilePhone::ENationalNumber;
        iMsgAttrib.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
        TBuf8<160> source;
        TUint8 UD[145]={'t','e','s','t',' ','m','e','s','s','a','g','e',' ',
            'f','o','u','r',' ','4',' ','*','*','*','*','*','*','*','*','*',
            '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',
            '*','*','*','*','*','*','*','*','*','*','*','*','*','*',
            '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',
            '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',
            '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',
            '*','*','*','*','*','*','*','*','*','*','*','*','*','*','*','*',
            '*','*','*','*','*','*','*','*','*','*','*',' ','l','e','n','g',
            't','h','=','1','4','9'};

        source.Append(&UD[0],sizeof(UD));

        // If you want to convert user data to 7bit format:
        TBuf8<140> UserData;
        TUint16 byte_count = (TUint16)source.Length();
        GsmLibCharConversion( TKGsmLibConvertToGsmFormat, source, byte_count );
        GsmLibSmsPackMessage( UserData, source, byte_count );
        TUint16 charCnt = (TUint16)( (UserData.Length()*8) / 7 );
        if ( ( ( ( iGsmMes.Length()*8 ) % 7 ) == 0) &&
            charCnt && charCnt <= iGsmMes.Length() )
            {
            if ( UserData[charCnt-1] == 0 )
            // if the last char is NULL, Actual charCnt is charCnt-1
                {
                charCnt--;
                }
            }

        //CASE: SEND SMS MESSAGE
        iMsgAttrib.iFlags = (RMobileSmsMessaging::KSmsDataFormat +
                        RMobileSmsMessaging::KGsmServiceCentre +
                        RMobileSmsMessaging::KRemotePartyInfo +
                        RMobileSmsMessaging::KMoreToSend);
        // TSY takes now SC address from front of the TPDU
        iMsgAttrib.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
        iMsgAttrib.iGsmServiceCentre.iNumberPlan =
            RMobilePhone::EIsdnNumberPlan;
        iMsgAttrib.iGsmServiceCentre.iTypeOfNumber =
            RMobilePhone::EInternationalNumber;

        iMsgAttrib.iGsmServiceCentre.iTelNumber = messagecenternumber;
        iMsgAttrib.iMore = (TBool)(EFalse);

        MsgParameters = 0;
        MsgParameters |= SmsMsgTypeSubmitOrSubmitR;
        MsgParameters |= SmsMsgVPabsolute;
        TBuf8<7> ValidityPeriod;
        TUint8 validity[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
        ValidityPeriod.Append(&validity[0],sizeof(validity));
        TBuf<22> DEtemp = number;
        TPtrC16 DEnumber = DEtemp;
        TBuf8<12> DestinationAddress;
        GsmConvUnicodeTo0340Addr(
            2,   //TON national number
            1,   //NPI isdn numbering plan
            DestinationAddress,
            DEnumber );
        msgdata.SetLength(0);                   //reset buffer
        msgdata.Append(MsgParameters);          //Message params
        msgdata.Append(0x10);                   //TP-Message-Reference
        msgdata.Append(DestinationAddress);     //TP-Destination-Address
        msgdata.Append(0x00);                   //TP-Protocol-Identifier
        msgdata.Append(0x02);                   //TP-Data-Coding-Scheme (8bit)
        msgdata.Append(0xA7);                   //TP-Validity-Period
        msgdata.Append(charCnt);                //TP-User-Data-Length
        msgdata.Append(UserData);               //TP-User-Data

         //Create package
        CMmDataPackage package;

        //structure for all sms parameters and data
        TSendSmsDataAndAttributes sendData;

        sendData.iAttributes = &iMsgAttrib;
        sendData.iMsgData = &msgdata;
        if (satoriginated == (_L("SAT_ORIGINATED")))
            {
            sendData.iIpc = EMmTsySmsSendSatMessage;
            }
        else if (satoriginated == (_L("NO_FDN_CHECK")))
            {
            sendData.iIpc = EMobileSmsMessagingSendMessageNoFdnCheck;
            }
        else
            {
            sendData.iIpc = EMobileSmsMessagingSendMessage;
            }
        //Pack parameters
        package.PackData( &sendData );

        if (satoriginated == (_L("SAT_ORIGINATED")))
            {
            ret = HandleRequestL( EMmTsySmsSendSatMessage, &package );
            }
        else if (satoriginated == (_L("NO_FDN_CHECK")))
            {
            ret = HandleRequestL( EMobileSmsMessagingSendMessageNoFdnCheck, &package );
            }
        else
            {
            ret = HandleRequestL( EMobileSmsMessagingSendMessage, &package );
            }

        // Signal with return status if it is not KErrNone
        if ( KErrNone != ret )
            {
            Signal ( ret );
            ret = KErrNone;
            }

        if ( KErrNone == ret )

            {
            _LIT8( KSendOk, "Send SMS Message ok" );
            iSMSLog->Log((TDesC8)KSendOk );
            iSMSLog->Log( _L("Send SMS Message ok"));
            }
        else
            {
            _LIT8( KSendFailed, "Send SMS Message failed: %d" );
            iSMSLog->Log((TDesC8)KSendFailed, ret );
            }
        }

    else
        {
        ret = KErrNotFound;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteSendSMSMessage
// Complete SendSMSMessage method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteSendSMSMessage( TInt aResult ) //return Completion result value
    {
    _LIT8( KResult, "CompleteSendSMSMessage result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteSendSMSMessage
// Complete SendSMSMessage method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteSendSMSMessage( TInt aResult, CMmDataPackage* aDataPackage )
    {
    _LIT8( KResult, "CompleteSendSMSMessage result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    // Check parameters depending on the test case
    if ( iSmsParameterCheck &&
        ( ETestCaseSms019 == iTestCaseNumber ||
        ETestCaseSms001 == iTestCaseNumber ) )
        {
        TUint16 msgRef( 0 );
        TBuf8<RMobileSmsMessaging::KGsmTpduSize> pdu;
        aDataPackage->UnPackData( msgRef, pdu );

        if ( msgRef != iMsgRef || pdu != iPdu )
            {
            aResult = KErrGeneral;
            }
        }

    iTestCaseNumber = ETestCaseNotDefined;
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::SendTwoSMSMessages
// SendTwoSMSMessages method function.
// -----------------------------------------------------------------------------
//
TInt CSMS::SendTwoSMSMessagesL( CStifItemParser& aItem )
    {
    _LIT8(KSendSMSMessage, "SMS::SendTwoSMSMessages");
    iSMSLog->Log((TDesC8)KSendSMSMessage);

    TInt ret = KErrNotFound;
    TPtrC16  string;               // Name of the test case
    TPtrC  number;                 // Number of destination
    TPtrC  messagecenternumber;    // Number of MessageCenter
    aItem.GetNextString(string);
    aItem.GetNextString(number);
    aItem.GetNextString(messagecenternumber);
    TBuf8 <260> msgdata;
    if (string == (_L("SendTwoSMSMessage1")))
        {
        TUint MsgParameters( 0 );
        const TUint SmsMsgTypeSubmitOrSubmitR   = 0x01;
        const TUint SmsMsgVPabsolute            = 0x34;     // absolute format


        // TSY doesn't handle this information. Destination address goes in PDU.
        iMsgAttrib.iDestination.iTelNumber = number;


        iMsgAttrib.iDestination.iTypeOfNumber = RMobilePhone::ENationalNumber;
        iMsgAttrib.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan;


        TBuf8<160> source;


        if (iSecondMessage == EFalse)
            {
            TUint8 UD[35]=
                {
                'F','i','r','s','t',' ','m','e','s','s','a','g','e',
                ' ','T','e','s','t',' ','S','e','n','d',' ','t','w','o',
                ' ','m','e','s','s','a','g','e'
                };
            source.Append(&UD[0],sizeof(UD));
            iSecondMessage = ETrue; // First message has been send


            //CASE: SEND SMS MESSAGE
            iMsgAttrib.iFlags = (RMobileSmsMessaging::KSmsDataFormat +
                            RMobileSmsMessaging::KGsmServiceCentre +
                            RMobileSmsMessaging::KRemotePartyInfo +
                            RMobileSmsMessaging::KMoreToSend);

            // TSY takes now SC address from front of the TPDU
            iMsgAttrib.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
            iMsgAttrib.iGsmServiceCentre.iNumberPlan =
                RMobilePhone::EIsdnNumberPlan;

            iMsgAttrib.iGsmServiceCentre.iTypeOfNumber =
                RMobilePhone::EInternationalNumber;

            iMsgAttrib.iGsmServiceCentre.iTelNumber = messagecenternumber;
            iMsgAttrib.iMore = (TBool)(EFalse);

            MsgParameters = 0;
            MsgParameters |= SmsMsgTypeSubmitOrSubmitR;
            MsgParameters |= SmsMsgVPabsolute;

            TBuf8<7> ValidityPeriod;
            TUint8 validity[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
            ValidityPeriod.Append(&validity[0],sizeof(validity));
            }

        else
            {
            TUint8 UD[36]=
                {
                'S','e','c','o','n','d',' ','m','e','s','s','a','g','e',
                ' ','T','e','s','t',' ','S','e','n','d',' ','t','w','o',
                ' ','m','e','s','s','a','g','e'
                };
            source.Append(&UD[0],sizeof(UD));


            //CASE: SEND SMS MESSAGE
            iMsgAttrib.iFlags = (RMobileSmsMessaging::KSmsDataFormat +
                                 RMobileSmsMessaging::KGsmServiceCentre +
                                 RMobileSmsMessaging::KRemotePartyInfo +
                                 RMobileSmsMessaging::KMoreToSend);

            // TSY takes now SC address from front of the TPDU
            iMsgAttrib.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;
            iMsgAttrib.iGsmServiceCentre.iNumberPlan =
                RMobilePhone::EIsdnNumberPlan;

            iMsgAttrib.iGsmServiceCentre.iTypeOfNumber =
                RMobilePhone::EInternationalNumber;

            iMsgAttrib.iGsmServiceCentre.iTelNumber = messagecenternumber;
            iMsgAttrib.iMore = (TBool)(EFalse);
            MsgParameters = 0;
            MsgParameters |= SmsMsgTypeSubmitOrSubmitR;
            MsgParameters |= SmsMsgVPabsolute;

            //TP-Status-Report-Request
            const TUint StatusReportRequested = 0x20; // binary 00100000
            MsgParameters |= StatusReportRequested;
            }

        TBuf8<140> UserData;

        TUint16 byte_count = (TUint16)source.Length();
        GsmLibCharConversion( TKGsmLibConvertToGsmFormat, source, byte_count );
        GsmLibSmsPackMessage( UserData, source, byte_count );
        TUint16 charCnt = (TUint16)( (UserData.Length()*8) / 7 );

        if ( ( ( ( iGsmMes.Length()*8 ) % 7 ) == 0) &&
            charCnt && charCnt <= iGsmMes.Length() )
            {
            if ( UserData[charCnt-1] == 0 )
            // if the last char is NULL, Actual charCnt is charCnt-1
                {
                charCnt--;
                }
            }

        TBuf<22> DEtemp = number;//_L("0405431828");
        TPtrC16 DEnumber = DEtemp;
        TBuf8<12> DestinationAddress;
        GsmConvUnicodeTo0340Addr(
            2,   //TON national number
            1,   //NPI isdn numbering plan
            DestinationAddress,
            DEnumber );

        msgdata.SetLength(0);                   //reset buffer
        msgdata.Append(MsgParameters);          //Message params
        msgdata.Append(0x10);                   //TP-Message-Reference
        msgdata.Append(DestinationAddress);     //TP-Destination-Address
        msgdata.Append(0x00);                   //TP-Protocol-Identifier
        msgdata.Append(0x02);                   //TP-Data-Coding-Scheme (8bit)
        msgdata.Append(0xA7);                   //TP-Validity-Period
        msgdata.Append(charCnt);                //TP-User-Data-Length
        msgdata.Append(UserData);               //TP-User-Data
        // MO SMS bearer setting
        //Create package
        CMmDataPackage package;

        //structure for all sms parameters and data
        TSendSmsDataAndAttributes sendData;

        sendData.iAttributes = &iMsgAttrib;
        sendData.iMsgData = &msgdata;
        sendData.iIpc = EMobileSmsMessagingSendMessage;

        //Pack parameters
        package.PackData( &sendData );

        ret = HandleRequestL( EMobileSmsMessagingSendMessage, &package );

        if ( KErrNone == ret )

            {
            _LIT8( KSendOk, "Send SMS Message ok" );
            iSMSLog->Log((TDesC8)KSendOk );
            iSMSLog->Log( _L("Send SMS Message ok"));
            }
        else
            {
            _LIT8( KSendFailed, "Send SMS Message failed: %d" );
            iSMSLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotFound;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::ReadSMSMessage
// Reading SMS message for slot
// -----------------------------------------------------------------------------
//
TInt CSMS::ReadSMSMessageL( CStifItemParser& aItem )
    {
    _LIT8(KReadSMSMessage, "SMS::ReadSMSMessage");
    iSMSLog->Log((TDesC8)KReadSMSMessage);
    TInt ret (KErrNotFound);
    TInt slot;
    TPtrC16 msgStatus;
    TPtrC16 numberingPlan;
    TPtrC16 typeOfNumber;
    TPtrC   serviceCentreAddress;
    TPtrC16 isClass2;
    TInt   tpdu;

    aItem.GetNextInt( slot );
    iSmsMsg.iLocation = slot;

    aItem.GetNextString( msgStatus );
    if ( msgStatus == ( _L("KSimSmsMtNotRead" ) ) )
        {
        iSmsMsg.iMessageStatus = KSimSmsMtNotRead;
        }
    else if ( msgStatus == ( _L("KSimSmsMtRead" ) ) )
        {
        iSmsMsg.iMessageStatus = KSimSmsMtRead;
        }
    else
        {
        iSmsMsg.iMessageStatus = KSimSmsNotPresent;
        }

    aItem.GetNextString( numberingPlan );
    if ( numberingPlan == ( _L("EIsdnNumberPlan" ) ) )
        {
        iSmsMsg.iMobileScNPI  = RMobilePhone::EIsdnNumberPlan;
        }
     else
        {
        iSmsMsg.iMobileScNPI  = RMobilePhone::EUnknownNumberingPlan;
        }

    aItem.GetNextString( typeOfNumber );
    if ( typeOfNumber == ( _L( "EInternationalNumber" ) ) )
        {
        iSmsMsg.iMobileScTON = RMobilePhone::EInternationalNumber;
        }
     else
        {
        iSmsMsg.iMobileScTON = RMobilePhone::EUnknownNumber;
        }

    aItem.GetNextString( serviceCentreAddress );
    iSmsMsg.iServiceCentre = serviceCentreAddress;


    iSmsMsg.iSmsClass2 = EFalse;
    aItem.GetNextString( isClass2 );
    if ( isClass2 == ( _L( "CLASS2" ) ) )
        {
        iSmsMsg.iSmsClass2 = ETrue;
        }

    while ( !aItem.GetNextInt( tpdu ) )
        {
        iSmsMsg.iSmsMsg.Append( tpdu );
        }

    CMmDataPackage package;
    package.PackData( &slot );
    ret= HandleRequestL( EMobilePhoneStoreRead, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KOk, "Read SMS Message ok: %d" );
        iSMSLog->Log((TDesC8)KOk );
        }
    else
       {
       _LIT8( KFail, "Read SMS Message fails: %d" );
       iSMSLog->Log((TDesC8)KFail, ret );
       }

   return ret;
   }


// -----------------------------------------------------------------------------
// CSMS::CompleteReadSMSMessage
// Complete ReadSMSMessage method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteReadSMSMessage(
    TInt aResult, //Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // checking results and unpacking message
    if ( KErrNone == aResult )
        {
        TSmsMsg smsMsg;
        aDataPackage->UnPackData( smsMsg );
        if ( ( smsMsg.iLocation != iSmsMsg.iLocation ) ||
             ( smsMsg.iMessageStatus != iSmsMsg.iMessageStatus ) ||
             ( smsMsg.iMobileScNPI != iSmsMsg.iMobileScNPI ) ||
             ( smsMsg.iMobileScTON != iSmsMsg.iMobileScTON ) ||
             ( smsMsg.iServiceCentre != iSmsMsg.iServiceCentre ) ||
             ( smsMsg.iSmsMsg != iSmsMsg.iSmsMsg ) ||
             ( smsMsg.iSmsClass2 != iSmsMsg.iSmsClass2 ) )
            {
            aResult = KErrGeneral;
            }
        }

    if ( KErrNone == aResult )
        {
        _LIT8( KOk, "Complete Read SMS Message ok: %d" );
        iSMSLog->Log((TDesC8)KOk );
        }
    else
        {
        _LIT8( KFail, "Compelte Read SMS Message Failed: %d" );
        iSMSLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSMS::DeleteSMSMessage
// Deletes SMS messages
// -----------------------------------------------------------------------------
//
TInt CSMS::DeleteSMSMessageL( CStifItemParser& aItem )
    {
    _LIT8(KDeleteSMSMessage, "SMS::DeleteSMSMessage");
    iSMSLog->Log((TDesC8)KDeleteSMSMessage);
    TInt ret (KErrGeneral);
    TInt slot;
    aItem.GetNextInt(slot);

    CMmDataPackage package;
    package.PackData( &slot );

    ret= HandleRequestL( EMobilePhoneStoreDelete, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Delete SMS Message ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Delete SMS Message fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteDeleteSMSMessage
// Complete DeleteSMSMessage method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteDeleteSMSMessage( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Delete SMS Message result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::DeleteAllSMSMessages
// Deletes all SMS messages
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSMS::DeleteAllSMSMessagesL( CStifItemParser& )
    {
    _LIT8(KDeleteAllSMSMessages, "SMS::DeleteAllSMSMessages");
    iSMSLog->Log((TDesC8)KDeleteAllSMSMessages);
    TInt ret (KErrGeneral);

    ret = HandleRequestL( EMobilePhoneStoreDeleteAll );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Delete All SMS Messages ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Delete All SMS Messages fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteDeleteAllSMSMessages
// Complete DeleteSMSMessage method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteDeleteAllSMSMessages( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Delete All SMS Messages result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::SetMoSmsBearer
// Sets MO SMS Bearer (packet data or GSM)
// -----------------------------------------------------------------------------
//
TInt CSMS::SetMoSmsBearerL( CStifItemParser& aItem )
    {
    TInt ret (KErrGeneral);

    TInt bearerInt;
    aItem.GetNextInt(bearerInt);

    _LIT8(KSetMoSmsBearer, "SMS::SetMoSmsBearer, bearer=%d");
    iSMSLog->Log((TDesC8)KSetMoSmsBearer, bearerInt);

    RMobileSmsMessaging::TMobileSmsBearer bearer =
        static_cast< RMobileSmsMessaging::TMobileSmsBearer >( bearerInt );

    CMmDataPackage package;
    package.PackData( &bearer );

    ret = HandleRequestL( EMobileSmsMessagingSetMoSmsBearer, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Set MO SMS Bearer ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Set MO SMS Bearer fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteSetMoSmsBearer
// Completes SetMoSmsBearer
// -----------------------------------------------------------------------------
//
void CSMS::CompleteSetMoSmsBearer( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Set MO SMS Bearer result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::WriteSMSMessage
// Write message to qnted slot
// -----------------------------------------------------------------------------
//
TInt CSMS::WriteSMSMessageL( CStifItemParser& aItem )
    {
    _LIT8(KWriteSMSMessage, "SMS::WriteSMSMessage");
    iSMSLog->Log((TDesC8)KWriteSMSMessage);

    TInt ret (KErrNotFound);
    TInt slot;
    TPtrC number;
    TPtrC servicecenternumber;
    aItem.GetNextString(number);
    aItem.GetNextString(servicecenternumber);
    aItem.GetNextInt(slot);

    // TName namee = _L( "SMS_SIM" );
    // TBuf8<12> serviceCenterAddress;
    TBuf8<12> destinationAddress;
    TBuf8<200> omaPDU;

    //TUint msgParameters( 0 );
    RMobileSmsStore::TMobileGsmSmsEntryV1 smsentry;
    RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsentryPckg( smsentry );

    TBuf8<7> validityperiod;
    TUint8 validity[7]={ 0x01,0x02,0x03,0x04,0x05,0x06,0x07 };
    validityperiod.Append(&validity[0],sizeof(validity));

    TUint MsgParameters( 0 );
    const TUint SmsMsgTypeSubmitOrSubmitR   = 0x01;
    const TUint SmsMsgVPabsolute            = 0x34;     // absolute format

    // TSY doesn't handle this information. Destination address goes in PDU.
    iMsgAttrib.iDestination.iTelNumber = number;

    iMsgAttrib.iDestination.iTypeOfNumber = RMobilePhone::ENationalNumber;
    iMsgAttrib.iDestination.iNumberPlan = RMobilePhone::EIsdnNumberPlan;

    TBuf8<160> source;

    if (-1 == slot)
        {
        TUint8 UD[145]={'t','e','s','t',' ','m','e','s','s','a','g','e',' ',
        'f','o','r',' ','S','M','S',' ','T','e','s','t',' ','C','a','s',
        'e',' ','T','C','-','S','M','S','-','0','1','6','b',' ','t','h',
        'i','s',' ','m','e','s','s','a','g','e',' ','i','s',' ',
        's','t','o','r','e','d',' ','i','n',' ','f','i','r','s','t',' ',
        'a','v','a','i','l','e','b','l','e',' ','s','l','o','t',' ','b',
        'y',' ','g','i','v','i','n','g',' ','s','l','o','t',' ','n','u',
        'm','b','e','r',' ','a','s',' ','-','1','*','*','*','*','*','*',
        '*','*','*','*','*','*','*','*','*','*','*',' ','l','e','n','g',
        't','h','=','1','4','9'};
        source.Append(&UD[0],sizeof(UD));
        }
    else
        {
        TUint8 UD[73]={'t','e','s','t',' ','m','e','s','s','a','g','e',' ',
        ' ','f','o','r',' ','t','e','s','t',' ','1','6','a',' ','i','n',
        'S','M','S',' ','T','e','s','t',' ','C','a','s','e','s',' ','*',
        'h','u','o','m','a','t','t','a','v','a','s','t','i',' ',
        '*','*','*','*',' ','L','e','n','g','t','h','=','7','5'};
        source.Append(&UD[0],sizeof(UD));
        }

    // If you want to convert user data to 7bit format:
    TBuf8<140> userdata;
    TUint16 byte_count = ( TUint16 )source.Length();
    GsmLibCharConversion(
        TKGsmLibConvertToGsmFormat, source, byte_count );
    GsmLibSmsPackMessage(
        userdata, source, byte_count );

    TBuf<23> detemp = number;
    TPtrC16 denumber = detemp;
    GsmConvUnicodeTo0340Addr(
        1,  //TON international number
        1,  //NPI isdn numbering plan
        destinationAddress,
        denumber );

    omaPDU.Append(0x24);                //Message params
    omaPDU.Append(destinationAddress);  //TP-Orginating Address
    omaPDU.Append(0x00);                //TP-Protocol-Identifier
    omaPDU.Append(0x00);                //TP-Data-Coding-Scheme
    omaPDU.Append(0x10);                //TP-Service Centre Time Stamp
    omaPDU.Append(0x80);                //TP-Service Centre Time Stamp
    omaPDU.Append(0x71);                //TP-Service Centre Time Stamp
    omaPDU.Append(0x41);                //TP-Service Centre Time Stamp
    omaPDU.Append(0x73);                //TP-Service Centre Time Stamp
    omaPDU.Append(0x14);                //TP-Service Centre Time Stamp
    omaPDU.Append(0x80);                //TP-Service Centre Time Stamp
    omaPDU.Append(byte_count);          //TP-User-Data-Length
    omaPDU.Append(userdata);            //TP-User-Data

    smsentry.iMsgData.Copy(omaPDU);

    smsentry.iMsgStatus = RMobileSmsStore::EStoredMessageRead;
    smsentry.iIndex = slot;
    smsentry.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
    smsentry.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smsentry.iServiceCentre.iTelNumber = servicecenternumber;

    //Create package
    CMmDataPackage package;

    RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* smsentryPckgPtr = &smsentryPckg;

    //Pack parameters
    package.PackData(&smsentryPckgPtr, &slot);

    ret = HandleRequestL( EMobilePhoneStoreWrite, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "Write SMS Message ok" );
        iSMSLog->Log((TDesC8)KResultOk );
        iSMSLog->Log( _L("Write SMS Message ok"));
        }
    else
        {
        _LIT8( KResultFailed, "Write SMS Message failed: %d" );
        iSMSLog->Log((TDesC8)KResultFailed, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteWriteSMSMessage
// Complete WriteSMSMessage method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteWriteSMSMessage(
    TInt aResult, //Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    // unpacking message
    TSmsMsg* smsMsg;
    aDataPackage->UnPackData( smsMsg );

    _LIT8(KResult, "Complete Write SMS Message: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::ReceiveMessage
// Cancels Receive Mode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSMS::ReceiveSMSMessageL( CStifItemParser& aItem )
   {
    _LIT8(KReceiveSMSMessages, "SMS::ReceiveSMSMessages");
    TInt ret (KErrNone);

    // Save SMS class type for comparison when receivin is completed.
    TPtrC string;
    aItem.GetNextString ( string );

    // Set NetworkSelection
    if ( _L("Class2") == string )
        {
        iSMSLog->Log( _L("Expect class 2 SMS") );
        iReceivedSmsClass2 = ETrue;
        }
    else
        {
        iSMSLog->Log( _L("Expect class 1 SMS") );
        iReceivedSmsClass2 = EFalse;
        }

    if ( KErrNone == ret )
        {
        _LIT8( KResultOk, "Receive SMS Message ok" );
        iSMSLog->Log((TDesC8)KResultOk );
        iSMSLog->Log( _L("Receive SMS Message ok"));
        }
    else
        {
        _LIT8( KResultFailed, "Receive SMS Message failed: %d" );
        iSMSLog->Log((TDesC8)KResultFailed, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSMS::CompleteReceiveSMSMessage
// Complete Receive SMS Message method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteReceiveSMSMessage(
    TInt aResult, //Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    _LIT8(KResult, "Complete Receive SMS Message result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    TBool smsInd;
    TSmsMsg* smsMsg;
    aDataPackage->UnPackData( smsInd, smsMsg );
    // Is the SMS Class received the same as stored in ReceiveSMSMessageL
    if ( smsMsg->iSmsClass2 != iReceivedSmsClass2 )
        {
        aResult = KErrGeneral;
        }
    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::ActivateSMSRouting
// Activate SMS routing
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSMS::ActivateSMSRoutingL( CStifItemParser& )
    {
    _LIT8(KActivateSMSRouting, "SMS::ActivateSMsRouting");
    iSMSLog->Log((TDesC8)KActivateSMSRouting);
    TInt ret (KErrNone);

    ret= HandleRequestL( EMmTsyActivateSmsRouting );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Activate SMS routing ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Activate SMS routing fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteActivateSMSRouting
// Complete Activate SMS Routing method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteActivateSMSRouting( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Activate SMS Routing result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::CompleteActivateSMSRouting
// Complete Activate SMS Routing method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteActivateSMSRouting(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8(KResult, "Complete Activate SMS Routing result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    TUint8 receptionStatus;
    aDataPackage->UnPackData( receptionStatus );
    if ( SMS_RECEPTION_INACTIVE == receptionStatus )
        {
        aResult = KErrGeneral;
        }
    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::AckSMS
// Acknowledge SMS message
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSMS::AckSMSL( CStifItemParser& )
    {
    _LIT8(KAckSMS, "SMS::Acknowledge SMS");
    iSMSLog->Log((TDesC8)KAckSMS);
    TInt ret (KErrNone);

    CMmDataPackage package;
    TDesC8 empty = KNullDesC8;
    package.PackData( &empty );
    ret= HandleRequestL( EMobileSmsMessagingAckSmsStored, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Acknowledge SMS ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Acknowledge SMS fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteAckSMS
// Complete AckSMS method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteAckSMS( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Ack SMS result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::DeactivateSMSRouting
// Deactivate SMS routing
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSMS::DeactivateSMSRoutingL( CStifItemParser& )
    {
    _LIT8(KDeactivateSMSRouting, "SMS::DeactivateSMSRouting");
    iSMSLog->Log((TDesC8)KDeactivateSMSRouting);
    TInt ret (KErrNone);

    ret= HandleRequestL( EMmTsyDeactivateSmsRouting );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Deactivate SMS routing ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Deactivate SMS routing fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteDeactivateSMSRouting
// Complete Deactivate SMS Routing method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteDeactivateSMSRouting( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Deactivate SMS Routing result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteDeactivateSMSRouting
// Complete Deactivate SMS Routing method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteDeactivateSMSRouting(
    TInt aResult,
    CMmDataPackage* aDataPackage )
    {
    _LIT8(KResult, "Complete Deactivate SMS Routing result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    TUint8 receptionStatus;
    aDataPackage->UnPackData( receptionStatus );
    if ( SMS_RECEPTION_ACTIVE == receptionStatus )
        {
        aResult = KErrGeneral;
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::ResumeSMS
// Resume SMS
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSMS::ResumeSMSL( CStifItemParser& )
    {
    _LIT8(KResumeSMS, "SMS::ResumeSMS");
    iSMSLog->Log((TDesC8)KResumeSMS);
    TInt ret (KErrNone);

    ret= HandleRequestL( EMobileSmsMessagingResumeSmsReception );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Resume SMS ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Resume SMS fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteResumeSMS
// Complete Resume SMS method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteResumeSMS( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Resume SMS result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::ReadAllSMSMessages
// Reading All SMS message for slot
// -----------------------------------------------------------------------------
//
TInt CSMS::ReadAllSMSMessageL ( CStifItemParser& aItem )
    {
    _LIT8(KReadAllSMSMessage, "SMS::ReadAllSMSMessage");
    iSMSLog->Log((TDesC8)KReadAllSMSMessage);
    TInt ret (KErrNotFound);
    TPtrC16 msgStatus;
    TPtrC16 numberingPlan;
    TPtrC16 typeOfNumber;
    TPtrC   serviceCentreAddress;
    TPtrC16 isClass2;
    TInt   tpdu;

    aItem.GetNextString( msgStatus );

    if ( msgStatus == ( _L("KSimSmsMtNotRead" ) ) )
        {
        iSmsMsg.iMessageStatus = KSimSmsMtNotRead;
        }
    else if ( msgStatus == ( _L("KSimSmsMtRead" ) ) )
        {
        iSmsMsg.iMessageStatus = KSimSmsMtRead;
        }
    else
        {
        iSmsMsg.iMessageStatus = KSimSmsNotPresent;
        }

    aItem.GetNextString( numberingPlan );
    if ( numberingPlan == ( _L("EIsdnNumberPlan" ) ) )
        {
        iSmsMsg.iMobileScNPI  = RMobilePhone::EIsdnNumberPlan;
        }
     else
        {
        iSmsMsg.iMobileScNPI  = RMobilePhone::EUnknownNumberingPlan;
        }

    aItem.GetNextString( typeOfNumber );
    if ( typeOfNumber == ( _L( "EInternationalNumber" ) ) )
        {
        iSmsMsg.iMobileScTON = RMobilePhone::EInternationalNumber;
        }
     else
        {
        iSmsMsg.iMobileScTON = RMobilePhone::EUnknownNumber;
        }

    aItem.GetNextString( serviceCentreAddress );
    iSmsMsg.iServiceCentre = serviceCentreAddress;


    iSmsMsg.iSmsClass2 = EFalse;
    aItem.GetNextString( isClass2 );
    if ( isClass2 == ( _L( "CLASS2" ) ) )
        {
        iSmsMsg.iSmsClass2 = ETrue;
        }

    while ( !aItem.GetNextInt( tpdu ) )
        {
        iSmsMsg.iSmsMsg.Append( tpdu );
        }

    ret= HandleRequestL( EMobilePhoneStoreReadAllPhase1 );

    if ( KErrNotFound == ret )
        {
        _LIT8( KOk, "Read All SMS Message ok: %d" );
        iSMSLog->Log((TDesC8)KOk );
        }
    else
        {
        _LIT8( KFail, "Read All SMS Message fails: %d" );
        iSMSLog->Log((TDesC8)KFail, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSMS::CompleteReadAllSMSMessage
// Complete ReadAllSMSMessage method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteReadAllSMSMessage(
    TInt aResult, //Completion result value
    CMmDataPackage* aDataPackage ) // pointer to DataPackage
    {
    // checking results and unpacking message
    if ( KErrNone == aResult )
        {
        TSmsMsg* smsMsg;
        aDataPackage->UnPackData( iSmspListArray );
        TInt count ( iSmspListArray->Count() );

        for ( TInt location( 0 ); location < count; location++ )
            {
            smsMsg = (TSmsMsg *)iSmspListArray->At( location );

            if ( ( smsMsg->iLocation != location + 1 ) ||
                 ( smsMsg->iMessageStatus != iSmsMsg.iMessageStatus ) ||
                 ( smsMsg->iMobileScNPI != iSmsMsg.iMobileScNPI ) ||
                 ( smsMsg->iMobileScTON != iSmsMsg.iMobileScTON ) ||
                 ( smsMsg->iServiceCentre != iSmsMsg.iServiceCentre ) ||
                 ( smsMsg->iSmsMsg != iSmsMsg.iSmsMsg ) ||
                 ( smsMsg->iSmsClass2 != iSmsMsg.iSmsClass2 ) )
                {
                aResult = KErrGeneral;
                break;
                }
            }
        }

    if ( KErrNone == aResult )
        {
        _LIT8( KOk, "Complete Read All SMS Message ok: %d" );
        iSMSLog->Log((TDesC8)KOk );
        }
    else
        {
        _LIT8( KFail, "Compelte Read All SMS Message Failed: %d" );
        iSMSLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::NackSMS
// Reject Receive SMS message
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSMS::NackSMSL( CStifItemParser& aItem )
    {
    _LIT8(KNackSMS, "SMS::NackSMS");
    iSMSLog->Log((TDesC8)KNackSMS);
    TInt ret (KErrNone);
    TInt errorvalue;

    aItem.GetNextInt( errorvalue );

    CMmDataPackage package;

    TDesC8 empty = KNullDesC8;
    package.PackData( &empty, &errorvalue);
    ret= HandleRequestL( EMobileSmsMessagingNackSmsStored, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "Reject receive SMS ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "Reject receive SMS fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteNackSMS
// Complete NackSMS method function.
// -----------------------------------------------------------------------------
//
void CSMS::CompleteNackSMS( TInt aResult ) //Completion result value
    {
    _LIT8(KResult, "Complete Reject receive SMS result: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::SIMMsgStorageInfo
// Get SIM SMS Storage info
// -----------------------------------------------------------------------------
//
TInt CSMS::SIMMsgStorageInfoL( CStifItemParser& /*aItem*/ )
    {
    TInt ret (KErrGeneral);

    _LIT8(KSIMMsgStorageInfo, "SMS::SIMMsgStorageInfo request");
    iSMSLog->Log((TDesC8)KSIMMsgStorageInfo);
    iSMSMsgStorageRequestOngoing = ETrue;

    ret = HandleRequestL( EMobileSmsMessagingGetMessageStoreInfo );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "request send ok" );
        iSMSLog->Log((TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "request failed(Cache not ready): %d" ); //SMS cache not ready
        iSMSLog->Log((TDesC8)KSendFailed, ret );
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CSIM::CompleteSIMMsgStorageInfo
// Complete SIMS SMS Storage Info
// -----------------------------------------------------------------------------
//
void CSMS::CompleteSIMMsgStorageInfo(
    TInt aResult,
    CMmDataPackage* aDataPackage
    )
    {
    // If specified requests are handled the signal is always sent.
    if ( !iHandleSpecifiedRequests )
        {
        if( iSMSMsgStorageRequestOngoing )
            {
            _LIT8( KResult, "CompleteSIMMsgStorageInfo result: %d" );
            iSMSLog->Log((TDesC8)KResult, aResult );

            TUint8 numOfLoc;
            TInt usedEntries;

            if( KErrNone == aResult )
                {
                aDataPackage->UnPackData ( numOfLoc, usedEntries );
                _LIT8( KResult, "CompleteSIMMsgStorageInfo: Available locations: %d, Used entries: %d" );
                iSMSLog->Log((TDesC8)KResult, (TInt)numOfLoc, (TInt)usedEntries );
                }
            else
                {
                _LIT8( KFail, "CompleteSIMMsgStorageInfo, error: %d" );
                iSMSLog->Log((TDesC8)KFail, aResult );
                }
            iSMSMsgStorageRequestOngoing = EFalse;
            Signal( aResult );
            }
        else
            {
            _LIT8( KResult, "CompleteSIMMsgStorageInfo - not requested from script (in boot)" );
            iSMSLog->Log((TDesC8)KResult );
            }
        }
    else
        {
        _LIT8( KResult, "CompleteSIMMsgStorageInfo / iHandleSpecifiedRequests is set / result: %d" );
            iSMSLog->Log((TDesC8)KResult, aResult );

        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CSMS::ReadSmsParameters
// Reads SMS Parameters from SIM.
// -----------------------------------------------------------------------------
//
TInt CSMS::ReadSmsParametersL( CStifItemParser& /*aItem*/ )
    {
    TInt ret ( KErrGeneral );

    _LIT8(KReadSmsParameters, "SMS::ReadSmsParameters");
    iSMSLog->Log(KReadSmsParameters );

    // no parameters
    ret = HandleRequestL( EMobileSmsMessagingGetSmspListPhase1 );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "ReadSmsParameters ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "ReadSmsParameters fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteReadSmsParameters
// Completes ReadSmsParameters
// -----------------------------------------------------------------------------
//
void CSMS::CompleteReadSmsParameters(
    TInt aResult, //Completion result value
    CMmDataPackage* aDataPackage )// SMS Parameters
    {
    if ( KErrNone == aResult )
        {
        CArrayPtrFlat<TSmsParameters>* params;
        aDataPackage->UnPackData( &params );

        TInt numOfSets = params->Count();
        _LIT8( KResultOk, "CSMS::CompleteReadSmsParameters: There are %d SMS parameter sets." );
        iSMSLog->Log((TDesC8)KResultOk, numOfSets );

        /* SMS parameters, from cmmsmsutility.h
        struct TSmsParameters
            {
            TUint8 iLocationNumber;
            TUint8 iParameterIndicator;
            TUint8 iProtocolId;
            TUint8 iDataCodingScheme;
            TUint8 iValidityPeriod;
            TBuf<KMaxAddressBufferSize> iServiceCenterAddress;
            RMobilePhone::TMobileTON iMobileScTON;
            RMobilePhone::TMobileNPI iMobileScNPI;
            TBuf<KMaxAddressBufferSize> iDestinationAddress;
            RMobilePhone::TMobileTON iMobileDeTON;
            RMobilePhone::TMobileNPI iMobileDeNPI;
            TBuf<RMobileSmsMessaging::KMaxSmspTextSize> iAlphaTagData;
            TBool iAlphaTagPresent;
            };
        */
        for ( TInt i = 0; i < numOfSets; i++ )
            {
            TSmsParameters* p = params->At( i );
            iSMSLog->Log( _L("SMS Parameter set %d:"), i);
            iSMSLog->Log( _L("  parameter indicator = 0x%x"), p->iParameterIndicator );
            iSMSLog->Log( _L("  protocol id         = 0x%x"), p->iProtocolId );
            iSMSLog->Log( _L("  data coding scheme  = 0x%x"), p->iDataCodingScheme );
            iSMSLog->Log( _L("  validity period     = 0x%x"), p->iValidityPeriod );
            iSMSLog->Log( _L("  service center address = %S"), &p->iServiceCenterAddress );
            iSMSLog->Log( _L("  destination address    = %S"), &p->iDestinationAddress );
            iSMSLog->Log( _L("  alpha tag              = %S"), &p->iAlphaTagData );

            if ( iSmsParameterCheck && ETestCaseSms008 == iTestCaseNumber )
                {
                TSmsParameters* refParams = iSmspListArray->At( i );

                if ( ( p->iParameterIndicator != refParams->iParameterIndicator ) &&
                    ( 0xff != refParams->iParameterIndicator ) )
                    {
                    aResult = KErrGeneral;
                    }
                if ( ( p->iProtocolId != refParams->iProtocolId ) &&
                    ( 0xff != refParams->iProtocolId ) )
                    {
                    aResult = KErrGeneral;
                    }
                if ( ( p->iDataCodingScheme != refParams->iDataCodingScheme ) &&
                     ( 0xff != refParams->iDataCodingScheme ) )
                    {
                    aResult = KErrGeneral;
                    }
                if ( ( p->iValidityPeriod != refParams->iValidityPeriod ) &&
                     ( 0xff != refParams->iValidityPeriod ) )
                    {
                    aResult = KErrGeneral;
                    }
                if ( ( p->iServiceCenterAddress != refParams->iServiceCenterAddress ) &&
                     ( 0 != refParams->iServiceCenterAddress.Compare( _L( "NA" ) ) ) )
                    {
                    aResult = KErrGeneral;
                    }
                if ( ( p->iDestinationAddress != refParams->iDestinationAddress ) &&
                        ( 0 != refParams->iDestinationAddress.Compare( _L( "NA" ) ) ) )
                    {
                    aResult = KErrGeneral;
                    }
                if ( ( p->iAlphaTagData != refParams->iAlphaTagData ) &&
                        ( 0 != refParams->iAlphaTagData.Compare( _L( "NA" ) ) ) )
                    {
                    aResult = KErrGeneral;
                    }
                }
            }
        iSMSLog->Log(_L8("CSMS::CompleteReadSmsParameters OK."));
        }
    else
        {
        _LIT8( KResultFailed, "Complete ReadSmsParameters Failed: %d" );
        iSMSLog->Log((TDesC8)KResultFailed, aResult );
        }

    iTestCaseNumber = ETestCaseNotDefined;
    Signal( aResult );
    }


// -----------------------------------------------------------------------------
// CSMS::StoreSmsParameters
// Stores SMS parameters to SIM. Parameters from script: SIM slot,
// SC number, alpha tag.
// -----------------------------------------------------------------------------
//
TInt CSMS::StoreSmsParametersL( CStifItemParser& aItem )
    {
    TInt ret ( KErrGeneral );

    _LIT8(KStoreSmsParameters, "SMS::StoreSmsParameters");
    iSMSLog->Log( KStoreSmsParameters );

    /* SMS parameters, from etelmm.h
    class TMobileSmspEntryV1 : public RMobilePhone::TMultimodeType
    The slot index where this SMSP entry is stored in the SIM's SMSP file.
        TInt iIndex;
    A bit-wise sum of the constants defined in TMobileSmspStoreValidParams, that
    specifies which parameters are valid within this entry.
        TUint32 iValidParams;
    Default protocol ID. Coded according to GSM 03.40.
        TUint8 iProtocolId;
    Default data coding scheme. Coded according to GSM 03.40.
        TUint8 iDcs;
    Default validity period, in relative format and coded on 8 bits as defined
    by GSM 03.40.
        TUint8 iValidityPeriod;
    Reserved for future use.
        TUint8 iReservedFiller;
    Default destination for outgoing SMS.
        RMobilePhone::TMobileAddress iDestination;
    Default service centre for outgoing SMS.
        RMobilePhone::TMobileAddress iServiceCentre;
    The alpha-tag associated with this SMSP entry, expressed as Unicode characters.
        TBuf<KMaxSmspTextSize> iText;
    */
    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    aItem.GetNextInt(smspEntry.iIndex); // SIM location

    TPtrC smsc;     // Number of SM Service Center
    aItem.GetNextString(smsc);
    smspEntry.iServiceCentre.iTelNumber = smsc;
    smspEntry.iServiceCentre.iTypeOfNumber = RMobilePhone::EInternationalNumber;
    smspEntry.iServiceCentre.iNumberPlan = RMobilePhone::EIsdnNumberPlan;

    TPtrC alphaTag; // Alpha tag
    aItem.GetNextString( alphaTag );
    smspEntry.iText = alphaTag;

    smspEntry.iValidParams = RMobileSmsMessaging::KSCAIncluded;

    CMmDataPackage package;
    package.PackData( &smspEntry );
    ret = HandleRequestL( EMobileSmsMessagingStoreSmspList, &package );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "StoreSmsParameters ok" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "StoreSmsParameters fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteStoreSmsParameters
// Completes StoreSmsParameters
// -----------------------------------------------------------------------------
//
void CSMS::CompleteStoreSmsParameters( TInt aResult ) //Completion result value
    {
    if ( KErrNone == aResult )
        {
        iSMSLog->Log(_L8("CSMS::Complete StoreSmsParameters OK."));
        }
    else
        {
        _LIT8( KResultFailed, "Complete StoreSmsParameters Failed: %d" );
        iSMSLog->Log((TDesC8)KResultFailed, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::SMSCacheInitialization
// SMS cache initialization
// -----------------------------------------------------------------------------
//
TInt CSMS::SMSCacheInitializationL( CStifItemParser& /* aItem */ )
    {
    TInt ret ( KErrGeneral );

    _LIT8(KSMSCacheInitialization, "SMS::SMSCacheInitialization");
    iSMSLog->Log( KSMSCacheInitialization );
    ret = HandleRequestL( EMmTsyBootNotifySimStatusReadyIPC );

    if ( KErrNone == ret )
        {
        _LIT8(KResultOk, "SMSCacheInitialization OK" );
        iSMSLog->Log((TDesC8)KResultOk);
        }
    else
        {
        _LIT8(KResultFail, "SMSCacheInitialization fails: %d" );
        iSMSLog->Log((TDesC8)KResultFail, ret );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteSMSCacheInitialization
// Completes SMSCacheInitialization
// -----------------------------------------------------------------------------
//
void CSMS::CompleteSMSCacheInitialization( TInt aResult ) //Completion result value
    {
    if ( KErrNone == aResult )
        {
        iSMSLog->Log(_L8("CSMS::Complete SMSCacheInitializationL OK."));
        }
    else
        {
        _LIT8( KResultFailed, "Complete SMSCacheInitializationL Failed: %d" );
        iSMSLog->Log((TDesC8)KResultFailed, aResult );
        }

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::CompleteMobilePhoneStoreGetInfo
// Completes MobilePhoneStoreGetInfo
// -----------------------------------------------------------------------------
//
void CSMS::CompleteMobilePhoneStoreGetInfo( TInt aResult ) //Completion result value
    {
    _LIT8( KResult, "Complete CompleteMobilePhoneStoreGetInfo: %d" );
    iSMSLog->Log((TDesC8)KResult, aResult );

    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSMS::GsmConvUnicodeTo0340Addr
// -----------------------------------------------------------------------------
//
void CSMS::GsmConvUnicodeTo0340Addr(
    TUint8 aTon,        //Type of number
    TUint8 aNpi,        //Numbering plan identification
    TDes8 &aDest,       //Service center address
    TDesC16 &aSource    //Telephone number
    )
    {
    if ( aSource.Length() != 0 )
        {
        GsmConvUnicodeToAddr( aTon, aNpi, aDest, aSource, ETrue );
        }
    else
        {
        // Address length of 0 is a special case
        aDest.Append( 0 );

        // Type-of-Address: TON = Unknown, NPI = ISDN/telephone num plan
        aDest.Append( 0x81 );
        }
    }

// -----------------------------------------------------------------------------
// ::GsmConvUnicodeToAddr
// Converts type of number, numbering plan identification and
// phone number given in unicode string to the GSM 03.40 or
// 04.11 standard
// -----------------------------------------------------------------------------
//
void CSMS::GsmConvUnicodeToAddr(
    TUint8 aTon,        //Type of number
    TUint8 aNpi,        //Numbering plan identification
    TDes8 &aDest,       //Service center address
    TDesC16 &aSource,   //Telephone number
    TBool a0340Address  //GSM 03.40 address
    )
    {
    //get address length
    TUint numberLength = aSource.Length();
    TUint destLength = numberLength;
    TUint i( 0 );

    /*  Solve type of address
        Type of Address field format is as follows:

           7      6     5     4      3      2       1       0    bit
        |     |      |     |     |      |       |       |       |
        |  1  | Type of number   | Numbering-plan-identification|
        |     |      |     |     |      |       |       |       |
    */

    //type of number is three bits from left and fourth bit must be '1'
    TUint8 typeOfNumber = TUint8( aTon | 0x08 );
    //move four bit to the left
    typeOfNumber = TUint8( typeOfNumber << 4 );
    //numbering plan is four bits from left
    TUint8 numberingPlan = TUint8( aNpi & 0x0F );
    //set type of number and numbering plan to the typeOfNumber variable
    typeOfNumber = TUint8( typeOfNumber | numberingPlan );

    if ( 5 == aTon )    //Alphanumeric
        {
        TBuf8<KMaxLengthOfAddressData> addressData;
        TUint8 amountOfsemiOctets( 0 );

        GsmUnicodeAddrToAlphanumericAddr( aSource, addressData,
            amountOfsemiOctets );

        //fill address length field
        if ( a0340Address )
            {
            //amount of used semi-octets in address data
            aDest.Append( amountOfsemiOctets );
            }
        else    //0411 address
            {
            //Actually alphanumeric address data for 0411 address is not
            //supported (see GSM 24.011 and 24.008)...
            //Length of address data + length of address type field
            aDest.Append( addressData.Length() + 1 );
            }

        //add typeOfNumber to the buffer
        aDest.Append( typeOfNumber );

        //add address data to the buffer
        aDest.Insert( 2, addressData );
        }
    else
        {
        //number can contain character '+'
        if ( '+' == aSource[i] )
            {
            i++;
            destLength--;

            // Maximum amount of digits is 20 (3GPP TS 23.040)
            if ( destLength > KMaxAmountOfDigits )
                {
                destLength = KMaxAmountOfDigits;
                numberLength = 21;
                }
            }
        else
            {
            // Maximum amount of digits is 20 (3GPP TS 23.040)
            if ( destLength > KMaxAmountOfDigits )
                {
                destLength = KMaxAmountOfDigits;
                numberLength = 20;
                }
            }

        // calculate address length in bytes
        TUint bufferLength = ( destLength + 1 )/2 + 2;

        //set buffer length
        aDest.SetLength( bufferLength );

        //add length to the buffer
        if ( a0340Address )
            {
            //amount of used semi-octets in address data
            aDest[0] = ( TUint8 )destLength;
            }
        else
            {
            //length of whole address data - length of address length field
            aDest[0] = ( TUint8 )( bufferLength - 1 );
            }

        //add typeOfNumber to the buffer
        aDest[1] = typeOfNumber;

        //needed variables
        TUint8 numLsb( 0 );
        TUint8 numMsb( 0 );
        TUint index( 2 );

        //add numbers to the buffer
        for ( ; ( i+1 ) < numberLength ; i+=2 )
            {
            numLsb = TUint8( aSource[i] - '0' );
            numMsb = TUint8( ( aSource[i+1] - '0' ) << 4 );

            aDest[index++] = TUint8( numLsb | numMsb );
            }

        //if numberLength is odd, last number is fixed with 0xF0
        if ( i < numberLength )
            {
            aDest[index] = TUint8( 0xF0 | ( aSource[i] - '0' ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSMS::GsmConvAddrToUnicode
// Converts GSM 03.40 or 04.11 address to type of number,
// numbering plan identification and phone number given in
// unicode string
// -----------------------------------------------------------------------------
//
void CSMS::GsmConvAddrToUnicode(
    TDes &aDest,                            //Service center address
    TDesC8 &aSource,                        //Telephone number
    RMobilePhone::TMobileTON& aTon,         //Type of number
    RMobilePhone::TMobileNPI& aNpi,         //Telephone number
    TBool a0340Address                      //GSM 03.40 address
    )
    {
    TUint addrDataLength( 0 );
    TBool halfs = ETrue;
    TUint index( 0 );
    TUint num2( 0 );

    //Get amount of digits in address data
    if ( a0340Address )
        {
        addrDataLength = aSource[index];
        }
    else    //04.11 address
        {
        addrDataLength = 2 * ( aSource[index] - 1 );
        }

    // Check if there are too many digits
    // Maximum amount of digits is 20 (3GPP TS 23.040)
    if ( addrDataLength > KMaxAmountOfDigits )
        {
        addrDataLength = KMaxAmountOfDigits;
        }

    index++;

    // Lets take type of number field
    TUint8 typeOfNum = aSource[index];
    GsmFindOutTonAndNpi( typeOfNum, &aTon, &aNpi );

    if ( ( aTon == RMobilePhone::EAlphanumericNumber )
        && ( addrDataLength != 0x00 ) )
        {
        //Calculate address length in bytes. If statement above check's that
        //addrDataLength is not zero.
        TUint addrDataLengthInBytes = ( addrDataLength + 1 ) / 2;

        //Get address data
        TPtrC8 addressData = aSource.Mid( 2, addrDataLengthInBytes);

        //amountOfCharsInAddrData = amount of used bits / length of char
        TUint amountOfCharsInAddrData = ( addrDataLength * 4 ) / 7;

        GsmAlphanumericAddrToUnicodeAddr( addressData, amountOfCharsInAddrData,
                                          aDest );
        }
    else
        {
        // Check if number is international number and addrDataLength is
        // bigger than zero
        if ( ( aTon == RMobilePhone::EInternationalNumber )
            && ( addrDataLength != 0x00 ) )
            {
            aDest.Append( '+' );
            }

        index++;

        while ( addrDataLength-- )
            {
            if ( halfs )
                {               // Lets take only four bits from right
                num2 = gsmSmsSemiOctetToChar( aSource[index] & 0x0F );
                aDest.Append( num2 );
                halfs = EFalse;
                }
            else
                {
                if ( ( ( aSource[index] >> 4 ) == 0x0F ) && !a0340Address )
                    {
                    //If 04.11 address contains odd number of digits,
                    //bits 5 to 8 of the last octet shall be filled with an
                    //end mark coded as "1111".
                    break;
                    }

                halfs = ETrue;  // Lets take only four bits from left
                num2 = gsmSmsSemiOctetToChar( aSource[index] >> 4 );
                aDest.Append( num2 );
                index++;
                }
            }
        }
    }

//-----------------------------------------------------------------------------
//    CSMS::GsmLibCharConversion
//
//    The character codes supported by the MS (the NMP character set) are
//    not the same as 'defined in GSM 03.40. The former is 8-bit set and
//    the latter is a 7-bit set. Therefore, the characters must be converted
//    by using this function. However, characters having the same code in
//    both sets are not converted.
//
//    Note that all handset characters are not supported by the GSM 03.40.
//    Such characters are converted to spaces or to the nearest substitute
//    (generally the acute/grave marks are removed).
//    Control characters and Japanese characters are converted to spaces.
//
//    For the NMP character set, see "Nokia Character Set (Final version)",
//    v1.0, in /user/cui2/.

//-----------------------------------------------------------------------------
//
void CSMS::GsmLibCharConversion(
    TUint aDirection, //directions
    TDes8& aString, //string
    TUint16 aLength ) //length
    {
    TUint16 j = 0;
    TUint16 i = 0;

    TBuf8<180> aDestination;

    if ( aDirection == TKGsmLibConvertToNmpFormat )
        {
        for ( i = 0; i < aLength; i++)
            {
            if ( aString[i] == 0xFF ) // This ends the string in SIM card
                {
                aDestination.Append( '\0' );    // This ends the string in
                                                // NMP format
                break;
                }

            aString[i] &= 0x7F;     // only 7 bits used in GSM character set

            //Conversion made using GSM 7 bit default alphabet extension
                //table.
                if ( ( aString[i] > 0x5A) && ( aString[i] < 0x5F ) ||
                    ( aString[i] > 0x7A ) && ( aString[i] < 0x7F ) )
                    {
                    for ( j = 0 ; j < TKGsmLibConvTableSize; j++ )
                        {
                        if ( aString[i] == TKGsmExtensionConvTable[j].
                        iHandsetChar )
                            {
                            aDestination.Append( 0x1B );//extension table flag
                            aDestination.Append(
                                TKGsmExtensionConvTable[j].iSmsChar );
                            break;
                            }
                        else if ( aString[i] == 0x20A0 ) //Euro
                            {
                            aDestination.Append( 0x65 );
                            }
                        }   // end of for loop

                    }

            // Conversion could be done only taking out the most significant
            // bit from Unicode characters between 0x25 - 0x5A or 0x61 - 0x7A.
            // We should be used KGsmLibConvTable if it is outside that are
            // or it is 0x40 ( @ ).
            else if ( ( aString[i] < 0x20 )
                || ( ( aString[i] > 0x5A ) && ( aString[i] < 0x61 ) )
                ||   ( aString[i] > 0x7A )
                ||   ( aString[i] == 0x40 ) )
                {

                for ( j = 0 ; j < TKGsmLibBidirTableSize; j++ )
                    {
                    if ( aString[i] == TKGsmLibConvTable[j].iSmsChar )
                        {
                        aDestination.Append(
                            TKGsmLibConvTable[j].iHandsetChar );
                        break;
                        }
                    }   // end of for loop
                }
            else
                {
                aDestination.Append( aString[i] );
                }
            }   // end of for loop

        }
    else // aDirection = KGsmLibConvertToGsmFormat
        {
        for ( i = 0; i < aLength; i++ )
            {
            if ( aString[i] == '\0' )   // End of string in NMP format
                {
                aDestination.Append( 0xFF );    // End of string in SIM
                }

            else if ( ( ( aString[i]  < 0x10 )  && ( aString[i] != 0x0A )
                    && ( aString[i] != 0x0D ) ) ||
                    ( ( aString[i] > 0x7F ) && ( aString[i] < 0xC0 ) ) )
                {
                // Convert to SPACE if control character or Japanese character
                //   NOTE (CR (0x0D) and LF (0x0A) are not converted.
                aDestination.Append( 0x20 );
                }
            else
                {
                //Conversion made using GSM 7 bit default alphabet extension
                //table.
                if ( ( aString[i] > 0x5A) && ( aString[i] < 0x5F ) ||
                    ( aString[i] > 0x7A ) && ( aString[i] < 0x7F ) )

                    {
                    for ( j = 0 ; j < TKGsmLibConvTableSize; j++ )
                        {
                        if ( aString[i] == TKGsmExtensionConvTable[j].
                        iHandsetChar )
                            {
                            aDestination.Append( 0x1B );// extension table flag
                            aDestination.Append(
                                TKGsmExtensionConvTable[j].iSmsChar );
                                break;
                            }
                        else if ( aString[i] == 0x20A0 ) //Euro
                            {
                            aDestination.Append( 0x65 );
                            }
                        }   // end of for loop
                    }

                // Conversion could be done only taking out the most significant
                // bit from Unicode characters between 0x25 - 0x5A or 0x61 - 0x7A.
                // We should be used KGsmLibConvTable if it is outside that are
                // or it is 0x40 ( @ ).

                else if ( ( aString[i] < 0x25 )
                    || ( ( aString[i] > 0x5A ) && ( aString[i] < 0x61 ) )
                    || ( aString[i] > 0x7A )
                    || ( aString[i] == 0x40 ) )
                    {
                    for ( j = 0; j < TKGsmLibConvTableSize; j++ )
                        {
                        if ( aString[i] == TKGsmLibConvTable[j].iHandsetChar )
                            {
                            aDestination.Append(
                                TKGsmLibConvTable[j].iSmsChar );
                            break;
                            }
                        }   // end of for loop
                    }
                else
                    {
                    aDestination.Append( aString[i] );
                    }
                }   // end of else
            }   // end of for loop
        }   // end of else
    aString.Copy( aDestination );
    }


//-----------------------------------------------------------------------------
//  CSMS::GsmLibSmsPackMessage
//  This function packs a given text string consisting of 7-bit
//  characters into 8 bit bytes in such a way that a 160-character
//  message takes only 140 bytes of memory after the compression.
//-----------------------------------------------------------------------------
//
TUint16 CSMS::GsmLibSmsPackMessage(
    TDes8 &aDest,//destination
    TDes8 &aSrc, //source
    TUint16 aByte_count ) //Byte counting
    {
    TUint16 si, di;           // Indexes
    TInt tmp_modulo;          // Temporary; to improve efficiency

    // Reset the indexes.
    si = 0;
    di = 0;
    tmp_modulo = di % 7;

    while ( si < aByte_count )
        {
        // Is this the last character of the message
        // If it is, just shift it to the right. Otherwise,
        // fill the rest of the byte with the bits of the next source character.

        if ( si == (aByte_count - 1) )
            {
            TUint num2 = (aSrc[si] >> tmp_modulo);
            aDest.Append(num2);
            }
        else
            {
            TUint num2 = (aSrc[si] >> tmp_modulo) |
            (aSrc[si + 1] << (7 - tmp_modulo));
            aDest.Append(num2);
            }

        di++;
        tmp_modulo = di % 7;

        // Check if the destination byte could take the entire source character.
        // In that case, the source character does not have to be divided and
        // the next source character can be taken.

        if ( tmp_modulo == 0)
            {
            si += 2;
            }
        else
            {
            si++;
            }
        }

    // Return the length of the coded message.
    return( di );
   }

// -----------------------------------------------------------------------------
// CSMS::GsmUnicodeAddrToAlphanumericAddr
// Converts Unicode characters to GSM 7-bit bit Default Alphabet
// characters (Defined in GSM 23.038). Note that all Unicode characters
// are not supported by the GSM 7-bit bit Default Alphabet set. Such
// characters are converted to spaces or to the nearest substitute
// (generally the acute/grave marks are removed). Control characters and
// special characters are converted to spaces. This method supports GSM
// 7-bit bit Default Alphabet extension table
// mechanism.

// After character conversion this method packs address data by calling
// GsmLibSmsPackMessage method*/
// -----------------------------------------------------------------------------
//
void CSMS::GsmUnicodeAddrToAlphanumericAddr(
    TDesC16& aSource,           // Source address data
    TDes8& aDestination,        // Destination data
    TUint8& aAmountOfSemiOctets // Amount of semi-octets
    )
    {
    TUint16 j = 0;
    TUint16 i = 0;

    TBuf8<RMobilePhone::KMaxMobileTelNumberSize> alphaChars;
    for ( i = 0; i < aSource.Length(); i++ )
        {
        if ( ( aSource[i] > 0x24 ) && ( aSource[i] < 0x5B ) &&
             ( aSource[i] != 0x40 ) ||
             ( aSource[i] > 0x60 ) && ( aSource[i] < 0x7B ) )
            {
            // Conversion could be done only taking out the most significant
            // bit from Unicode characters between 0x25 - 0x5A or 0x61 - 0x7A
            // excluding '@' (0x40).
            alphaChars.Append(aSource[i]);
            }
        else if ( ( aSource[i] > 0x5A) && ( aSource[i] < 0x5F ) ||
             ( aSource[i] > 0x7A ) && ( aSource[i] < 0x7F ) ||
             ( aSource[i] == 0x20AC ) )
            {
            //Conversion made using GSM 7 bit default alphabet extension
            //table. Characters: [,\,],^,{,|,},~,&euro
            for ( j = 0 ; j < KGsmLibConvTableSize; j++ )
                {
                if ( aSource[i] == KGsmExtensionConvTable[j].iUnicodeChar )
                    {
                    alphaChars.Append( 0x1B );// extension table flag
                    alphaChars.Append(
                        KGsmExtensionConvTable[j].iGsmChar );
                        break;
                    }
                }   // end of for loop
            }
        else
            {
            //Try to find character from conversion table.
            TBool notFound( ETrue );
            for ( j = 0; j < KGsmLibConvTableSize; j++ )
                {
                if ( aSource[i] == KGsmLibConvTable[j].iUnicodeChar )
                    {
                    alphaChars.Append(
                        KGsmLibConvTable[j].iGsmChar );
                    notFound = EFalse;
                    break;
                    }
                }   // end of for loop

            if( notFound )
                {
                //Couldn't found similar character. Convert to SPACE.
                alphaChars.Append( 0x20 );
                }
            }
        }

    TUint8 packedChars = GsmLibPackAlphanumericAddress
        ( aDestination, alphaChars );

    //How many unused bits packed address contains
    TUint8 unusedBits = (TUint8)( ( aDestination.Length() * 8 ) -
                                  ( packedChars * 7 ) );

    //How many semi-octets is used in packed address.
    if ( unusedBits >= 4 )
        {
        //Last semi-octet doesn't contain address data
        aAmountOfSemiOctets = (TUint8)( ( aDestination.Length() * 2 ) - 1 );
        }
    else
        {
        //All semi-octets are used
        aAmountOfSemiOctets = (TUint8)( aDestination.Length() * 2 );
        }
    }

// -----------------------------------------------------------------------------
// CSMS::GsmFindOutTonAndNpi
// Converts type of number and numbering plan identification
// information from the type of address parameter to the
// RMobilePhone::TMobileTON and RMobilePhone::TMobileNPI format
// -----------------------------------------------------------------------------
//
void CSMS::GsmFindOutTonAndNpi(
    TUint8 typeOfAddress,               //Type of address
    RMobilePhone::TMobileTON* ton,      //Type of number
    RMobilePhone::TMobileNPI* npi       //Numbering plan identification
    )
    {
    TUint8 numberingPlanIdentification = ( TUint8 )( typeOfAddress & 0x0F );
    TUint8 temp = ( TUint8 )( typeOfAddress >> 4 );
    TUint8 typeOfNumber = ( TUint8 )( temp & 0x07 );

    switch ( typeOfNumber )
        {
        case KInternationalNumber:
            {
            *ton = ( RMobilePhone::EInternationalNumber );
            break;
            }
        case KNationalNumber:
            {
            *ton = ( RMobilePhone::ENationalNumber );
            break;
            }
        case KNetworkSpecificNumber:
            {
            *ton = ( RMobilePhone::ENetworkSpecificNumber );
            break;
            }
        case KSubscriberNumber:
            {
            *ton = ( RMobilePhone::ESubscriberNumber );
            break;
            }
        case KAlphanumeric:
            {
            *ton = ( RMobilePhone::EAlphanumericNumber );
            break;
            }
        case KAbbreviatedNumber:
            {
            *ton = ( RMobilePhone::EAbbreviatedNumber );
            break;
            }
        default:
            {
            *ton = ( RMobilePhone::EUnknownNumber );
            break;
            }
        }

    switch ( numberingPlanIdentification )
        {
        case KIsdnTelephoneNumPlan:
            {
            *npi = ( RMobilePhone::EIsdnNumberPlan );
            break;
            }
        case KDataNumPlan:
            {
            *npi = ( RMobilePhone::EDataNumberPlan );
            break;
            }
        case KTelexNumPlan:
            {
            *npi = ( RMobilePhone::ETelexNumberPlan );
            break;
            }
        case KServiceCentreSpecificPlan1:
            {
            *npi = ( RMobilePhone::EServiceCentreSpecificPlan1 );
            break;
            }
        case KServiceCentreSpecificPlan2:
            {
            *npi = ( RMobilePhone::EServiceCentreSpecificPlan2 );
            break;
            }
        case KNationalNumPlan:
            {
            *npi = ( RMobilePhone::ENationalNumberPlan );
            break;
            }
        case KPrivateNumPlan:
            {
            *npi = ( RMobilePhone::EPrivateNumberPlan );
            break;
            }
        case KErmesNumPlan:
            {
            *npi = ( RMobilePhone::EERMESNumberPlan );
            break;
            }
        default:
            {
            *npi = ( RMobilePhone::EUnknownNumberingPlan );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSMS::GsmAlphanumericAddrToUnicodeAddr
// This method first decodes a address data that has been
// coded using the algorithm described in GSM 03.40 annex 2 by calling
// GsmLibUnpackAlphanumericAddress method. After that this method
// converts GSM 7-bit default alphabet characters to Unicode format.
// This method supports GSM 7-bit bit Default Alphabet extension table
// mechanism
// -----------------------------------------------------------------------------
//
void CSMS::GsmAlphanumericAddrToUnicodeAddr(
    TDesC8& aSource,        // Source address data
    TUint& aAmountOfChars,  // Amount of characters in address data
    TDes16& aDestination    // Destination data
    )
    {
    //Maximum length of address data is 10 bytes. These bytes can contain
    //11 packed 7-bit characters.
    TBuf8<(KMaxLengthOfAddressData + 1)> unpackedSrc;
    GsmLibUnpackAlphanumericAddress(aSource, aAmountOfChars, unpackedSrc);

    TUint8 unpackedLength = (TUint8)unpackedSrc.Length();
    TUint8 j( 0 );
    TUint8 i( 0 );

    for ( i = 0; i < unpackedLength; i++)
        {
        unpackedSrc[i] &= 0x7F;     // only 7 bits used in GSM character set

        if ( ( unpackedSrc[i] > 0x24 ) && ( unpackedSrc[i] < 0x5B ) &&
             ( unpackedSrc[i] != 0x40 ) ||
             ( unpackedSrc[i] > 0x60 ) && ( unpackedSrc[i] < 0x7B ) )
            {
            // Character codes in Unicode and GSM 7-bit default alphabet
            // are same between 0x25 - 0x5A and 0x61 - 0x7A
            // excluding 0x40.
            aDestination.Append(unpackedSrc[i]);
            }
        else if ( unpackedSrc[i] == 0x1B )
            {
            //0x1B is an escape code to the extension table. Code after escape
            //code should been found from extension table.

            i++;

            //It is possible that extension table contains escape code to
            //another extension table. Increase index i until we find something
            //else than another escape code.
            for (; i < unpackedLength; i++)
                {
                if ( unpackedSrc[i] != 0x1B )
                    {
                    //Found something else than escape code. Let's convert
                    //character from extension table to Unicode character.
                    TBool notFound( ETrue );

                    for ( j = 0 ; j < KGsmExtensionConvTableSize; j++ )
                        {
                        if ( unpackedSrc[i] ==
                            KGsmExtensionConvTable[j].iGsmChar )
                            {
                            aDestination.Append(
                                KGsmExtensionConvTable[j].iUnicodeChar );
                            notFound = EFalse;
                            break;
                            }
                        }

                    if ( notFound )
                        {
                        //Couldn't find character from extension table.
                        //Convert to SPACE.
                        aDestination.Append( 0x20 );
                        }

                    break;
                    }
                else
                    {
                    //Found escape code to another to another extension table.
                    //On receipt of this code, a receiving entity shall display
                    //a space until another extension table is defined.
                    aDestination.Append( 0x20 );
                    }
                }   // end of for loop
            }   // end of else if
        else
            {
            //Find character from conversion table.
            for ( j = 0; j < KGsmLibConvTableSize; j++ )
                {
                if ( unpackedSrc[i] == KGsmLibConvTable[j].iGsmChar )
                    {
                    aDestination.Append(
                        KGsmLibConvTable[j].iUnicodeChar );
                    break;
                    }
                }   // end of for loop
            }   // end of else
        }   // end of for loop
    }

// -----------------------------------------------------------------------------
// CSMS::GsmLibPackAlphanumericAddress
// This function packs a given text string consisting of 7-bit
// characters into 8 bit bytes in such a way that a 11-character
// address takes only 10 bytes of memory after the compression
// -----------------------------------------------------------------------------
//
TUint8 CSMS::GsmLibPackAlphanumericAddress(
    TDes8 &aDest,            // Packed address
    TDes8 &aSource           // Unpacked address
    )
    {
    TUint8 si( 0 );
    TUint8 di( 0 );
    TUint tempModulo;

    // Set tempModulo and byteCount
    tempModulo = di % 7;
    TUint8 byteCount = (TUint8)aSource.Length();

    // Maximum length of address data field is 10 bytes. 10 bytes can contain
    // 11 compressed 7-bit characters. Because of this maximum
    // amount of characters in alphanumeric address is 11.
    if ( byteCount > ( KMaxLengthOfAddressData + 1 ) )
        {
        byteCount = KMaxLengthOfAddressData + 1;
        }

    while ( si < byteCount )
        {
        // Is this the last character of the message
        // If it is, just shift it to the right. Otherwise,
        // fill the rest of the byte with the bits of the next
        // source character.

        if ( si == (byteCount - 1) )
            {
            TUint num2 = ( aSource[si] >> tempModulo );
            aDest.Append( num2 );
            }
        else
            {
            TUint num2 = ( aSource[si] >> tempModulo ) |
                ( aSource[si + 1] << ( 7 - tempModulo ) );
            aDest.Append( num2 );
            }

        di++;
        tempModulo = di % 7;

        // Check if the destination byte could take the entire source character.
        // In that case, the source character does not have to be divided and
        // the next source character can be taken.

        if ( tempModulo == 0)
            {
            si += 2;
            }
        else
            {
            si++;
            }
        }

    // Return the length of the coded message.
    return byteCount;
    }

// -----------------------------------------------------------------------------
// CSMS::GsmLibUnpackAlphanumericAddress
//  This function decodes a address that has been coded using
// the algorithm described in GSM 03.40 annex 2. The result string will
// consist of bytes that contain a 7-bit character each
// -----------------------------------------------------------------------------
//
void CSMS::GsmLibUnpackAlphanumericAddress(
    TDesC8 &aSource,        // Packed address data
    TUint aAmountOfChars,   // Amount of characters in address data
    TDes8 &aDest            // Unpacked address data
    )
    {
    TUint8 di( 0 );
    TUint8 si( 0 );
    TUint tempModulo;

    // Maximum length of address data field is 10 bytes. 10 bytes can contain
    // 11 compressed 7-bit characters. Because of this maximum
    // amount of characters in alphanumeric address is 11.
    if ( aAmountOfChars > ( KMaxLengthOfAddressData + 1 ) )
        {
        aAmountOfChars = KMaxLengthOfAddressData + 1;
        }

    // The location of the current 7-bit character determines the
    // action to be taken. Only every 7th character is not divided into
    // two bytes. All other characters will have to be contructed by
    // combining bits of two consequent bytes.

    for ( di = 0; di < aAmountOfChars; di++ )
        {
        TUint num2 = 0;
        tempModulo = di % 8;
        switch ( tempModulo )
            {
            case 0:
                num2 = aSource[si] & 0x7F;
                break;
            case 7:
                num2 = ( aSource[si] >> 1 ) & 0x7F;
                si++;
                break;
            default:
                num2  = aSource[si] >> ( 8 - tempModulo );
                num2 &= KGsmLibSmsUnpackMaskTable[ tempModulo ];
                num2 |= aSource[si+1] << ( tempModulo );
                num2 &= 0x7F;
                si++;
                break;
            }
        aDest.Append( num2 );
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
