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



#ifndef DPTX2PN_H
#define DPTX2PN_H

//  INCLUDES
#include <e32base.h>                // e32 user library
#include "dpdatabuffer.h"           // base class for rx and tx buffers
#include "dpobserver.h"             // observer base class
#include "dpdataclient.h"			// data client that access buffer

// LOCAL CONSTANTS
// none


// DATA TYPES
// none

// FORWARD DECLARATIONS
// none


// CLASS DECLARATION

/**
*  CDpTx2Pn abstracts data flow from TX buffer into Isa Api. RunL() writes data
*  to Isa Api and signals ourselves (Tx2Pn), if more data needs to be written.
*  CDpTx2Pn is signalled from Dte2Tx. CDpTx2Pn is also signalled, when PIF or
*  flow control handler update us. PIF and flow control handler are observed
*  with CDpTx2PnObserver object.
*/
class CDpTx2Pn : public CActive, public MDpDataClient
    {
    private:

        class CDpTx2PnObserver : public CDpObserver
            {
            public:
                void UpDate( CDpSubject* aChangedSubject );
                void Init( CDpTx2Pn* aDpTx2Pn );
            private:
                CDpTx2Pn* iDpTx2Pn;
            };

    public:  // Methods

	// Constructors and destructor

        /**
        * Static constructor.
		* @param aDataPort Dataport main class
        */
	    static CDpTx2Pn* NewL( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpTx2Pn();

    public:  // New methods

        /**
        * This method writes data to Isa Api. If break is pending, the amount of
        * data to be written is maximum of break bytes and maximum reservation
        * size (MMRS). If break is not pending, the amount is maximum of bytes
        * in TX buffer and MMRS. Read data element is reserved, before actual
        * write.
        * @return Error code
	    */
        TInt WritePn();

        /**
        *  This method sets up flag to indicate, whether we are requested to
        *  notify the C32 client, when output buffer (TX) is emptied.
        * @param aUsedOrNot Notify to enabled or disabled?
        */
        void SetOutputBufferEmptyNotification( const TBool aUsedOrNot );

        /**
        * This method writes data to Isa Api. In 7-bit data case paritybits are
        * added here using parity handler.
        * @param aTx Data
        * @return Error code
	    */
        TInt PipeSendWithParityAdd( TPtr8& aTx );

        /**
        * This method sets break bytes and flag to indicate that there is data
        * to be send before break.
        * @param aBreakBytes Amount of break bytes
        */
        void SetBreakBytes( const TInt aBreakBytes );

        /**
        * This method resets break bytes.
        */
        void CancelBreakBytes();

        /**
        * This method signals ourselves (Tx2Pn), if there is data in TX buffer
        * and Dp2Dcs flow control is not ON.
        * @param aChangedSubject Not used
        */
        void UpDate( CDpSubject* aChangedSubject );

	// Methods from base classes

        // From CActive

        /**
        * From CActive This method cancels pending request with KErrCancel.
        */
        void DoCancel();

        /**
        * From CActive This method writes data to Isa Api, if Dp2Dcs flow
        * control is OFF. This method also notifies the C32 client about
        * emptied TX buffer, if the buffer is empty and the client has ordered
        * notification. After write, active object is set active again. If there
        * are still bytes in TX buffer and flow control is OFF, we (Tx2Pn) are
        * signalled again.
        */
        void RunL();

        /**
        * From CActive This method handles leave in RunL(). This method returns
        * the given error value.
        * @param aError Leave reason
        * @return Leave reason is returned in error code
	    */
	    TInt RunError(TInt aError);

        // From CDpDataClient

        /**
        * From CDpDataClient This method signals this active object.
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

    private: // Methods

	// Constructors and destructor
        /**
        * C++ default constructor.
		* @param aDataPort Dataport main class
        */
        CDpTx2Pn( CDpDataPort& aDataPort );

        /**
        * Symbian 2nd phase constructor.
        */
	    void ConstructL();

    // New methods

        /**
        * This method informs, whether flow control is ON. This method informs
        * that flow control is ON also, when the pipe is not enabled.
        * @return Is flow control ON?
	    */
        TBool IsFlowCtrlOn();

        /**
        * This method handles return value of data element reservation in
        * WritePn(). This method writes data from reserved buffer area and
        * possible tail to Isa Api with PipeSendWithParityAdd(). After write,
        * data elemnt is released. If break was pending and break bytes are
        * sent, break signal is sent and break request completed.
        * @param aReturnValue Return value from WritePn()
        * @return Error code
	    */
	    TInt HandleReturnValue( const TInt aReturnValue );

    private:    // Data

		// Serial protocol port
        CDpDataPort&        iDataPort;

		// Break
		CDpBreak&			iBreak;

        // TX buffer
        CDpDataBuffer&      iBufferTx;

        // Pipe interface (PIF)
        CDpPif&				iPifDcs;

        // Flow control handler
        CDpFlowCtrl&        iFlowCtrl;

        // Port data configuration
        CDpDataConfig&      iDataConfig;

        // Parity handler
        CDpParityHandler&   iParityHandler;

        // Reserved TX buffer area
        TPtr8               iTx;

        // Reserved TX buffer area tail
        TPtr8               iTxTail;

        // Is break pending
        TBool               iBreakPending;

        // Amount of break bytes
        TInt                iBreakBytes;

        // Should emptying of buffer be notified to the C32 client
        TBool               iOutBufIsEmptyNotify;

        // Tx2Pn PIF observer observer
        CDpTx2PnObserver    iDpTx2PnPifObserver;

        // Tx2Pn flow control observer
        CDpTx2PnObserver    iDpTx2PnFcObserver;

		// Is request active
		TBool iRequestActive;

        // Role (EDbDataClientReader/EDbDataClientWriter)
        const TUint8 iRole;
 

#ifdef _DEBUG
        // Amount of total sent bytes
	    TInt                iTotalSent;
#endif

    };

#endif      // DPTX2PN_H

//  End of File
