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



#ifndef SATNOTIFICATIONS_H
#define SATNOTIFICATIONS_H


//  INCLUDES
#include <e32base.h>    // base class cbase
#include <etelsat.h>    // etel sat response packages
#include <et_tsy.h>     // type declarations for tsy

// CONSTANTS
const TUint8 KRefreshMasterFileHeader             = 0x3F;


// FORWARD DECLARATIONS
class CSatMessHandler;
class CTsySatMessaging;
class TIsiReceiveC;
class CBerTlv;
class CTlv;

// CLASS DECLARATION
/**
*  Sat notifications base class.
*  Base class for SAT command notification classes.
*
*  @lib SIMATKTSY.LIB
*  @since Series 60 Series60_2.6
*/
class CSatNotificationsBase : public CBase
    {
    protected:    // Constructors

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotificationsBase( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );


    public:  // Destructor

        /**
        * Destructor.
        * @since NCP 5.0
        */
        virtual ~CSatNotificationsBase();


    public:     // Pure virtuals

        /**
        * Pure virtual method for derived classes. Handles PCmd ISI messages
        * from phonet which is an interface to CellMo software.
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        virtual void MessageReceived( const TIsiReceiveC& aIsiMessage ) = 0;

        /**
        * Pure virtual method for derived classes. Etel SAT Notification
        * request.
        * @since Series 60 Series60_2.6
        * @param aReqHandle notify request handle. The request handle
        * has to be non-null (.i.e pending request) in order to process
        * an incoming PCmd ISI message
        * @param aDataPtr pointer to a descriptor. Usually the pointer is
        * casted to a suitable ETel SAT data package in order to access
        * structured data fields and parameters. This is the first step
        * in the adaptation process, CellMo to Symbian.
        * @return none
        */
        virtual void Notify( const TTsyReqHandle aReqHandle,
            TDes8* aDataPtr = NULL ) = 0;

    public:     // New methods

        /**
        * Cancels notification request.
        * @since Series 60 Series60_2.6
        * @param aReqHandle notify request handle
        * @return status
        */
        TInt CancelNotification( const TTsyReqHandle aReqHandle );

        /**
        * Virtual method for derived classes. Handles the
        * terminal response
        * @since NCP 5.0
        * @param aRsp Pointer to a descriptor. Usually the pointer is
        * casted to a suitable ETel SAT data package in order to access
        * structured data fields and parameters. This is the first step
        * in the adaptation process, Symbian to CellMo. The terminal
        * response is reshaped into an ISI message.
        * @return status
        */
        virtual TInt TerminalResponse( TDes8* aRsp );

        /**
        * Return command details of the current proactive command
        * @since Series 60 Series60_2.6
        * @param none
        * @return reference to a descriptor containing the PCmd's command
        * details TLV.
        */
        TDes8& GetCmdDetails();

        /**
        * Return the transaction identifier of the current PCmd ISI message.
        * The transaction identifier is a CellMo specific value.
        * This value cannot be used by Etel clients.
        * @since Series 60 Series60_2.6
        * @param none
        * @return TUint8 Transaction Id value (0-255)
        */
        TUint8 GetTransactionId();


    protected: // New methods

        /**
        * Returns TsyReqhandle to the caller
        * @since Series 60 Series60_2.6
        * param none
        * @return TsyReqHandle
        */
        TTsyReqHandle TsyReqHandle();

        /**
        * Completes the notify request to Etel
        * @since Series 60 Series60_2.6
        * @param aError possible error code
        * @return none
        */
        void CompleteRequest( const TInt aError );


    protected: // Private nested class

        // CLASS DECLARATION
        /**
        *  Protected Sat notifications nested class.
        *  Contains the allowed general results for the specific command
        *
        *  @since NCP 5.1
        */
        class TAllowedResults
            {
            public:

                /**
                * C++ default constructor.
                * @since NCP 5.1
                */
                TAllowedResults();

                /**
                * Equals-operator for TAllowedResults-class. Test wether
                * the provided general result is allowed for the command
                * or not.
                * @since NCP 5.1
                * @param aResult Result to be tested
                * @return ETrue if allowed, EFalse if not.
                */
                TBool operator==( const RSat::TPCmdResult aResult ) const;

                /**
                * Nonequals-operator for TAllowedResults-class. Test wether
                * the provided general result is not allowed for the command
                * or not.
                * @since NCP 5.1
                * @param aResult Result to be tested
                * @return ETrue if not allowed, otherwise EFalse.
                */
                TBool operator!=( const RSat::TPCmdResult aResult ) const;

                /**
                * Assignment operator for TAllowedResults-class. Provided
                * general result is allowed for command.
                * (results = RSat::KSuccess)
                * @since NCP 5.1
                * @param aResult Result to be allowed
                * @return TAllowedResults-class
                */
                TAllowedResults& operator=( const RSat::TPCmdResult aResult );

                /**
                * Increment operator for TAllowedResults-class. Provided
                * general result is allowed for command.
                * (results += RSat::KPartialComprehension)
                * @since NCP 5.1
                * @param aResult Result to be allowed
                * @return TAllowedResults-class
                */
                TAllowedResults& operator+=( const RSat::TPCmdResult aResult );

                /**
                * Increment operator for TAllowedResults-class. Results allowed
                * by the provided results-class are appended.
                * (results += otherResults)
                * @since NCP 5.1
                * @param aResults Results to be allowed
                * @return TAllowedResults-class
                */
                TAllowedResults& operator+=( const TAllowedResults aResults );

                /**
                * Increment operator for TAllowedResults-class. Provided
                * general result is allowed for command.
                * (results = results + RSat::KSuccess)
                * @since NCP 5.1
                * @param aResult Result to be allowed
                * @return TAllowedResults-class
                */
                TAllowedResults& operator+( const RSat::TPCmdResult aResult );


            private:    // Methods

                /**
                * Creates 64-bit field from the provided general result
                * @since NCP 5.1
                * @param aResult Result to be converted
                * @return Bit field representing the general result
                */
                inline TUint64 CreateBitField( const RSat::TPCmdResult aResult ) const;

            private:    // Members

                // Bit field holding the allowed results
                TUint64     iResultField;
            };


    protected: // Data

        // Sat messagehandler pointer
        CSatMessHandler*    iSatMessHandler;

        // Sat messaging pointer
        CTsySatMessaging*   iSatMessaging;

        // Command details Tlv
        TBuf8<5>            iCommandDetails;

        // Transaction ID (0-255)
        TUint8              iTransId;

        // Request handle, coming from ETel
        TTsyReqHandle       iReqHandle;

        // Bit field for allowed general results for specific command
        TAllowedResults     iAllowedResults;

    private: // Friend methods

        /**
        * Overwrite the default plus operator for general results. Combines
        * results into TAllowedResults-class. Has to be declared as a friend
        * method to enable the usage of internal TAllowedResults-class.
        * (results = RSat::KSuccess + RSat::KPartialComprehension)
        * @since NCP 5.1
        * @param aResultA First result to be allowed
        * @param aResultB Second result to be allowed
        * @return TAllowedResults-class
        */
        friend TAllowedResults operator+( const RSat::TPCmdResult aResultA,
            const RSat::TPCmdResult aResultB );
    };


/**
*  SAT Display Text notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to display a text message.
*  It allows SIM to define the priority of that message,
*  and the text string format.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyDisplayText : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyDisplayText( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyDisplayText();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles DisplayText PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the Display text terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TDisplayTextV2Pckg* iDisplayTextV2Pckg;
    };


/**
*  SAT Get Inkey notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to display text and expect the user
*  to enter a single character. Any response entered by the user shall
*  be passed transparently by the SAT to the SIM.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyGetInkey : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyGetInkey( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyGetInkey();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles GetInkey PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the Get Inkey terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TGetInkeyV2Pckg* iGetInkeyV2Pckg;
    };


/**
*  SAT Get Input notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to display text and that any
*  response string entered by the user shall be passed
*  by the SAT to the SIM. If the SIM provides a default text,
*  the SAT shall display this default text, which user may accept,
*  reject or edit as the response string.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyGetInput : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyGetInput( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyGetInput();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles GetInput PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the Get Input terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package received from ETel
        RSat::TGetInputV1Pckg* iGetInputV1Pckg;
    };


/**
*  SAT Play Tone notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to play an audio tone.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyPlayTone : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyPlayTone( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyPlayTone();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles PlayTone PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the Play Tone terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package received from ETel
        RSat::TPlayToneV2Pckg* iPlayToneV2Pckg;
    };


/**
*  SAT PollInterval notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd negotiates how often the ME shall send STATUS commands
*  related to Proactive Polling
*  Does not require any interaction with the Symbian OS side, and has no impact
*  on the Etel SAT client.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyPollInterval : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyPollInterval( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyPollInterval();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles PollInterval PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client. Not used in this PCmd since there is no interaction
        * required with Etel SAT client.
        * @since NCP 5.0
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );
    };


/**
*  SAT SetUpMenu notification class.
*  Instantiated by CSatMessaging class.
*  The SIM shall supply a set of menu items, which shall be used as
*  a main menu in SAT application. The user has then the opportunity
*  to choose one of these menu items at his own discretion.
*  Each item comprises a short identifier (used to indicate the selection)
*  and a text string. Also an Alpha identifier is supplied which acts as
*  a title for the list of menu items.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySetUpMenu : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySetUpMenu( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySetUpMenu();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SetUpMenu PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client.
        * @since NCP 5.0
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SetUpMenu terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    public: // New methods

        /**
        * From CSatNotificationsBase Handles the SetUpMenu terminal response
        * @since Series 60 Series60_2.6
        * @param none
        * @return TDes16& reference to a descriptor containing
        * a SAT application name
        */
        TDes16& SatApplicationName();


    private: // Data

        // Package received from ETel
        RSat::TSetUpMenuV2Pckg*     iSetUpMenuV2Pckg;

        // Toolkit name
        TBuf<RSat::KAlphaIdMaxSize> iToolKitName;

        // Buffer for isimessage. Used only if cmd is received before a notify
        HBufC8*                     iSatIsiMsg;

        //Flag for items next indicator
        TBool                       iItemsNextIndicatorRemoved;
    };


/**
*  SAT SelectItem notification class.
*  Instantiated by CSatMessaging class.
*  The SIM shall supply a set of items from which the user may
*  choose one. Each item comprises a short identifier
*  (used to indicate the selection) and a text string.
*  Optionally the SIM may include an alpha identifier. This is intended
*  to act as a title for the list of items.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySelectItem : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySelectItem( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySelectItem();


    public: // Methods from base classes

        /**
        * Initializer
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        void ConstructL( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * From CSatNotificationsBase Handles SelectItem PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SelectItem terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSelectItemV2Pckg*    iSelectItemV2Pckg;

        //Flag for items next indicator
        TBool                       iItemNextIndicatorRemoved;
    };


/**
*  SAT SendSm notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to send a short message to the
*  network.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySendSm : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySendSm( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySendSm();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SendSm PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SendSm terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    public: // New methods

        /**
        * From CSatNotificationsBase Get index of TP-UDL(User Data Length)
        * field from Tpdu
        * @param aTpdu pointer to Tpdu data
        * @return TUint8 index of TP-UDL field
        */
        TUint8 GetTpUdlIndex( TPtrC8 aTpdu );

        /**
        * From CSatNotificationsBase Get index of TP-DCS(Data Coding Scheme)
        * field from Tpdu
        * @param aTpdu pointer to Tpdu data
        * @return TUint8 index of TP-DCS field
        */
        TUint8 GetTpDcsIndex( TPtrC8 aTpdu );

        /**
        * From CSatNotificationsBase Check TPDU validity
        * @since Series 60 Series60_2.6
        * @param aTpdu pointer to Tpdu data
        * @return TInt KErrNone or KErrCorrupt
        */
        TInt CheckTpdu( TPtrC8 aTpdu );

        /**
        * From CSatNotificationsBase Pack SMS if it is required by the sim
        * @since Series 60 Series60_2.6
        * @param aTpdu pointer to received Tpdu data
        * @param aSendSm reference to package containing packed sms data
        * @return TInt KErrNone or KErrCorrupt
        */
        TInt PackSms( TPtrC8 aTpdu, TTpdu& aSendSm );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSendSmV1Pckg* iSendSmV1Pckg;

    };


/**
*  SAT SendDtmf notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to send a DTMF string towards the network.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySendDtmf : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySendDtmf( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySendDtmf();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SendDtmf PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SendDtmf terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSendDtmfV1Pckg* iSendDtmfV1Pckg;
    };


/**
*  SAT SendSs notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to send a SS string towards the network.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySendSs : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySendSs( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySendSs();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SendSs PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SendSs terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    public: // New methods

        /**
        * From CSatNotificationsBase Checks SS string if it is call forwarding
        * and append '+' character.
        * @since Series 60 Series60_2.6
        * @param aSource Original string is received from SIM
        * @param aSsString SS string compatible with ETel SAT
        * @return none
        */
        void CheckCallForwarding( TPtrC8 aSource, RSat::TSsString& aSsString );

        /**
        * From CSatNotificationsBase Checks validity of SS string.
        * If string includes undefined SS
        * characters or length is zero then return KErrCorrupt
        * @since Series 60 Series60_2.6
        * @param aSsString Original string is received from SIM
        * @return TInt KErrNone or KErrCorrupt
        */
        TInt CheckSsStringValidity( TPtrC8 aSsString );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSendSsV1Pckg*    iSendSsV1Pckg;
    };


/**
*  SAT SendUssd notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to send a USSD string towards the network.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySendUssd : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySendUssd( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySendUssd();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SendUssd PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SendUssd terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSendUssdV1Pckg*  iSendUssdV1Pckg;
    };


/**
*  SAT SetUpCall notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to setup a call.
*  Can be setup in three different ways:
*  - Set up a call, only if not currently busy on another call
*  - Set up a call, put all other call on hold
*  - Set up a call, disconnect other calls
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySetUpCall : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySetUpCall( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySetUpCall();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SetUpCall PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SetUpCall terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    public: // New methods

        /**
        * From CSatNotificationsBase Cache call connected event
        * @since Series 60 Series60_2.6
        * @param aEnvelope reference to a descriptor containing envelope data.
        * @return none
        */
        void StoreCallConnectedEvent( const TDesC8& aEnvelope );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSetUpCallV1Pckg* iSetUpCallV1Pckg;

        // Cache for Call connected event
        TBuf8<12>               iCallConnectedEvent;

    };


/**
*  SAT Refresh notification class.
*  Instantiated by CSatMessaging class.
*  The purpose of this PCmd is to allow the SAT
*  (which alerts other applications in the phone) to be notified
*  of the changes in SIM configuration that have occurred as
*  the result of a SIM application activity. It is up to SAT to
*  ensure this is done correctly..
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyRefresh : public CSatNotificationsBase
    {
    public: // Data types

        // Enumaration of cache EF files.
        enum TCachedSimAtkFiles
            {
            KCacheEFSST     = 0x01, // SIM Service table
            KCacheEFCBMID   = 0x02, // CB DataDownload message IDs
            // Next 0x04, 0x08 etc..
            };

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyRefresh( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyRefresh();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles Refresh PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the Refresh terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    public: // New methods

        /**
        * From CSatNotificationsBase Notification request for RefreshRequired
        * received from Etel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void NotifyRefreshRequired( const TTsyReqHandle aRegHandle,
            TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Cancels notification of
        * RefreshRequiredParams request.
        * @since Series 60 Series60_2.6
        * @param aReqHandle notify request handle
        * @return status
        */
        TInt CancelRefreshRequiredNotification(
            const TTsyReqHandle aReqHandle );

        /**
        * From CSatNotificationsBase Response for RefreshRequired notification
        * @since Series 60 Series60_2.6
        * @param aDataPtr Pointer to descriptor containing response data
        * @return TInt
        */
        TInt RefreshAllowed( TDesC8* aDataPtr );

        /**
        * From CSatNotificationsBase Overloads original protected
        * CompleteRequest method from Base class.
        * @since Series 60 Series60_2.6
        * @param aError possible error code
        * @return none
        */
        void CompleteRequest( const TInt aError );

        /**
        * From CSatNotificationsBase Returns list of files that are
        * cached in SIM ATK TSY
        * @since Series 60 Series60_2.6
        * @param none
        * @return TUin16 iInternalCache
        */
        TUint16 CachedFiles() const;


    private: // New methods

        /**
        * Maps a 3GPP refresh value to UICC refresh value
        * @param a3GppRefresh Command Qualifier from 3GPP spec
        * @return TUint8 ServiceType from UICC specification
        */
        TUint8 Map3GppRefreshToUiccValues( const TUint8 a3GppRefresh );


    private: // Data

        // Package allocated by an Etel client
        RSat::TRefreshV2Pckg*                  iRefreshV2Pckg;

        // Package allocated by an Etel client
        RSat::TRefreshV2Pckg*                  iRefreshRequiredV2Pckg;

        // file list with full path
        TBuf8<RSat::KRefreshedFileListMaxSize> iFileList;

        // To store Aplication id
        TBuf8<RSat::KAidMaxSize>               iAid;

        // Request handle for Refresh Required, coming from ETel
        TTsyReqHandle                          iReqHandleRefreshRequired;

        // List of files that are cached in SIM ATK TSY
        TUint16                                iInternalCache;
    };


/**
*  SAT SimSessionEnd notification class.
*  Instantiated by CSatMessaging class.
*  SIM Session end tells the client when the last PCmd in SIM application has
*  been processed, i.e. the SIM application has ended.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySimSessionEnd : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySimSessionEnd( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySimSessionEnd();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SimSessionEnd PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& /*aIsiMessage*/ );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );
    };


/**
*  SAT SetUpIdleModeText notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to change the text displayed
*  by the mobile equipment whenever it is in idle mode.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySetUpIdleModeText : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySetUpIdleModeText( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySetUpIdleModeText();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SetUpIdleModeText PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SetUpIdleModeText
        * terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSetUpIdleModeTextV1Pckg* iSetUpIdleModeTextV1Pckg;

    };


/**
*  SAT CallControlRequest notification class.
*  Instantiated by CSatMessaging class.
*  CallControlRequest is not treated as a proactive command in Series60.
*  The method CompleteNotification is used to notify an Etel SAT client
*  that the SIM Applet has provided an alpha identifier in response to
*  a CallControl envelope. The class CSatCC implements the CallControl
*  functionality.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyCallControlRequest : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyCallControlRequest( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyCallControlRequest();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles CallControlRequest PCmd ISI message
        * Not in use.
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& /*aIsiMessage*/ );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );


    public: // New methods

        /**
        * From CSatNotificationsBase Notification completion
        * @since Series 60 Series60_2.6
        * @param aAlphaId alpha identifier in Etel SAT format
        * @param aResult call control result in Etel SAT format
        * @return none
        */
        void CompleteNotification( TDesC& aAlphaId,
            RSat::TControlResult aResult );

        /**
        * From CSatNotificationsBase Notification completion for CC GPRS
        * @param aAlphaId: Alpha identifier in Etel SAT format
        * @param aPdpContextParameters: PDP Context Parameters
        * @param aResult: Call control result in Etel SAT format
        * @return none
        */
        void CompleteNotification(
            const TDesC& aAlphaId,
            const RSat::TControlResult aResult );

        /**
        * From CSatNotificationsBase Notification completion
        * @param aCcResult CC result mapped to ISA values
        * @param aEnvelopeResponse response data received from SIM application
        * @return none
        */
        void CompleteNotificationL( const TUint8 aCcResult,
            TPtrC8 aEnvelopeResponse );

    private: // Data

        // Package allocated by an Etel client
        RSat::TCallControlV5Pckg*   iCallControlPckg;
    };


/**
*  SAT LaunchBrowser notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to start a web browser application instance on
*  the mobile equipment.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyLaunchBrowser : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyLaunchBrowser( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyLaunchBrowser();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles LaunchBrowser PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the LaunchBrowser terminal
        * response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TLaunchBrowserV2Pckg* iLaunchBrowserV2Pckg;
    };


/**
*  SAT SetUpEventList notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to set up an event list in order
*  to be able to process events received from ETel SAT clients
*  or CellMo clients.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifySetUpEventList : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySetUpEventList( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySetUpEventList();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SetUpEventList PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SetUpEventList terminal
        * response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSetUpEventListV1Pckg*    iSetUpEventListV1Pckg;

        // Command qualifier
        TUint8                          iCmdQualifier;

        // Event list
        TUint                           iEvents;
    };


/**
*  SAT PollingOff notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd resets how often the ME shall send STATUS commands
*  related to Proactive Polling. The default value for reset is 25 seconds.
*  Does not require any interaction with the Symbian OS side, and has no impact
*  on the Etel SAT client.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyPollingOff : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyPollingOff( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyPollingOff();

    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles PollingOff PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage to received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client. Not used in this PCmd since there is no interaction
        * required with Etel SAT client.
        * @since NCP 5.0
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );
    };


/**
*  SAT LocalInfo notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to provide local information. Some
*  of the subfunctionality do not require interaction with Etel.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyLocalInfo : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyLocalInfo( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyLocalInfo();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles LocalInfo PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle,
                             TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the LocalInfo terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp = NULL );


    public: // New methods

        /**
        * From CSatNotificationsBase Handles the LocalInfo terminal response
        * @since Series 60 Series60_2.6
        * @param aClearSatus If true clear status
        * @return TBool iLocalInfoIsOngoing
        */
        TBool Status( TBool aClearStatus = EFalse );


    private: // Data

        // Package allocated by an Etel client
        RSat::TLocalInfoV3Pckg* iLocalInfoV3Pckg;

        // Response package allocated by SimAtkTsy. In use when terminal
        // response result and parameters are determined in SimAtkTsy.
        RSat::TLocalInfoRspV3   iLocalInfoRspV3;

        // Flag
        TBool                   iLocalInfoIsOngoing;
    };


/**
*  SAT TimerManagement notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to start one timer. Up to 8 timers.
*  Does not require any interaction with the Symbian OS side, and has no impact
*  on the Etel SAT client.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyTimerMgmt : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyTimerMgmt( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyTimerMgmt();

    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles TimerManagement PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client. Not used in this PCmd since there is no interaction
        * required with Etel SAT client.
        * @since NCP 5.0
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );


    private:    // New methods
        /**
        * From CSatNotificationsBase Converts time to seconds
        * @since Series 60 Series60_2.6
        * @param aTime pointer to time data
        * @return TUint32 time expressed in seconds
        */
        TUint32 ConvertToSeconds( TPtrC8 time );


    private: // Data

        // Command qualifier
        TUint8 iCmdQualifier;
    };


/**
*  SAT MoreTime notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd informs the SAT that the SIM needs more time.
*  Does not require any interaction with the Symbian OS side, and has no impact
*  on the Etel SAT client.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyMoreTime : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyMoreTime( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyMoreTime();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles MoreTime PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client. Not used in this PCmd since there is no interaction
        * required with Etel SAT client.
        * @since NCP 5.0
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );
    };


/**
*  SAT LanguageNotification notification class.
*  Instantiated by CSatMessaging class.
*  The SIM shall use this PCmd to notify the ME about the language
*  currently used for any text string within proactive commands or
*  envelope command responses.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyLanguageNotification : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyLanguageNotification( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyLanguageNotification();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles LanguageNotification PCmd
        * ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle,
                             TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the LanguageNotification
        * terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: //Data

        // Package allocated by an Etel client
        RSat::TLanguageNotificationV2Pckg*  iLanguageNotificationV2Pckg;
    };


/**
*  SAT OpenChannel notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to open either GPRS channel, or a CSD channel
*  or a local link channel, exclusively.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_3.0
*/
class CSatNotifyOpenChannel : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyOpenChannel( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyOpenChannel();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles OpenChannel PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the OpenChannel terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );

        /**
        * From CSatNotificationsBase Handles Bearer type and parameters data
        * @since PP 5.2
        * @param aBearerDescriptionTlv tlv containing specific data
        * @param aBearer bearer structure
        * @return none
        */
        void GetBearerTypeAndParams( CTlv& aBearerDescriptionTlv,
            RSat::TBearer& aBearer );

        /**
        * From CSatNotificationsBase Handles SIM/ME Interface transport level and
        * Data Destination Address specific data
        * @since PP 5.2
        * @param aBerTlv tlv containing specific data
        * @param aSimMeInterface SIM/ME interface structure
        * @param aDestinationAddress Data Destination address structure
        * @return KErrNone or KErrNotFound
        */
        TInt SimMeInterfaceAndDataDestinationAddress( CBerTlv& aBerTlv,
            RSat::TSimMeInterface& aSimMeInterface,
            RSat::TOtherAddress& aDestinationAddress );

        /**
        * From CSatNotificationsBase Handles Local Address specific data
        * @since PP 5.2
        * @param aBerTlv tlv containing specific data
        * @param aLocalAddress Local address structure
        * @return none
        */
        void LocalAddress( CBerTlv& aBerTlv,
            RSat::TOtherAddress& aLocalAddress );

        /**
        * From CSatNotificationsBase Handles the OpenChannel GPRS Bearer
        * specific data
        * @since PP 5.2
        * @param aBerTlv tlv containing Bearer specific data
        * @param RSat::TOpenGprsChannelV4& aOpenGprsChannelV4 data
        * @return none
        */
        void GprsBearerSpecific( CBerTlv& aBerTlv,
            RSat::TOpenGprsChannelV4& aOpenGprsChannelV4 );

    private: // Data

        // Packages allocated by ETel:

        // Base class
        RSat::TOpenChannelBaseV2Pckg*       iOpenBaseChannelV2Pckg;

        // Open Cs Channel package store
        RSat::TOpenCsChannelV2Pckg*         iOpenCsChannelV2Pckg;

        // Open Gprs Channel package store
        RSat::TOpenGprsChannelV4Pckg*       iOpenGprsChannelV4Pckg;

        // Open Local Link Channel package store
        RSat::TOpenLocalLinksChannelV2Pckg* iOpenLocalLinksChannelV2Pckg;
    };


/**
*  SAT GetChannelStatus notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to tell the SIM about a channel status specified
*  in the PCmd.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_3.0
*/
class CSatNotifyGetChannelStatus : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyGetChannelStatus( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyGetChannelStatus();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles GetChannelStatus PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the GetChannelStatus
        * terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TGetChannelStatusV2Pckg* iGetChannelStatusRspV2Pckg;
    };


/**
*  SAT CloseChannel notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to close either GPRS channel, or a CSD channel
*  or a local link channel, exclusively.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_3.0
*/
class CSatNotifyCloseChannel : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyCloseChannel( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyCloseChannel();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles CloseChannel PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the CloseChannel terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TCloseChannelV2Pckg*  iCloseChannelRspV2Pckg;
    };


/**
*  SAT SendData notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to send data through either GPRS channel,
*  or a CSD channel or a local link channel, exclusively.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_3.0
*/
class CSatNotifySendData : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifySendData( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifySendData();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles SendData PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the SendData terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TSendDataV2Pckg*  iSendDataRspV2Pckg;
    };


/**
*  SAT ReceiveData notification class.
*  Instantiated by CSatMessaging class.
*  This PCmd instructs the SAT to receive data either from GPRS channel,
*  or a CSD channel or a local link channel, exclusively.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_3.0
*/
class CSatNotifyReceiveData : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyReceiveData( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyReceiveData();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles ReceiveData PCmd ISI message
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Handles the ReceiveData terminal response
        * @since Series 60 Series60_2.6
        * @param aRsp pointer to a descriptor containing the response structure
        * @return status
        */
        TInt TerminalResponse( TDes8* aRsp );


    private: // Data

        // Package allocated by an Etel client
        RSat::TReceiveDataV2Pckg*   iReceiveDataRspV2Pckg;
    };


/**
*  SAT MoSmControlRequest notification class.
*  Instantiated by CSatMessaging class.
*  MoSmControlRequest is not treated as a proactive command in Series60.
*  The method CompleteNotification is used to notify an Etel SAT client
*  that the SIM Applet has provided an alpha identifier in response to
*  a CallControl envelope. The class CSatMoSmsCtrl implements the MoSmControl
*  functionality.
*
*  @lib SIMATKTSY.lib
*  @since Series 60 Series60_2.6
*/
class CSatNotifyMoSmControlRequest : public CSatNotificationsBase
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatNotifyMoSmControlRequest( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        ~CSatNotifyMoSmControlRequest();


    public: // Methods from base classes

        /**
        * From CSatNotificationsBase Handles MoSmControlRequest PCmd ISI message
        * Not in use.
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MessageReceived( const TIsiReceiveC& /*aIsiMessage*/ );

        /**
        * From CSatNotificationsBase Notification request received from
        * ETel client
        * @since Series 60 Series60_2.6
        * @param aReqHandle request handle from ETel
        * @param aDataPtr pointer to parameter class
        * @return none
        */
        void Notify( const TTsyReqHandle aReqHandle, TDes8* aDataPtr = NULL );

        /**
        * From CSatNotificationsBase Notification completion
        * @since Series 60 Series60_2.6
        * @param aAlphaId alpha identifier in Etel SAT format
        * @param aResult call control result in Etel SAT format
        * @return none
        */
        void CompleteNotification( TDesC& aAlphaId,
            RSat::TControlResult aResult );


    private: // Data

        // Package allocated by an Etel client
        RSat::TMoSmControlV1Pckg*   iMoSmControlV1Pckg;
    };

#endif // SATNOTIFICATIONS_H

// End of File
