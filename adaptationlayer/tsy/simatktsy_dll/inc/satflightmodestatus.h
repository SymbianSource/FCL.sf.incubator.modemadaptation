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



#ifndef SATFLIGHTMODESTATUS_H
#define SATFLIGHTMODESTATUS_H

// INCLUDES
#include <e32base.h>                // class cactive
#include <e32property.h>            // class rproperty

// CONSTANTS
    //None

// MACROS
    //None

// DATA TYPES
    //None

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATION
class CSatMessHandler;
class CTsySatMessaging;

// CLASS DECLARATION

/**
* CSatFlightModeStatus
* SimAtkTSY Flight mode support Active object for checking flight mode status
* values.
*/
class CSatFlightModeStatus : public CActive
    {
    public:     // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to TsySatMessaging class
        */
        static CSatFlightModeStatus* NewL( 
            CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * Destructor.
        */
        virtual ~CSatFlightModeStatus();

    private:    // Constructors

        /**
        * C++ constructor.
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatFlightModeStatus(
            CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    public:     // New functions

        /**
        * Flight mode status value
        * @return Flight mode status value, True or False.
        */
        TBool FlightModeStatusValue();

    public:     // Functions from base classes
        //None

    protected:  // New functions
        //None

    protected:  // Functions from base classes
        //None

    private:    // New functions

        /**
        * Checks Flight mode statuses and sends indication of status state
        * changes
        */
        void CheckStatusChange();

    private:    // Functions from base classes

        /**
        * Activates active object when request is completed.
        */
        void RunL();

        /**
        * When RunL leaves, RunError is called.
        * @param TInt aError: Error from RunL
        * @return TInt: KErrNone
        */
        TInt RunError( TInt aError );

        /**
        * From CActive
        * Cancels active object request.
        */
        void DoCancel();

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Satmesshandler class, responsible for
        // sending ISI messages to via NokiaTSY CMmPhonetSender and receiving
        // ISI messages via Nokia TSY CMmPhonetReceiver.
        CSatMessHandler*    iSatMessHandler;

        // This object receives and sends messages to the external world
        // using the message handler and messaging classes.
        CTsySatMessaging*   iSatMessaging;

        // Flight mode status value. True if Flight Mode is ON.
        TBool               iFlightModeActive;

        // Handling to Pub sub key
        RProperty           iProperty;

    public:     // Friend classes
        //None

    protected:  // Friend classes
        //None

    private:    // Friend classes
        //None

    };

#endif      // SATFLIGHTMODESTATUS_H

// End of File
