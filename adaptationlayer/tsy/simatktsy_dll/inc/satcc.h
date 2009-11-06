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



#ifndef SATCC_H
#define SATCC_H


//  INCLUDES
#include <e32base.h>        // base class cbase
#include <etelsat.h>        // etel sat api


// CONSTANTS
const TUint8 KBccMaxSize = 0x10; // Maximum length for BCC
const TUint8 KStringMaxSize = 249; // Maximum length for SS and 
                                   // Call Destination address string
const TUint8 KUssdStringMaxSize = 160; // Maximum length for USSD string
const TUint8 KCallModeSize = 2; // Call Mode + Call Mode Info


// FORWARD DECLARATIONS
class TTlv;
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
class CSatCC : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        static CSatCC* NewL( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        * @since NCP 5.0
        */
        virtual ~CSatCC();


    private:    // Constructors

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatCC( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since NCP 5.0
        */
        void ConstructL();


    public: // New functions

        /**
        * Handles an incoming command from phonet
        * Called by Phonet receiver when it receives ISI msg.
        * @since NCP 3.1
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceivedL( const TIsiReceiveC& aIsiMessage );

        /*
        * Empties the arrays in case of refresh
        * @since Series60_ver 2.6
        * @return none
        */
        void ClearArraysForRefresh();

        /*
        *  Set internal flag according to EF-SST
        * @since Series60_ver 2.6
        * @param aStatus
        * @return none
        */
        void SetStatusOfUssdSupport( TBool aStatus );

        /**
        * Set CSatCC internal Ton and Npi
        * @since Series60_ver 2.6
        * @param aTonNpi
        * @return none
        */
        void SetTonNpi( const TUint8 aTonNpi );

    private: // Data structures

        struct TCallControl
            {
            // General with all resource control indications
            TUint8 iTransId;

            TUint8 iRecourceId;

            TUint8 iResourceSeqId;

            // Call specific
            TUint8 iCallId;

            TBuf8<KCallModeSize> iCallMode;

            TBuf8<KBccMaxSize> iBearerCapabilities;

            TUint8 iAddressType;

            // SS and Call specific
            TBuf8<KStringMaxSize> iString;

            // USSD specific
            TUint8 iUssdCodingInfo;
            TBuf8<KUssdStringMaxSize> iUssdString;
            };

    private: // New methods

        /**
        * add Location Information simple TLV to TTlv
        * @since Series60_ver 2.6
        * @param TLV to add location information
        * @ return none
        */
        void AddLocationInformationToTlv( TTlv& aTlv );

        /**
        * send (to sim) call control envelope for SS operation
        * @param aCcstruct structure containing data to be sent
        * @return phonet sender status
        */
        TInt SendSSEnvelope(const TCallControl& aCcstruct);

        /**
        * send (to sim) call control envelope for USSD operation
        * @param aCcstruct structure containing data to be sent
        * @return phonet sender status
        */
        TInt SendUSSDEnvelope( const TCallControl& aCcstruct );

        /**
        * send (to sim) call control envelope for Call operation
        * @param aCallControl structure containing data to be sent
        * @return phonet sender status
        */
        TInt SendCallEnvelope
            (
            const TIsiReceiveC& aIsiMessage,
            const TCallControl& aCallControl
            );

        /**
        * Send (to UICC) call control envelope for PDP Context Activation
        * @since NCP 5.1
        * @param aPdpCcEnvelopeTid: Transaction ID received in CC GPRS event
        * @param aPdpContextActivationParams: PDP Activation Parameters
        * @return phonet sender status
        */
        TInt SendPdpContextActivationEnvelope( const TUint8 aPdpCcEnvelopeTid,
            const TDesC8& aPdpContextActivationParams );

        /**
        * Handles UICC_CAT_RESP, which comes from UICC as a reply
        * to call control envelope.
        * @param aIsiMessage Received UICC_CAT_RESP
        * @return void
        */
        void UiccCatRespEnvelopeReceived( const TIsiReceiveC& aIsiMessage );

        /*
        * This methods returns ETrue when a Ussd string contains
        * only "*", "#", and the numbers 0-9.
        * @since Series60_ver 2.6
        * @param aUSSDString
        * @return true if ussd string contains valid values
        */
        TBool IsOnlyDigitsInUssd( TPtrC8 aUSSDString );

        /**
        * Search thought the iCCarray and finds the index of requested
        * CC event info
        * @since Series60_ver 3.0
        * @param aTid requested transaction ID
        * @return index of CC event or KErrNotFound
        */
        TInt GetArrayIndexById( const TInt &aTid );

        /**
        * Handles resource control request from modem Call server
        * @param aIsiMessage CALL_MODEM_RESOURCE_IND
        * @return none
        */
        void CallModemResourceInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates resource control response for modem Call server
        * @param aTcc internally stored call control structure
        * @param aResult call control result from (U)SIM
        * @param aAtkData possible additional response data provided by (U)SIM
        * @return none
        */
        void SendCallModemResourceReq(
            const TCallControl& aTcc,
            const TUint8 aResult,
            TPtrC8 aAtkData );

        /**
        * Handles resource control request from modem SS server
        * @param aIsiMessage SS_RESOURCE_CONTROL_IND
        * @return none
        */
        void SsResourceControlInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates resource control response for modem SS server
        * @param aTcc internally stored call control structure
        * @param aResult call control result from (U)SIM
        * @param aAtkData possible additional response data provided by (U)SIM
        * @return none
        */
        void SendSsResourceControlReq(
            const TCallControl& aTcc,
            const TUint8 aResult,
            TPtrC8 aAtkData );

        /**
        * Handles resource control request from modem GPDS server
        * @param aIsiMessage GPDS_RESOURCE_CONTROL_IND
        * @return none
        */
        void GpdsResourceControlInd( const TIsiReceiveC& aIsiMessage );

        /**
        * Creates resource control response for modem GPDS server
        * @param aTcc internally stored call control structure
        * @param aResult call control result from (U)SIM
        * @param aAtkData possible additional response data provided by (U)SIM
        * @return none
        */
        void SendGpdsResourceControlReq(
            const TCallControl& aTcc,
            const TUint8 aResult,
            TPtrC8 aAtkData );

    private: // Data

        // Array for storing structures
        RArray<TCallControl>*   iCallControlArray;

        // Pointers for messaging classes for internal communication
        CSatMessHandler*        iSatMessHandler;
        CTsySatMessaging*       iSatMessaging;

        // True if Service n°41 in EF-SST is supported.
        TBool                   iUssdTlvSupported;

        // True if TonNpi is resolved from SEND SS proactive commmand
        TBool                   iTonNpiPresent;

        // For setting TON/NPI for SendSSEnvelope
        TUint8                  iTonNpiForSS;

        // Storage to temporarily store response to call control envelope
        HBufC8* iEnvelopeResponseData;

        // CC result reported to cellmo
        TUint8 iCcResult;

        // call control status in (U)SIM card
        TBool iCallControlEnabled;

        // call control on GPRS status in (U)SIM card
        TBool iCallControlOnGPRSEnabled;
    };


#endif // SATCC_H


// End of file
