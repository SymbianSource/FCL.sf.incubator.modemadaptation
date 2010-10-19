/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef SATDATADDL_H
#define SATDATADDL_H

//  INCLUDES
#include <e32base.h>    // base class cbase


// CONSTANTS
// Maximum number of Cbmids accepted by SMS server is 20
const TUint8 KMaxCbmids = 20;


// FORWARD DECLARATIONS
class CSatMessHandler;
class CTsySatMessaging;
class TIsiReceiveC;


// CLASS DECLARATION
/**
*  Sat Call control handling
*
*  @lib simatktsy.lib
*  @since Series60_ver 2.6
*/
class CSatDataDownload : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        static CSatDataDownload* NewL( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        * @since NCP 5.0
        */
        virtual ~CSatDataDownload();

        /**
        * Handles UICC_CAT_RESP, which comes from UICC as a reply
        * to data download envelope.
        * @param aIsiMessage Received UICC_CAT_RESP
        */
        void UiccCatRespEnvelopeReceived( const TIsiReceiveC& aIsiMessage );


    private:    // Constructors

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatDataDownload( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since NCP 5.0
        */
        void ConstructL();


    public: // New functions

        /**
        * Handles an incoming command from phonet
        * Called by CSatMessHandler::SatMessageReceived when it receives
        * ISI msg.
        * @since NCP 5.0
        * @param aIsiMessage proactive command from phonet
        * @return none
        */
        void MessageReceivedL( const TIsiReceiveC& aIsiMessage );

        /*
        * Stores the status fo SMS PP DD Support by SIM
        * @since NCP 5.0
        * @param TBool aStatus SMS PP DD Support status
        * @return None
        */
        void SmsPpDlSupported( TBool aStatus );


    private: // Class specific enumeration

        /*
        * SMS PP DD TP Failure Causes from TS 23.040
        */
        enum TTpFailure
            {
            ENone                   = 0x00,
            EStorageFull            = 0xD0,
            EStorageNotSupported    = 0xD1,
            EMsError                = 0xD2,
            EMemoryFull             = 0xD3,
            ESatBusy                = 0xD4,
            ESatDlError             = 0xD5
            };


    private: // Methods

        /**
        * Handles cell broadcast indication
        * @since NCP 3.1
        * @param aIsiMessage ISI message received by Phonetsy
        * @return None
        */
        void CellBroadcastReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Handles sms point-to-point indication
        * @since NCP 3.1
        * @param aIsiMessage ISI message received by Phonetsy
        * @return None
        */
        void SmsSimMsgIndReceivedL( const TIsiReceiveC& aIsiMessage );

        /**
        * Build an SMS RECEIVE MSG  report to SMS server
        * according to routing result.
        * @since NCP 5.1
        * @param aTpFailure Tp Failure code to be included in report
        * @param aUserData User data that shall be included in report's
        * optional fields.
        * @return None
        */
         void BuildSimMsgReport( const TTpFailure aTpFailure,
            const TDesC8& aUserData );

        /*
        * Send envelope for cell broadcast download
        * @since Series60_ver 2.6
        * @param aTransId transaction id
        * @param aPdu coming from the TsySatMessaging
        * @return None
        */
        void SendCellBroadcastDdlEnvelope( TUint8 aTransId, TDesC8& aPdu );

        /*
        * Send envelope for Sms-Pp data download
        * @since Series60_ver 2.6
        * @param aTransId transaction id
        * @param aSmsScAddress Address of SMSC to be included in envelope
        * @param aPdu SMS PDU to be include in envelope
        * @return None
        */
        void SendSmsPpDdlEnvelope( TUint8 aTransId, TDesC8& aSmsScAddress,
            TDesC8& aPdu );


    private: // DSata

        // Pointer to CSatMesshandler instance
        CSatMessHandler*        iSatMessHandler;

        // Pointer to CSatMessaging instance
        CTsySatMessaging*       iSatMessaging;

        // Protocol Identifier to be sent in the report to Sms server
        // to acknowledge a Sms-Pp message
        TUint8                  iSmsPpProtocolId;

        // Data Coding Scheme to be sent in the report to Sms server
        // to acknowledge a Sms-Pp message
        TUint8                  iSmsPpDcs;

        // Transaction Id that is sent with envelope relative
        // to Sms-Pp Data Download message notification.
        TUint8                  iSmsPpTransactionId;

        // Boolean to check whether a Sim response is expected by Sms-Pp
        // Data Download
        TBool                   iSmsPpDdOngoing;

        // Is the SMS PP DD supported by SIM or not.
        TBool                   iSmsPpDdSupported;
    };

#endif // SATDATADDL_H

// End of file
