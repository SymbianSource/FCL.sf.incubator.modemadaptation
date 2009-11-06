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



#ifndef DPDTE2TX_H
#define DPDTE2TX_H


//  INCLUDES
#include "dpdatabuffer.h"       // base class for rx and tx buffers
#include "dpobserver.h"         // observer base class
#include "dpdataclient.h"		// data client that access buffer

// LOCAL CONSTANTS
// none


// DATA TYPES
// none


// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  CDpDte2Tx abstracts data flow from the C32 client into TX buffer. CDpDte2Tx
*  signals itself, when data is written into TX buffer. RunL() handles actual
*  write operation.
*/
class CDpDte2Tx : public CActive, public MDpDataClient
    {
    private:

        class CDpDte2TxObserver : public CDpObserver
            {
            public:
                void UpDate( CDpSubject* aChangedSubject );
                void Init( CDpDte2Tx* aDpDte2Tx );
            private:
                CDpDte2Tx* iDpDte2Tx;
            };

    public:  // Methods

	// Constructors and destructor
        /**
        * Static constructor.
		* @param aDataPort Dataport main class
        */
        static CDpDte2Tx* NewL( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpDte2Tx();

	public:  // New methods

        /**
        * This method puts write pending and signals itself (Dte2Tx).
        * @param aClientBuffer Pointer to client buffer
        * @param aLength The amount of data to be written
        * @return Error code
	    */
        TInt WriteTx( const TAny* aClientBuffer, const TInt aLength );

        /**
        * This method cancels a pending write operation. RunL() does nothing,
        * if write is not pending.
        */
        void CancelWrite();

    // Methods from base classes

        // From CActive

        /**
        * From CActive This method cancels pending request with KErrCancel.
        */
        void DoCancel();

        /**
        * From CActive This method handles write operation after signalling.
        * This method does nothing, if write is not pending or is cancelled. 
        * Buffer element is reserved, data is written to TX buffer and
        * buffer element is released. Echo data is echoed to echo buffer.
        * Escape sequence is scanned from data. Write is completed to the C32
        * client, if all data has been written into TX buffer. Otherwise
        * active object's request is put back to pending.
        */
        void RunL();

        /**
        * From CActive This method handles leave in RunL(). This method returns
        * the given error value.
        * @param aError Leave reason
        * @return Leave reason is returned in error code
	    */
	    TInt RunError( TInt aError );

        // From CDpDataClient

        /**
        * From CDpDataClient ?member_description.
        */
        void ReleaseNotify();

        /**
        * This method notifies that buffer is flushed. Classes which have access
        * to buffers are derived from this class. Derived class could override
        * this method, otherwise this default method will be used.
		* release this cactive object
        */
        void FlushNotify();

        /**
        * This method gets the role (reader/writer) of data client.
        * @return Role (EDbDataClientReader/EDbDataClientWriter)
        */
        TUint8 Role();

        // From CDpObserver
    
        /**
        * From CDpObserver We are observer of FlowCtrl. E.g. when flow control
        * status changes, we are called. 
        * @param aChangedSubject Not used
        */
        void UpDate( CDpSubject* aChangedSubject );

    private: // Methods

    // Constructors and destructor
        /**
        * C++ default constructor.
		* @param aDataPort Dataport main class
        */
        CDpDte2Tx( CDpDataPort& aDataPort );
	    
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:    // Data

		// Serial protocol port
        CDpDataPort&    iDataPort;

		// TX buffer
        CDpDataBuffer&  iBufferTx;

    	// Flow control handler
        CDpFlowCtrl&    iFlowCtrl;

        // Dte2Tx flow control observer
        CDpDte2TxObserver iDpDte2TxFcObserver;

		// Escape sequence detector
        CDpEscDetect&   iEscDetect;

		// Port data configuration
        CDpDataConfig&  iDataConfig;

		// Moves data from RX buffer to client
        CDpRx2Dte&      iRx2Dte;

		// Reserved TX buffer area
        TPtr8           iTx;

		// Is write pending
        TBool           iWritePending;

		// Pending client buffer
        TAny*           iPendingClientBuffer;

		// Length of client buffer
        TInt            iPendingLength;

		// Read offset
	    TInt			iIPCReadOffset;
		
		// Is request active
		TBool iRequestActive;

        // Role (EDbDataClientReader/EDbDataClientWriter)
        const TUint8 iRole;
    };

#endif      // DPDTE2TX_H

//  End of File
