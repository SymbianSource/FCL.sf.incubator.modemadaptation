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



#ifndef SATMOSMSCTRL_H
#define SATMOSMSCTRL_H


//  INCLUDES
#include <e32base.h>    // base class cbase
#include <etelsat.h>    // etel sat api


// Message Type received in SMS_RESOURCE_REQ message
const TUint8 KSmsCommandType = 2;
const TUint8 KSmsSubmitType = 1;

// FORWARD DECLARATIONS
class CSatMessHandler;
class CTsySatMessaging;
class TIsiReceiveC;


// CLASS DECLARATION
/**
*  CSatMoSmsCtrl
*  Sat MO SMS control handling
*  @lib SIMATKTSY.LIB
*  @since Series 60 Series60_2.6
*/
class CSatMoSmsCtrl : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        static CSatMoSmsCtrl* NewL( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        * @since NCP 5.0
        */
        virtual ~CSatMoSmsCtrl();


    private:    // Constructors

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatMoSmsCtrl( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since NCP 5.0
        */
        void ConstructL();


    public: // New methods

        /**
        * Handles an incoming command from phonet
        * Called by Phonet receiver when it receives ISI msg.
        * @since NCP 5.0
        * @param aIsiMessage proactive command from phonet
        * @return TInt
        */
        TInt MessageReceived( const TIsiReceiveC& aIsiMessage );

        /*
        * Returns iIsMoSmsCtrlActivated
        * This is used by CTsySatMessaging::MoSmsControlReceived
        * @since Series 60 Series60_2.6
        */
        TBool IsActivated();

        /*
        * Set iIsMoSmsCtrlActivated to ETrue
        * Allows this object to process incoming Mo-Sm requests
        * @since Series 60 Series60_2.6
        */
        void Activate();

        /*
        * Set iIsMoSmsCtrlActivated to EFalse
        * Prevents this object to process incoming Mo-Sm requests
        * @since Series 60 Series60_2.6
        */
        void Deactivate();


    private: // New methods

        /*
        * Indication received from SMS server with the SMS parameters
        * of the SMS to be sent by the Mobile Equipment.
        * The response will tell whether the SMS can be sent
        * to network or not.
        * @since NCP 5.0
        * @param aIsiMessage coming from ISA SMS Server
        * @return none
        */
        void SmsResourceIndReceived( const TIsiReceiveC& aIsiMessage );

        /*
        * Send an envelope to SIM server in order to check wheter the SMS
        * can be sent or not. The response will be used to send a response
        * to SMS server
        * @since Series 60 Series60_2.6
        * @param aTraId transaction id
        * @param aAddressData1 RP address
        * @param aAddressData2 TP address
        * @return none
        */
        void SendMoSmsCtrlEnvelope( TUint8 aTraId, TDes8& aAddressData1,
            TDes8& aAddressData2 );

        /*
        * Handles UICC_CAT_RESP, which comes from SIM as a reply
        * to MO SMS control envelope.
        * @param aIsiMessage Received ISI message
        * @return success code of ISI message sending attempt
        */
        TInt UiccCatRespEnvelopeReceived( const TIsiReceiveC& aIsiMessage );

        /*
        * Extracts data from AtkSwDataNtf message data part
        * @since Series 60 Series60_2.6
        * @param aAtkData input
        * @param aAddr1 output, contains the RP address provided by the SIM
        * @param aAddr2 ouput, contains the TP address provided by the SIM
        * @param aAlphaId output, contains the alpha id provided by the SIM
        * @param aEmptyAlphaId output, ETrue if alpha id is present and empty
        * @return none
        */
        void ParseAtkSwDataNtf( TPtrC8& aAtkData, TDes8& aAddr1, TDes8& aAddr2,
            RSat::TAlphaId& aAlphaId, TBool& aEmptyAlphaId );

        /*
        * This method cleans the address data, so that all the byte found
        * after a 0xXF or a 0xFX are ignored and removed.
        * @param  aAddr Address data to be cleaned
        * @return None
        */
        void CleanAddressData( TDes8& aAddr );

        /*
        * This method checks the consistency of the data sent by the SIM to
        * the ME, when the call is allowed and modified.
        * @param aRPAddr SMSC RP address
        * @param aTPAddr  TP Destinatiion address
        * @return ETtrue if data are consistent, else EFalse
        */
        TBool VerifySimRespData( TDes8& aRPAddr, TDes8& aTPAddr );

        /*
        * This method prepare subblocks for SMS_RESOURCE_REQ IsiMessage
        * @param number number of subblocks
        * @param status SMS is Denied or allowed
        * @param data Actual data to me send in IsiMessage
        * @param address1 Changed Service centre address received from SIM
        * @pram address2 Changed destination address received from SIM
        */
        void FormSmsResourceReqSb( const TUint8 status,TDes8& data, const TDes8& address1, const TDes8& address2 );


    private: // Data

        // This object receives and sends messages to the external world
        // using the message handler and messaging classes.
        CSatMessHandler*    iSatMessHandler;
        CTsySatMessaging*   iSatMessaging;

        // Boolean to check whether a Sim response is expected by MO-SMS Control
        TBool               iWaitingForEnvelopeResp;

        // Boolean to check whether MO-SMS Control is activated.
        // This is used by CTsySatMessaging::MoSmsControlReceived,
        // in order to know if this object is in use or not
        TBool               iIsMoSmsCtrlActivated;

        // Save transaction id of the envelope, in order to check whether
        // the incoming SwDataNtf is for MO-SMS Control. This has to be
        // ANDed with iWaitingForEnvelopeResp.
        // If this test condition is not strong enough, then it is recommended
        // to use an array like in Call Control, see SatCC.cpp/h.
        TUint8              iMoSmsCtrlEnvelopeTransactionId;

        // Save the transaction id of the incoming Mo-Sm request, which is
        // re-used in the resp.
        // It is questionable whether or not another boolean should be used
        // to remind if there is already an ongoing request not completed while
        // a new request is received. There is a risk that this transaction id
        // gets overwritten. But most likely ISA SMS server doesn't send
        // the next request before the completion of the previous one.
        TUint8              iMoSmsCtrlReqTransactionId;

        // Save the sender object id of the MO-SM req, which is
        // re-used in the resp.
        // The type TUint8 is based on the assumption on having at maximum
        // 256 ISA servers. There is a chance that future products possess
        // more than 256 servers, and therefore this type TUint8 would be
        // too small.
        TUint8              iSenderObject;

        // Save the Sequence id of MO SMS Indication, which is reused in
        // the resp messgae.
        // the type is TUint8 is based on the value range defined in ISI SMS Server
        // Message document
        TUint8 iSequenceId;

        // Save the Resource id of MO SMS Indication, which is reused in
        // the resp messgae.
        // the type is TUint16 is based on the value range defined in ISI SMS Server
        // Message document
        TUint16 iResourceId;

        // Save the Destination address TPDU Type of MO SMS Indication, which is reused in
        // the resp messgae.
        // the type is TUint8 is based on the information from 3gpp document
        TUint8 iMessageType;

        // Save the SMSC address subblock of MO SMS Indication, which is reused in
        // the resp messgae if message is allowed without any change or disallowed.
        // The size of buffer is based on the value range defined in
        // ISI SMS Server message document
        TBuf8<256> iAddressSubblock;

        // Save the Destination address subblock of MO SMS Indication, which is reused in
        // the resp messgae if message is allowed without any change or disallowed.
        // The size of buffer is based on the value range defined in
        // ISI SMS Server message document
        TBuf8<256> iUserDataSubblock;
    };

#endif // SATMOSMSCTRL_H


// End of file
