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



#ifndef CSATTIMER_H
#define CSATTIMER_H


//  INCLUDES
#include <e32base.h>        // base class cbase


// CONSTANTS
const TInt KEnvelopeMaxSize             = 0xFF;


// FORWARD DECLARATIONS
class CSatMessHandler;
class CTsySatMessaging;


// CLASS DECLARATION

/**
*  Sat Timer class.
*  This class handles timers in SAT. Class utilizes CPeriodic timer
*  to provide desired functionality.
*
*  @lib SIMATKTSY.LIB
*  @since Series 60 Series60_2.6
*/
class CSatTimer : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        static CSatTimer* NewL( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );
        
        /**
        * Destructor.
        * @since NCP 5.0
        */
        virtual ~CSatTimer();
        
        
    private:    // Constructors

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatTimer( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since NCP 5.0
        */
        void ConstructL();        
    

    public: // New functions

        /**
        * Starts the timer with the provided id and duration.
        * If there is already timer with same id, older is replaced by the
        * new one
        * @since Series 60 Series60_2.6
        * @param aTimerId ID of timer
        * @param aTimerValue duration of timer in seconds
        * @return KErrNone or error code
        */ 
        TInt StartTimer( TInt aTimerId, TUint32 aTimerValue );


        /**
        * Deletes timer from the table
        * @since Series 60 Series60_2.6
        * @param TInt aTimerId: timer identifier
        * @return TInt: KErrNone or KErrNotFound 
        */
        TInt DeleteTimerById( TInt aTimerId );

        /**
        * Gets current value of timer by timer id
        * @since Series 60 Series60_2.6
        * @param TInt aTimerId: timer identifier
        * @return TUint32: Current timer value 
        */
        TUint32 GetCurrentValueOfTimerById( TInt aTimerId );

        /**
        * Sets status for ProactiveCommandOnGoing flag
        * @since Series 60 Series60_2.6
        * @param TBool aStatus: status for proactiveCommand
        * @return None
        */
        void SetProactiveCommandOnGoingStatus( TBool aStatus );
        
        /**
        * Clears the timer table. Used when SIM is reset.
        * @since Series 60 Series60_3.0
        * @param None
        * @return None
        */
        void ClearTimerTable();
        
        /**
        * Stores the envelope for possible need of resending
        * @since Series 60 Series60_3.0
        * @param TUint8 aTId Transaction id of envelope about to be sent
        * @param TDesC8& aEnvelope Envelope data
        * @return KErrNone or KErrAlreadyExists if envelope with same 
        *   transaction id exists.
        */
        TInt StoreEnvelope( const TUint8 aTId, const TDesC8& aEnvelope );

        /**
        * Activates the resending stored envelope. This will happen if the
        * sending of envelope failed and error code returned from the SIM
        * is supported by the CSatTimer.
        * @since Series 60 Series60_3.0
        * @param TUint8 aTId Transaction id of envelope to be activated
        * @param TUint8 aCause Error code returned from SIM (SW1)
        * @return KErrNone. KErrNotFound if envelope with given id is not 
        *   found. KErrNotSupported if usupported cause was given.
        */
        TInt ActivateEnvelopeResend( const TUint8 aTId, const TUint8 aCause );

        /**
        * Removes the requested envelope from the envelope table.
        * @since Series 60 Series60_3.0
        * @param TUint8 aTId Transaction id of envelope about to be removed.
        * @param TBool aRemoveTimer If the envelope typ is a timer expiration, 
        *   It shall be removed also from internal timer table as a default.
        * @return KErrNone or KErrNotFound if envelope with given id coun't be
        *   found.
        */
        TInt RemoveEnvelope( const TUint8 aTId, TBool aRemoveTimer = ETrue );


    private:

        /**
        * Callback function that shall be called when timer is active.
        * @since Series 60 Series60_3.0
        * @param TAny* aSatTimer Reference to SatTimer
        * @return None
        */
        static TInt Tick( TAny* aSatTimer ); 

        /**
        * Starts the timer if it's not already running.
        * @since Series 60 Series60_3.0
        * @param None
        * @return None
        */
        void Start();

        /**
        * Stops the timer counting if there are no active timers
        * or delayed envelopes 
        * @since Series 60 Series60_3.0
        * @param None
        * @return None
        */ 
        void Stop();    

        /**
        * Restarts the deactivated Sat timer.
        * @since Series 60 Series60_3.0
        * @param aTimerId Id of timer to be restarted-
        * @return KErrNone or KErrNotFound if timer with given id wasn't found
        */ 
        TInt RestartTimer( TInt aTimerId );            

        /**
        * Returns true if the timer exists in the timer table.
        * @since Series 60 Series60_3.0
        * @param TInt aTimerId: timer identifier 
        * @return TBool : ETrue if the timer has an entry in the table
                         EFalse otherwise.
        */
        TBool CheckTimerTable( TInt aTimerId );

        /**
        * Locates the position of the requested envelope within envelope table.
        * @since Series 60 Series60_3.0
        * @param TUint8 aTId Transaction id of envelope about to be located.
        * @return KErrNone or KErrNotFound if envelope with given id coun't be
        *   found.
        */
        TInt LocateEnvelope( const TUint8 aTId );
        
        
    private: // Enumerations, data types
        
        // Envelope types        
        enum TEnvelopeType
            {
            ETypeUnknown            = 0xD0,
            ESmsPpDownload          = 0xD1,
            ECellBroadcast          = 0xD2,
            EMenuSelection          = 0xD3,
            ECallControl            = 0xD4,
            EMoShortMessageControl  = 0xD5,
            EEventDownload          = 0xD6,
            ETimerExpiration        = 0xD7
            };
            
        // Supported causes for envelope resending
        enum TCause
            {
            ESimBusy                = 0x93,
            ERefManagement          = 0x94
            };        

        // Envelope struct
        struct TSatEnvelope
            {
            TUint8  iTId;       // Transaction id
            TUint8  iType;      // Type of envelope
            TUint8  iDelay;     // Delay in ticks
            TBool   iActive;    // Is resending active
            TBuf8<KEnvelopeMaxSize> iEnvelope;
            };
            
    private: // Private nested class 
           
        class TTimer
            {
            public: // Enumerations
            
            enum TState
                {
                ETicking,       // When timer is ticking
                EBeingSent,     // When sending envelope
                EWaitingForTr   // When sent during pcmd
                };
            
            
            public: // Methods
                /**
                * Non default Constructor
                * @since Series 60 Series60_2.6
                * @param TInt iTimerId
                * @param TUint32 iStartTime
                * @param TUint32 iTimeStamp
                */
                TTimer( TInt iTimerId,
                        TUint32 iStartTime,
                        TUint32 iTimeStamp );

                /**
                * returns iTimeStamp from a TTimer
                * @since Series 60 Series60_3.0
                * @return TUint32: value of the iTimeStamp attribute
                */
                TUint32 TimeStamp();

                /**
                * returns iTimerId from a TTimer
                * @since Series 60 Series60_3.0
                * @return TInt: value of the iTimerId attribute
                */
                TInt Id();

                /**
                * returns iStartTime from a TTimer
                * @since Series 60 Series60_3.0
                * @return TUint32: value of the iStartTime attribute
                */
                TUint32 StartTime();
                
                /**
                * Setter for timer's internal state.
                * @param TBool aState Desired state
                * @since NCP 5.1
                * @return none
                */                
                void SetState( TState aState );
                
                /**
                * Getter for timer's internal state.
                * @since NCP 5.1
                * @return State of a timer
                */                
                TState State();
                
                /**
                * Returns ETrue if two entries are equal
                * @since Series 60 Series60_3.0
                * @param const TTimer& arg1
                * @param const TTimer& arg2
                * @return TBool: ETrue if two entries are equal
                */
                static TBool CompareEntries( const TTimer& arg1,
                    const TTimer& arg2 );

                /**
                * Returns order between two entries
                * @since Series 60 Series60_3.0
                * @param const CTableEntry& arg1
                * @param const CTableEntry& arg2
                * @return Tint: 0 if arg1 = arg2
                                1 if arg1 > arg2
                               -1 else
                */
                static TInt OrderEntries( const TTimer& arg1,
                    const TTimer& arg2 );

            private: // Data members

                // Timer identifier
                TInt        iTimerId;
                
                // Timer start time
                TUint32     iStartTime;
                
                // Time stamp
                TUint32     iTimeStamp;
                
                // Timer state machine
                TState      iState;
                
            };        

    private: // Data members

        // Pointer to CSatMesshandler instance
        CSatMessHandler*        iSatMessHandler;

        // Pointer to CSatMessaging instance
        CTsySatMessaging*       iSatMessaging;
        
        // Counts the number of second passed since the timer was started
        TUint32                 iSecondsPassed;
        
        // Because timer ticks twice in a second, we need to keep track when
        // to update iSecondsPassed-member
        TBool                   iHalfSecondTick;    

        // Flag for proactiveCommandOnGoing status
        // True if proactive command is ongoing
        TBool                   iProactiveCommandOnGoing;

        // Pointer to the CPeriodic type timer. (ETel)
        CPeriodic*              iTimer; 

        // Pointer to the timer entry array
        RArray<TTimer>*         iTimerTable;
        
        // Pointer to the envelope entry array
        RArray<TSatEnvelope>*   iEnvelopeTable;   

    };

#endif //CSATTIMER_H

// End of File
