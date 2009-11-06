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



#ifndef DPPN2RX_H
#define DPPN2RX_H

//  INCLUDES
#include "dpobserver.h"             // observer base class
#include "dpdatabuffer.h"           // base class for rx and tx buffers
#include "dpdataclient.h"			// data client that access buffer

#include <iscapi.h>

// CONSTANTS
// none

// MACROS
// none

// DATA TYPES
// none

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  CDpPn2Rx abstracts data flow from Isc Api into RX buffer.
*  CDpPn2Rx signals Rx2Dte and renews read from Isc Api in RunL(),
*  when CDpPn2Rx is signalled. CDpPn2Rx contains CDpPn2RxObserver
*  object, which observers pipe interface (PIF).
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CDpPn2Rx : public CActive, public MDpDataClient
    {
    private:

        class CDpPn2RxObserver : public CDpObserver
            {
            public: // Methods
                void UpDate( CDpSubject* aChangedSubject );
                void Init( CDpPn2Rx* aDpPn2Rx );

            private: // Data
                CDpPn2Rx* iDpPn2Rx;
            };

        enum TDpPn2RxState
            {
            EDefault,
            EDpReadWait,
            EElementWait
            };

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
		* @param aDataPort Dataport main class
        */
    	static CDpPn2Rx* NewL( CDpDataPort& aDataPort );			   

        /**
        * Destructor.
        */
        ~CDpPn2Rx();

    public: // New methods

		/**
		* This method informs whether high water mark is reached.
		* @return Is high water mark reached?
		*/
		TBool IsHighWaterMarkReached();

        /**
        * This method is called, when read from Isc Api has completed. Data
        * element is released. If there is new data, parity bits are extracted
        * and Rx2Dte is signalled. If data notifying is active, the C32 client
        * is also notified. New data element is reserved, possible high water
        * mark is handled, and read from Isc Api is renewed.
        * @return Error code
        */
        TInt Read();

        /**
        * This method activates/deactivates data available notification.
        * @param aUsedOrNot Is notification enabled?
        * @return Error code
        */
        TInt SetDataAvailableNotification( const TBool aUsedOrNot );

        /**
        * Pipe state or flow control has been changed. This method renews read,
        * if pipe is enabled and we are not already reading. This method cancels
        * read, if pipe is disconnected.
        * @param aChangedSubject Not used
        */
        void UpDate( CDpSubject* /*aChangedSubject*/ );

    public: // Methods from base classes

        /**
        * From CActive. Cancels this active object and cancels pipe receive.
        */
        void DoCancel();

        /**
        * From CActive. This method handles a read from Isc Api.
        */
        void RunL();

        /**
        * From CActive. This method handles leave in RunL().
        * @param aError Leave reason
        * @return Leave reason is returned in error code
        */
	    TInt RunError(TInt aError);

        /**
        * From CDpDataClient. This method signals ourselves (Pn2Rx).
        */
        void ReleaseNotify();

        /**
        * From CDpDataClient. This method notifies that buffer is flushed.
        * Classes which have access to buffers are derived from this class.
        * Derived class could override this method, otherwise this default
        * method will be used.
        */
        void FlushNotify();

        /**
        * This method gets the role (reader/writer) of data client.
        * @return Role (EDbDataClientReader/EDbDataClientWriter)
        */
        TUint8 Role();

    protected:  // New methods
        
        // none

    protected:  // Methods from base classes

        // none

    private: // Methods

        /**
        * C++ default constructor.
        */
        CDpPn2Rx( CDpDataPort& aDataPort );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

    public:     // Data
        // none

    protected:  // Data
        // none

    private:    // Data
        // DataPort
        CDpDataPort&        iDataPort;

        // Receive (RX) buffer
        CDpDataBuffer&      iBufferRx;

        // Pipe interface (PIF)
        CDpPif&				iPifDcs;

        // Port data configuration
        CDpDataConfig&      iDataConfig;

        // Flow control handler
        CDpFlowCtrl&        iFlowCtrl;

        // Parity handler
        CDpParityHandler&   iParityHandler;

        // Reserved RX buffer area
        TPtr8               iRx;

        // Pn2Rx state
        TDpPn2RxState       iState;

        // Should received data be notified to the C32 client?
        TBool               iInBufHasDataNotify;

        // Pn2Rx observer
        CDpPn2RxObserver    iDpPn2RxObserver;
		
		// Is request active
		TBool iRequestActive;

        // Required buffer lenght received from ISC on read
        // completion in case of KErrNoMemory situation
		TUint16			iLen;

        // Role (EDbDataClientReader/EDbDataClientWriter)
        const TUint8 iRole;

    };

#endif      // DPPN2RX_H

// End of File
