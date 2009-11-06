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



#ifndef SATMESSAGING_H
#define SATMESSAGING_H

//  INCLUDES
#include "cmmmessagerouter.h"
#include <ctsy/serviceapi/ctsysatmessagingbase.h>           // base class ctsysatmessagingbase

#include "tsylogger.h"                      // tsy logger
#include <etelsat.h>                        // etel sat api
#include <etelmm.h>                         // etel multimode api
#include <product_profile_definitions.h>    // product profile definitions

// CONSTANTS
const TInt      KZero               = 0;
const TUint8    KPadding            = 0;

// MACROS
    //None

// DATA TYPES
    //None

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS

// External
class CMmPhoneTsy;
class CMmPhoNetSender;
class CMmPhoNetReceiver;
class CMmSmsTsy;
class CMmMessageRouterBase;
class TIsiReceiveC;

// Internal
class CSatMessHandler;
class CSatCC;
class CSatTimer;
class CSatEventDownload;
class CSatDataDownload;
class CSatIcon;
class CSatMoSmsCtrl;
class TTransIdMessage;
class CSatFlightModeStatus;
class TTlv;

// Notifications
class CSatNotifyDisplayText;
class CSatNotifyGetInkey;
class CSatNotifyGetInput;
class CSatNotifyPlayTone;
class CSatNotifySetUpMenu;
class CSatNotifySelectItem;
class CSatNotifySendSm;
class CSatNotifySendSs;
class CSatNotifySendUssd;
class CSatNotifySetUpCall;
class CSatNotifyRefresh;
class CSatNotifySimSessionEnd;
class CSatNotifySetUpIdleModeText;
class CSatNotifyLaunchBrowser;
class CSatNotifyCallControlRequest;
class CSatNotifyPollInterval;
class CSatNotifySendDtmf;
class CSatNotifySetUpEventList;
class CSatNotifyPollingOff;
class CSatNotifyLocalInfo;
class CSatNotifyTimerMgmt;
class CSatNotifyMoreTime;
class CSatNotifyLanguageNotification;
class CSatNotifyOpenChannel;
class CSatNotifyGetChannelStatus;
class CSatNotifyCloseChannel;
class CSatNotifyReceiveData;
class CSatNotifySendData;
class CSatNotifyMoSmControlRequest;

// CLASS DECLARATION
/**
*  Sat notifications base class.
*  Sat Messaging class.
*  Each SAT proactive command has its own class, which holds the information
*  relevant to the command in question. All proactive command classes are
*  created when CTsySatMessaging object is created. This way each proactive
*  command is handled by its own notify class. The proactive commands are
*  encapsulated to individual classes in order to simplify the design and
*  updating.
*  This class also handles SAT originated SMS message sending and menu
*  selection in SAT. All not supported functions are completed here.
*
*  @lib SIMATKTSY.LIB
*  @since Series 60 Series60_2.6
*/
class CTsySatMessaging : public CTsySatMessagingBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aMmPhone pointer to mmphone class
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aName Sat application fixed name
        */
        IMPORT_C static CTsySatMessaging* NewL(
            #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
            CMmPhoneTsy* aMmPhone, CMmMessageRouterBase* aRouter, TName aName );
            #else
            CMmMessageRouter* aRouter );
            #endif //NCP_COMMON_S60_VERSION_SUPPORT

        /**
        * Destructor.
        */
        virtual ~CTsySatMessaging();


    private: // Constructors

        /**
        * C++ default constructor.
        * @since Series 60 Series60_2.6
        * @param aMmPhone pointer to mmphone class
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aName Sat application fixed name
        */
        CTsySatMessaging(
            #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
            CMmPhoneTsy* aMmPhone, CMmPhoNetSender* aPnSend, CMmPhoNetReceiver* aPnReceive, TName aName );
            #else
            CMmPhoNetSender* aPnSend, CMmPhoNetReceiver* aPnReceive, CMmMessageRouter* aRouter );
            #endif //NCP_COMMON_S60_VERSION_SUPPORT


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Copy constructor, usage not allowed
        * @since Series 60 Series60_2.6
        * @param Satmesshandler reference
        */
        CTsySatMessaging( const CTsySatMessaging& aRhs );

        /**
        * Assignment operator, usage not allowed
        * @since Series 60 Series60_2.6
        * @param Satmesshandler reference
        * @return SatMessHandler reference
        */
        CTsySatMessaging& operator = ( const CTsySatMessaging& aRhs );


    public: // Functions from base classes

        /**
        * Opens a new object identified by a name.
        * @since Series 60 Series60_2.6
        * @param aName Name to open
        * @return TelObject pointer to a telephony object
        */
        CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
        * Opens a new object identified by new name. Can leave.
        * Not in use.
        * @since Series 60 Series60_2.6
        * @param aNewName Name to open
        * @return TelObject pointer to a telephony object
        */
        CTelObject* OpenNewObjectL( TDes& aNewName );

        /**
        * Command handler for ETel server messages. The IPC identifies
        * the command to be executed if supported.
        * @since Series 60 Series60_2.6
        * @param aTsyReqHandle TsyReqHandle
        * @param aIpc command identifier
        * @param aPackage Data pointer
        * @return status
        */
        TInt ExtFunc( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc, const TDataPackage& aPackage );

        /**
        * ReqMode method for checking what modes this component supports
        * @since Series 60 Series60_2.6
        * @param aIpc command identifier
        * @return reqmode
        */
        CTelObject::TReqMode ReqModeL( const TInt aIPC );

        /**
        * Cancels the ongoing service
        * @since Series 60 Series60_2.6
        * @param aIpc command identifier
        * @param aTsyReqHandle TsyReqHandle
        * @return status
        */
        TInt CancelService( const TInt aIpc,
            const TTsyReqHandle aTsyReqHandle );

        /**
        * Initializer method that is called by ETel Server
        * @since Series 60 Series60_2.6
        * @param none
        * @return none
        */
        void Init();

        /**
        * Register given command, checks if the given command is supported
        * @since Series 60 Series60_2.6
        * @param aIpc command identifier
        * @return TInt status
        */
        TInt RegisterNotification( const TInt aIpc );

        /**
        * DeRegister given command, checks if the given command is supported
        * @since Series 60 Series60_2.6
        * @param aIpc command identifier
        * @return TInt status
        */
        TInt DeregisterNotification( const TInt aIpc );

        /**
        * Returns number of slots to be used for given IPC
        * @since Series 60 Series60_2.6
        * @param aIpc command identifier
        * @return TInt status
        */
        TInt NumberOfSlotsL( const TInt aIpc );


    public: // New methods

        /**
        * Handles an incoming proactive command from ISA CellMo SIM server
        * Called by CSatMessHandler when it receives proactive command ISI msg
        * originated by ISA CellMo SIM server.
        * @since NCP 5.0
        * @param aIsiMessage received proactive command ISI msg
        * @return none
        */
        void PCmdReceivedL( const TIsiReceiveC& aIsiMessage );

        /**
        * Notifies client about end of session
        * @since NCP 5.0
        * @param aIsiMsg
        * @return none
        */
        void SessionEnd( const TIsiReceiveC& /*aIsiMessage*/ );

        /**
        * Notifies Etel SAT client about call control event
        * This method is actually used only when an alpha id is present.
        * This is a design desicion, though it could be possible to notify ETel
        * SAT client everytime there is a call control result.
        * @since Series 60 Series60_2.6
        * @param aAlphaId alpha identifier
        * @param TControlResult result
        * @return none
        */
        void NotifyClientAboutCallControlEvent( TDesC& aAlphaId,
            RSat::TControlResult aResult );

        /**
        * Notifies Etel SAT client about call control event
        * @param aCcResult CC result from SIM application mapped to ISA value
        * @param aEnvelopeResponse response from SIM application
        * @return none
        */
        void NotifyClientAboutCallControlEventL( const TUint8 aCcResult,
            TPtrC8 aEnvelopeResponse );

        /**
        * Notifies Etel SAT client about GPRS call control event
        * This method is actually used only when an alpha id is present.
        * This is a design desicion, though it could be possible to notify ETel
        * SAT client everytime there is a call control result.
        * @param aAlphaId alpha identifier
        * @param TControlResult result
        * @return none
        */
        void NotifyClientAboutGprsCallControlEvent(
            const TDesC& aAlphaId,
            const RSat::TControlResult aResult );

        /**
        * Notifies client about Mo-Sms control event
        * This method is actually used only when an alpha id is present.
        * This is a design desicion, though it could be possible to notify ETel
        * SAT client everytime there is a MoSm control result.
        * @since Series 60 Series60_2.6
        * @param aAlphaId alpha identifier
        * * !!! RSat::TAlphaIdBuf& would have been better...
        * @param TControlResult result
        * @return none
        */
        void NotifyClientAboutMoSmControlEvent( TDesC& aAlphaId,
            RSat::TControlResult aResult );

        /**
        * Processes ISI messages that are relevant for event download
        * Called by CSatMessHandler when it receives an ISI msg
        * @since Series 60 Series60_3.0
        * @param aIsiMessage Pointer to an ISI msg
        * @return none
        */
        void EventDownloadReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Processes ISI messages that are relevant for call control
        * Called by CSatMessHandler when it receives ISI msg
        * @since NCP 3.1
        * @param aIsiMessage received ISI message
        * @return none
        */
        void CallControlReceivedL( const TIsiReceiveC& aIsiMessage );

        /**
        * Processes ISI messages that are relevant for data download
        * Called by CSatMessHandler when it receives an ISI msg
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void DataDownloadReceivedL( const TIsiReceiveC& aIsiMessage );

        /**
        * Processes ISI messages that are relevant for Mo-Sms Ctrl
        * Called by CSatMessHandler when it receives an ISI msg
        * @since NCP 5.0
        * @param aIsiMessage received ISI message
        * @return none
        */
        void MoSmsControlReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * This methods returns ETrue if MoSmControl
        * is activated. Otherwise, it returns EFalse
        * @since Series 60 Series60_2.6
        * @param None
        * @return TBool, is MoSmControl activated or not
        */
        TBool IsMoSmControlBySimActivated();

        /**
        * Set up requested events that have to be reported to the SIM
        * @since Series 60 Series60_2.6
        * @param  aEvents bit mask
        * @return none
        */
        void SetUpEventList( TUint32 aEvents );

        /**
        * Terminal response command handler from ETel SAT client application.
        * @since Series 60 Series60_2.6
        * @param aPCmd proactive command Id
        * @param sRsp response structure
        * @param aTsyReqHandle TsyReqHandle
        * @return status
        */
        TInt TerminalResponse( RSat::TPCmd* aPCmd, TDes8* aRsp,
            TTsyReqHandle aTsyReqHandle );

        /**
        * Menu selection command handler from ETel SAT client application.
        * @since Series 60 Series60_2.6
        * @param aSelection command structure
        * @param aTsyReqHandle TsyReqHandle
        * @return status
        */
        TInt MenuSelection( TDes8* aSelection, TTsyReqHandle aTsyReqHandle );

        /**
        * Cell broadcast data download from ETel SAT client application
        * Not in use. CellBroadcast DDL is handled in CSatDataDownload class.
        * @since Series 60 Series60_2.6
        * @param aPackage containing the Pdu of the messsage
        * @param aTsyReqHandle Telephony service request handle
        * @return status
        */
        TInt NotifyCbDownload( TDes8* aPackage, TTsyReqHandle aTsyReqHandle );

        /**
        * Sms-Pp data download from ETel SAT client application
        * Not in use. Sms-Pp DDL is handled in CSatDataDownload class.
        * @since Series 60 Series60_2.6
        * @param aPackage containing the addresses and the Sms Tpdu
        * @param aTsyReqHandle Telephony service request handle
        * @return status
        */
        TInt NotifySmsPpDownload( TDes8* aPackage,
            TTsyReqHandle aTsyReqHandle );

        /**
        * Send SMS No Logging command handler from client application.
        * @since Series 60 Series60_2.6
        * @param aMsg Pointer to descriptor containing a command structure.
        * @param aMsgRef Pointer to descriptor containing a command structure.
        * @param aTsyReqHandle TsyReqHandle
        * @return status
        */
        TInt SendMessageNoLoggingL( TDes8* aMsg, TUint16* aMsgRef,
            TTsyReqHandle aTsyReqHandle );

        /**
        * Completes a SAT Originated SMS sending request to Etel SAT client
        * with the corresponding outcome.
        * @since Series 60 Series60_2.6
        * @param status was sending successfull/unsuccessfull
        * @return status
        */
        TInt CompleteSendSmsMessage( TInt aStatus );

        /**
        * Returns pointer to the SatMessHandler instance
        * @since Series 60 Series60_2.6
        * @return SatMessHandler*: SAT message handler pointer
        */
        CSatMessHandler* GetSatMessHandler();

        /**
        * Returns pointer to the CSatFlightModeStatus instance
        * @return CSatFlightModeStatus* Pointer to CSatFlightModeStatus
        */
        CSatFlightModeStatus* GetSatFlightModeStatus();

        /**
        * Timer Expiration handler
        * @since Series 60 Series60_2.6
        * @param TInt aTimerId: timer identification
        * @param TUint32 aTimerValue: timer value
        * @return status
        */
        TInt TimerExpiration( TInt aTimerId, TUint32 aTimerValue );

        /**
        * Returns pointer to the SatTimer instance
        * @since Series 60 Series60_2.6
        * @return CSatTimer*
        */
        CSatTimer* GetSatTimer();

        /**
        * Returns pointer to the CSatNotifyRefresh instance
        * @since Series 60 Series60_2.6
        * @return CSatNotifyRefresh*
        */
        CSatNotifyRefresh* GetNotifyRefresh();

        /**
        * Returns pointer to the CSatDataDownload instance
        * @since Series 60 Series60_2.6
        * @return CSatDataDownload*
        */
        CSatDataDownload* GetDataDownload();

        /**
        * Returns pointer to the NotifyLocalInfo instance
        * @since Series 60 Series60_2.6
        * @return CSatNotifyLocalInfo*
        */
        CSatNotifyLocalInfo* GetNotifyLocalInfo();

        /**
        * Returns pointer to the NotifyPollInterval instance
        * @since Series 60 Series60_2.6
        * @return CSatNotifyPollInterval*
        */
        CSatNotifyPollInterval* GetNotifyPollInterval();

        /**
        * Returns pointer to the NotifyPollingOff instance
        * @since Series 60 Series60_2.6
        * @return CSatNotifyPollingOff*
        */
        CSatNotifyPollingOff* GetNotifyPollingOff();

        /**
        * Returns pointer to the CSatMoSmsCtrl instance
        * @since Series 60 Series60_2.6
        * @return CSatMoSmsCtrl*
        */
        CSatMoSmsCtrl* GetMoSmsCtrl();

        /**
        * Returns pointer to the CSatEventDownload instance
        * @since Series 60 Series60_3.0
        * @return CSatEventDownload*
        */
        CSatEventDownload* GetEventDownload();

        /**
        * Get new transaction id
        * @since Series 60 Series60_2.6
        * @return  new transaction id
        */
        TUint8 GetTransactionId();

        /**
        * Sat ready indication
        * This method is used to retrieve a proactive command ISI message
        * which was received by the Nokia TSY CPhonetReceiver before the
        * creation of CSatMessaging and CSatMessHandler instances.
        * @since Series 60 Series60_2.6
        * @param TUint8: Message to be retrieved.
        * @return TInt: success/failure if msg was found
        */
        TInt SatReady( TUint8 aMsg );

         /**
        * Clears the array containing call control event data, due to
        * a refresh PCmd.
        * @since Series 60 Series60_2.6
        * @return none
        */
        void ClearCCArrays();

        /**
        * Returns pointer to the SatIcon instance
        * @since Series 60 Series60_2.6
        * @return CSatIcon*
        */
        CSatIcon* GetSatIcon();

        /**
        * Cache call connected envelope
        * @since Series 60 Series60_2.6
        * @param aEnvelope reference to descriptor containing
        * envelope data.
        * @return none
        */
        void StoreCallConnectedEvent( const TDesC8& aEnvelope );

        /**
        * Updates the status of SetUpCall command in SatEventDownload instance.
        * @since Series 60 Series60_2.6
        * @param aStatus
        * @return none
        */
        void SetSetUpCallStatus( const TBool aStatus );

        /**
        * Set CSatCC internal flag according to EF-SimServiceTable.
        * The flag tells whether Ussd TLV is supported in Call Control by SIM.
        * @since Series 60 Series60_2.6
        * @param aStatus
        * @return none
        */
        void SetStatusOfUssdSupport( const TBool aStatus );

        /**
        * Set CSatCC internal Ton and Npi
        * @since Series 60 Series60_2.6
        * @param aTonNpi
        * @return none
        */
        void SetTonNpi( const TUint8 aTonNpi );

        /**
        * Requests the storing of SMS message from CommonTSY
        * @since NCP 5.0
        * @param aSmsEntry SMS entry to be stored
        * @return ErrorCode returned by CommonTSY
        */
        TInt StoreSmsL( RMobileSmsStore::TMobileGsmSmsEntryV1& aSmsEntry );

#if ( NCP_COMMON_S60_VERSION_SUPPORT >= S60_VERSION_50 )
        /**
        * Returns pointer to message router instance
        * @return pointer to message router
        */
        CMmMessageRouter* GetMessageRouter();
#endif

    private: // New methods

        /**
        * Handles extended ETel SAT client requests, received from ExtFunc
        * @since Series 60 Series60_2.6
        * @param aTsyReqHandle TsyReqHandle
        * @param aIpc command identifier
        * @param aPackage Data pointer
        * @return status
        */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, const TInt aIpc,
            const TDataPackage& aPackage );

        /**
        * The (U)SAT client calls this method to inform the TSY that it has
        * posted all the relevant (U)SAT notifications and is ready to receive
        * the (U)SAT Commands.
        * @since PP5.2
        * @param TTsyReqHandle aTsyReqHandle
        * @return none
        */
        void UsatClientReadyIndication( TTsyReqHandle aTsyReqHandle );

    private:  // Data

        // Proactive command class instances
        // created when this class is created, all classes
        // need pointers to this class and to satmesshandler
        // class
        CSatNotifyDisplayText*          iNotifyDisplayText;
        CSatNotifyGetInkey*             iNotifyGetInkey;
        CSatNotifyGetInput*             iNotifyGetInput;
        CSatNotifyPlayTone*             iNotifyPlayTone;
        CSatNotifySetUpMenu*            iNotifySetUpMenu;
        CSatNotifySelectItem*           iNotifySelectItem;
        CSatNotifySendSm*               iNotifySendSm;
        CSatNotifySendSs*               iNotifySendSs;
        CSatNotifySendUssd*             iNotifySendUssd;
        CSatNotifySetUpCall*            iNotifySetUpCall;
        CSatNotifyRefresh*              iNotifyRefresh;
        CSatNotifySimSessionEnd*        iNotifySimSessionEnd;
        CSatNotifySetUpIdleModeText*    iNotifySetUpIdleModeText;
        CSatNotifyLaunchBrowser*        iNotifyLaunchBrowser;
        CSatNotifyCallControlRequest*   iNotifyCallControlRequest;
        CSatNotifyPollInterval*         iNotifyPollInterval;
        CSatNotifySendDtmf*             iNotifySendDtmf;
        CSatNotifySetUpEventList*       iNotifySetUpEventList;
        CSatNotifyPollingOff*           iNotifyPollingOff;
        CSatNotifyLocalInfo*            iNotifyLocalInfo;
        CSatNotifyTimerMgmt*            iNotifyTimerMgmt;
        CSatNotifyMoreTime*             iNotifyMoreTime;
        CSatNotifyLanguageNotification* iNotifyLanguageNotification;
        CSatNotifyOpenChannel*          iNotifyOpenChannel;
        CSatNotifyGetChannelStatus*     iNotifyGetChannelStatus;
        CSatNotifyCloseChannel*         iNotifyCloseChannel;
        CSatNotifyReceiveData*          iNotifyReceiveData;
        CSatNotifySendData*             iNotifySendData;
        CSatNotifyMoSmControlRequest*   iNotifyMoSmControlRequest;

        // Implemented in by cpp/h files in SimAtkTsy:
        CSatEventDownload*              iEventDownload;
        CSatTimer*                      iSatTimer;
        CSatCC*                         iSatCC;
        CSatDataDownload*               iSatDataDownload;
        CSatIcon*                       iSatIcon;
        CSatMoSmsCtrl*                  iSatMoSmsCtrl;
        TTransIdMessage*                iTransIdMsg;
        CSatFlightModeStatus*           iSatFlightModeStatus;

        // Satmesshandler class, responsible for
        // sending ISI messages to via NokiaTSY CMmPhonetSender and receiving
        // ISI messages via Nokia TSY CMmPhonetReceiver.
        CSatMessHandler*    iSatMessHandler;

        // Pointer to main PhoneTsy class of CommonTSY component
        CMmPhoneTsy*        iMmPhone;

        // Pointer to ISI message sending class of NokiaTSY component
        CMmPhoNetSender*    iPnSend;

        // Pointer to ISI message receiving class of NokiaTSY component
        CMmPhoNetReceiver*  iPnReceive;

        // Fixed sat application name
        TName               iSatAppName;

        // Request handle for SAT SMS sending to MMSMS of CommonTSY component
        TTsyReqHandle       iSendSmsTsyReqHandle;

        // Message router
        CMmMessageRouter*   iRouter;

    };

#endif  // SATMESSAGING_H

// End of File
