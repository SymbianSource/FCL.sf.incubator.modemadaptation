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



#ifndef SATEVENTDOWNLOAD_H
#define SATEVENTDOWNLOAD_H

//  INCLUDES
#include <e32base.h>        // base class cbase
#include <et_tsy.h>         // type declarations for tsy
#include <etelsat.h>        // etel sat api

#include "satmesshandler.h" // tlocationinfo


// FORWARD DECLARATIONS
class CSatMessHandler;
class CTsySatMessaging;
class TIsiReceiveC;


// CLASS DECLARATION
/**
*  CSatEventDownload
*  Sat event download handling
*  @lib SIMATKTSY.LIB
*  @since Series 60 Series60_2.6
*/
// DESCRIPTION
// Sat event download handling
class CSatEventDownload : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        static CSatEventDownload* NewL( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        * @since NCP 5.0
        */
        virtual ~CSatEventDownload();


    private:    // Constructors

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatEventDownload( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since NCP 5.0
        */
        void ConstructL();


    public: // New functions

        /**
        * Command handler for eventdownload IPC message.
        * @since Series 60 Series60_2.6
        * @param aTsyReqHandle TsyReqHandle
        * @param aIpc command identifier
        * @param aPackage Data pointer
        * @return status
        */
        TInt ExtFunc( const TTsyReqHandle aTsyReqHandle,
            const TDataPackage& aPackage );

        /**
        * Cancels IPC request.
        * @since Series 60 Series60_2.6
        * @param aReqHandle notify request handle
        * @return status
        */
        TInt Cancel( const TTsyReqHandle aReqHandle );

        /**
        * Handles an incoming command from phonet
        * Called by CSatMessHandler when it receives event download command
        * ISI msg.
        * @since Series 60 Series60_3.0
        * @param aIsiMessage proactice command from phonet
        * @return none
        */
        TInt MessageReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Sets on events
        * @since Series 60 Series60_2.6
        * @param aEvent  event mask
        * @return none
        */
        void SetUpEventList( TUint32 aEvents );

        /**
        * Method to set internal flag to EFalse or ETrue.
        * @since Series 60 Series60_2.6
        * @param aStatus EFalse or ETrue
        * @return none
        */
        void SetSetUpCallStatus( const TBool aStatus );

        /**
        * Access Technology change
        * Send Access technology change envelope to SIM
        * @since Series 60 Series60_3.0
        * @return phonet sender status
        */
        TInt SendAccessTechnologyChangeEnvelope( TUint8  accTechChange );

#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
        /**
        * Force Location status envelope sending straight after SET UP EVENT
        * LIST Terminal response, if Location status is part of event list and
        * INFO_PP_ENHANCED_NETWORK_SELECTION PP flag is enabled
        * @return phonet sender status
        */
        TInt ForceLocationStatusEnvelope();
#endif //INFO_PP_ENHANCED_NETWORK_SELECTION

    private:    // New methods

        /**
        * Send MT Call envelope to SIM
        * @param aTransactionId  gsm transaction id
        * @param aDiallingNumber dialling number
        * @param aCalledPartySubAddress subaddress
        * @return phonet sender status
        */
        TInt SendMTCallEnvelope( TUint8 aTransactionId,
                const TDesC8& aDiallingNumber,
                const TDesC8& aCalledPartySubAddress );

        /**
        * Send call connected envelope to SIM
        * @param aTransactionId  gsm transaction id
        * @param aMtCall flag true if MT call
        * @return phonet sender status
        */
        TInt SendCallConnectedEnvelope( TUint8 aTransactionId, TBool aMtCall);

        /**
        * Send call disconnected envelope to SIM
        * @param aTransactionId gsm transaction id
        * @param aMtCall flag true if MT call
        * @return phonet sender status
        */
        TInt SendCallDisconnectedEnvelope( TUint8 aTransactionId,
            TBool aMtCall, TBool aCauseGiven, const TDesC8& aCause);

        /**
        * Send location status envelope to SIM
        * @since NCP 5.0
        * @param aNewLocInfo Recently updated location info
        * @return phonet sender status
        */
        TInt SendLocationStatusEnvelope(
                const CSatMessHandler::TLocationInfo& aNewLocInfo );

        /**
        * Send user activity envelope to SIM
        * @since Series 60 Series60_2.6
        * @return phonet sender status
        */
        TInt SendUserActivityEnvelope();

        /**
        * Send idle screen available envelope to SIM
        * @since Series 60 Series60_2.6
        * @return phonet sender status
        */
        TInt SendIdleScreenAvailableEnvelope();


        /**
        * Send browser termination envelope to SIM
        * @since Series 60 Series60_2.6
        * @return phonet sender status
        */
        TInt SendBrowserTerminationEnvelope(
            RSat::TBrowserTerminationCause aCause);


        /**
        * Send language selection envelope to SIM
        * @since Series 60 Series60_2.6
        * @return phonet sender status
        */
        TInt SendLanguageSelectionEnvelope( TUint16 iLanguage );


        /**
        * Create transaction ID required for sending ISI message.
        * @since Series 60 Series60_2.6
        * @return ISI msg transaction ID
        */
        TUint8  GetTransactionId();

        /**
        * Handle CALL_MODEM_MESSAGE_IND indication
        * @param aIsiMessage call status ISI msg
        */
        void CallModemMessageIndReceived( const TIsiReceiveC& aIsiMessage );

        /**
        * Bearer Independent Protocol
        * Send data available envelope to SIM. The SIM may sent a
        * ReceiveData PCmd after a DataAvailable event
        * @since Series 60 Series60_2.6.
        * @param aStatus channel status
        * @param aLength data length to be received
        * @return phonet sender status
        */
        TInt SendDataAvailableEnvelope( RSat::TChannelStatus aStatus,
            TInt8 aLength );

        /**
        * Bearer Independent Protocol
        * Send channel status envelope to SIM
        * @since Series 60 Series60_2.6
        * @param aStatus channel status
        * @return phonet sender status
        */
        TInt SendChannelStatusEnvelope( RSat::TChannelStatus aStatus );


    private:    // Nested class

        // CLASS DECLARATION
        /**
        *  TSatEventList
        *  Handles event list management for mmsat, used to keep track of events
        *  that need to be send to USIM.
        *  @lib SIMATKTSY.LIB
        *  @since Series 60 Series60_2.6
        */
        class TSatEventList
            {
            public:

                /**
                * C++ constructor
                */
                TSatEventList() : iEvents(0) {}

                /**
                * Check if event is ín the set of enabled events
                * @since Series 60 Series60_2.6
                * @param aEvent event to check
                * @return true if the event is enabled
                */
                TBool IsEnabled(RSat::TEventList aEvent) const
                    {
                    return (iEvents & TUint32(aEvent) ) ? ETrue : EFalse;
                    }

                /**
                * Remove event from set of enabled events
                * @since Series 60 Series60_2.6
                * @param aEvent event to be removed
                * @return none
                */
                void Remove(RSat::TEventList aEvent)
                    {
                    iEvents &= ~TUint32(aEvent);
                    }

                /**
                * Add event to set of enabled events
                * @since Series 60 Series60_2.6
                * @param aEvent event to be added
                * @return none
                */
                void Add(RSat::TEventList aEvent)
                    {
                    iEvents |= TUint32(aEvent);
                    }

                /**
                * Set the list of enabled events
                * @since Series 60 Series60_2.6
                * @param aEvent bit mask containing events
                * @return none
                */
                void Set(TUint32 aEvent)
                    {
                    iEvents = aEvent;
                    }

            private: // Data for nested class

                // set of events
                TUint32 iEvents;
        };


    private:    // Data

        // Pointer to message handling class
        CSatMessHandler*                iSatMessHandler;

        // Pointer to messaging class
        CTsySatMessaging*               iSatMessaging;

        // Event list management
        TSatEventList                   iEventList;

        // Location information
        CSatMessHandler::TLocationInfo  iLocInfo;

        // Flag for SetUpCall
        TBool                           iSetUpCallOngoing;

        // Flag for SetUpEventList PCmd. Location status envelope is expected
        // every time after SetUpEventList PCmd eventhough the contents are
        // the same as in previously send envelope. Therefore forcing flag.
        TBool                           iForceLocationStatusEnvelope;

        // Flag for call server activity, i.e. ongoing calls
        TBool                           iCallsOngoing;

        // Flag for sending possible pending location status envelope
        TBool                           iLocationStatusEnvelopePending;
        
        // Flag for describing is NET_CELL_INFO_IND received. Needed when
        // forced location envelope is sent. If NET_CELL_INFO_IND is not
        // received, forced location envelope is not sent.
        TBool                           iNetCellInfoIndReceived;
};


#endif // SATEVENTDOWNLOAD_H


// End of file
