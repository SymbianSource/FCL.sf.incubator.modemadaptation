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



#ifndef DPBREAK_H
#define DPBREAK_H

//  INCLUDES
#include <e32base.h>            

// LOCAL CONSTANTS
// none

// DATA TYPES
// none

// FORWARD DECLARATIONS
class CDpDataPort;
class CDpDataBuffer;

// CLASS DECLARATION

/**
*  CDpBreak is a break handler for break signals, which are received from DCS.
*  CDpBreak handles different types of break signals and notifies the C32 client
*  about them. Break signal type can be configured from DCS. Break signal type
*  can be PEP_COMM_BREAK_DESTRUCT, PEP_COMM_BREAK_EXPEDITED or
*  PEP_COMM_BREAK_NON_EXPEDITED. Handling is different for different break
*  signal types.
*/
class CDpBreak :public CBase
    {
    public:  // Methods

	// Constructors and destructor

        /**
        * C++ default constructor.
		* @param aDataPort Dataport main class
        */
        CDpBreak( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpBreak();


		void Init();
    public: // New methods

        /**
        * This method completes the received break signal to the C32 client.
        * Handling for different types of break signals:
        * PEP_COMM_BREAK_DESTRUCT: Notifies the client and then flushes receive
        * (RX) buffer.
        * PEP_COMM_BREAK_EXPEDITED: Notifies the client and then sends data to
        * the client.
        * PEP_COMM_BREAK_NON_EXPEDITED: Sends data to the client and then
        * notifies the break to the client.
        * @return TInt:Error code
		*/
        TInt CompleteBreakNotify();

        /**
        * This method sets break notify status active or deactive. Break notify
        * status indicates, whether a received break signal is notified to the
        * client. 
        * @param aBreakNotify Break notify status
        */
        void SetBreakNotify(const TBool aBreakNotify);

        /**
        * This methods gets break notify status, which indicates whether a
        * received break signal is notified to the client.
        * @param aBreakNotify 
        * On return, aBreakNotify reference contains break
        * notify status
        */
        void GetBreakNotify(TBool& aBreakNotify) const;

        /**
        * Send break to DCS. This is called from CDpDataPort::Break() method.
        * @return TInt:Error code
        */
        TInt SendBreak();


    private:    // Data

        /** DataPort */
        CDpDataPort&        iDataPort;

        /** Receive (RX) buffer. */
        CDpDataBuffer&      iRx;

        /** Break notify status */
        TBool               iBreakNotify;

    };

#endif      // DPBREAK_H

//  End of File
