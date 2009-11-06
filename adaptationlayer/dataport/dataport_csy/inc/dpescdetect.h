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



#ifndef DPESCDETECT_H
#define DPESCDETECT_H

//  INCLUDES
//  none

// LOCAL CONSTANTS
// none


// DATA TYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  ?one_line_short_description of a class.
*  ?other_description_lines
*/
class CDpEscDetect :public CActive
    {
    public:  // Methods

	// Constructors and destructor
        /**
        * C++ default constructor.
		* resets Esc Sequence Timer and reset Escape Sequence Character Time Array.
        * Adds the CActive object to Scheduler
        */
        CDpEscDetect( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpEscDetect();

    public: // New methods

        /**
        * This method scans escape sequence from data. There must be time (>gt)
        * between last received data and first escape character. Escape
        * characters must come with time between them, which is smaller than gt.
        * And finally time between last escape sequence character  and next data
        * must be larger than gt. We start timer, when last escape sequence
        * character has been received. Timer is completed in RunL(), if gt
        * expires.
        * @param aDes Data descriptor
        * @return TInt: Error code
	    */
        TInt Scan( TPtr8& aDes );

        /**
        * This method sends escape indication to DCS.
        */
        void SendEscapeIndication();

	// Methods from base classes

        // From CActive

        /**
        * From CActive 
        * This method is signalled, when timer after last escape
        * sequence character has expired. If this happens, we are sure that it
        * was correct escape sequence and we send escape sequence indication to
        * DCS.
        */
        void RunL();

        /**
        * From CActive 
        * This method handles leave in RunL(). This method returns
        * the given error value.
        * @return TInt: Error code
        */
	    TInt RunError(TInt aError);

        /**
        * From CActive This method cancels active request.
        */
        void DoCancel();

    private: // Methods

    // New methods

        /**
        * This method checks latest timing of escape characters.
        * @param aLatestIndex Index of latest timing
        * @return Time interval acceptable?
		* <BR><B>Name of return value:</B> Time interval acceptable?
        * <BR><B>Type of return value:</B> Boolean
        */
        TBool CheckEscCharIntervals( const TInt aLatestIndex );

    private:    // Data
		/** DataPort */
        CDpDataPort&    iDataPort;

        /** Port data configuration */
        CDpDataConfig&  iDataConfig;

        /** Timer for escape sequence detection */
        RTimer          iEscTimer;

        /** Index of latest character */
        TInt            iLatestIndex;
        
        /** Is timer running */
        TBool           iTimerRunning;

        /** Array for escape sequence characters */
        TInt            iEscCharTimeArray[3];

        /** How many characters received */
        TInt            iHowManyEscCharsReceived;

        /** Time to previously received message */
        TTime           iTimeToPrevReceivedMessage;

    };

#endif      // DPESCDETECT_H

//  End of File
