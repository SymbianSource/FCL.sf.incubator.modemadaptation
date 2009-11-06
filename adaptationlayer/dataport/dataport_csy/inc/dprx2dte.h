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



#ifndef DPRX2DTE_H
#define DPRX2DTE_H

// INCLUDES
#include "dpdatabuffer.h"           // base class for rx and tx buffers
#include "dpdataclient.h"			// data client that access buffer

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
*  CDpRx2Dte abstracts data flow from RX buffer into the C32 client.
*  When CDpRx2Dte is signalled, CDpRx2Dte writes requested (from the C32
*  client) amount of data into client space. The request involves multiple
*  Rx2Dte signalling operations, if data needs to be written in parts.
*  CDpRx2Dte handles also echoing and terminator characters detection.
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CDpRx2Dte :public CActive, public MDpDataClient
    {
    public: // Constructors and destructor        

        /**
        * Two-phased constructor.
		* @param aDataPort Dataport main class
        */
	    static CDpRx2Dte* NewL( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpRx2Dte();

    public: // New methods

        /**
        * This method reserves read data element, and makes write into client
        * space. Amount of data to be written is maximum of requested length,
        * maximum reservation size (MMRS), and bytes before break.
        * @return Error code
        */
        TInt Write();

        /**
        * This method handles forwarded read requests from the C32 client. This
        * method also signals ourselves (Rx2Dte), if there is data in receive
        * (RX) or echo buffer.
        * @param aDes Pointer into client address space to the
        * descriptor containing the client’s buffer 
        * @param aLength The amount of data to be written into client space
        * @return Error code
        */
        TInt SetDteRead( const TAny* aDes, const TInt aLength );

        /**
        * This method handles forwarded read cancellation request from the C32
        * client. This method cancels read operation.
        * @return Error code
        */
        TInt ResetDteRead();

        /**
        * This method informs, whether read is pending.
        * @return Is read pending?
        */
        inline TBool ReadPending();

        /**
        * This method informs the wanted length.
        * @return How much data the C32 client requested?
        */
        inline TInt LengthWanted();

        /**
        * This method sets amount of break bytes to be sent before break.
        * If amount of break bytes is zero and the client has ordered
        * break notify, we complete break notify.
        * @param aBreakBytes Amount of break bytes
        */
        void SetBreakBytes( const TInt aBreakBytes );

        /**
        * We are requested to write TX data back to client buffer. This method
        * appends data to echo buffer, and signals ourselves (Rx2Dte).
        * @param aEchoData Echo data Reference
        */
        void EchoTx( const TPtr8& aEchoData );

        /**
        * This method gets the amount of echo bytes.
        * @return Amount of echo bytes
        */
        TInt EchoBytes();

        /**
        * This method gets the amount of free bytes in echo buffer.
        * @return Amount of free space
        */
        TInt EchoBytesFreeSpace();

        /**
        * This method resets echo bytes.
        */
        void ResetEchoBytes();

        /**
        * This method gests write offset.
        * @return Write offset
        */
        inline TInt IPCWriteOffset();

    public: // Methods from base classes

        /**
        * From CActive. This method cancels pending request with KErrCancel.
        */
        void DoCancel();

        /**
        * From CActive. This method handles writing of data into client space.
        */
        void RunL();

        /**
        * From CActive. This method handles leave in RunL() and returns
        * the given error value.
        * @param aError Leave reason
        * @return Leave reason is returned in error code
        */
	    TInt RunError( TInt aError );

        /**
        * From CDpDataClient. This method signals ourselves (Rx2Dte).
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

    protected: // New methods
        
        // none

    protected: // Methods from base classes

        // none

    private: // Methods

        /**
        * C++ default constructor.
		* @param aDataPort Dataport main class
        */
	    CDpRx2Dte( CDpDataPort& aDataPort );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

        /**
        * This method writes echo bytes back to the C32 client and signals
        * Dte2Tx. This method completes read to the client, if the client
        * has read with ReadOneOrMore() or if client requested length or
        * client buffer is full.
        * @return Error code
        */
        TInt WriteEchoToClient();

        /**
        * Writes bytes into the client space. This method also handles
        * low water mark, if Dcs2Dp flow control is ON and low water 
        * mark is reached.
        * @return Error code
        */
	    TInt WriteIntoClientSpace();

        /**
        * This method finds maximum length. i.e. which one is smaller: maximum
        * reservation size (MMRS) or client buffer where we write.
        * @return Maximum length in bytes
        */
	    TInt FindMaximumLength();

        /**
        * This method returns amount of break bytes, if break is pending. Amount
        * of used bytes is returned, if break is not pending. If the amount
        * exceeds given length, the length is returned.
        * @param aMaxLen Maximum length
        * @return Length
        */
	    TInt FindLength( const TInt aMaxLen );

        /**
        * This method makes actual write operation into client space. This
        * method writes also tail part of data element, if it exists. Data
        * is written only 'till terminator character, if terminator characters
        * are used. Read is completed to the C32 client, if the client has read
        * with ReadOneOrMore(), if terminator character was found, or if client
        * requested length or client buffer is full.
        * Reserved data element is released.
        * @param aLen Amount of bytes to be written
        * @return Error code
        */
	    TInt MakeRx2DteWrite( const TInt aLen );

        /**
        * This method writes the tail into client space. Data is written only
        * till terminator character, if terminator characters are used.
        * @param aInd On return, aInd contains index of terminator character
        * @param aTermCount Terminator characters count
        * @return Error code
        */
	    TInt WriteTail( TInt& aInd, const TInt aTermCount );

    public:     // Data
        // none
    
    protected:  // Data
        // none

    private:    // Data
		// DataPort
        CDpDataPort&    iDataPort;

        // RX buffer
        CDpDataBuffer&  iBufferRx;

		// Flow control handler
        CDpFlowCtrl&    iFlowCtrl;

		// Port data configuration
        CDpDataConfig&  iDataConfig;

		// Break signal handler
        CDpBreak&       iBreak;

		// Termination characters detector
        CDpTermDetect&  iTermDetect;

        // Reserved RX buffer area
        TPtr8   iRx;

        // Reserved RX buffer area tail
        TPtr8   iRxTail;

        // Pointer to client buffer
        TAny*   iClientBuffer;

        // Client buffer length
        TInt    iClientBufferLength;

        // Requested length.
        TInt    iClientRequestedLength;

        // Is read pending?
        TBool   iReadPending;

        // Is break pending?
        TBool   iBreakPending;

        // Amount of break bytes
        TInt    iBreakBytes;

        // Write offset
        TInt    iIPCWriteOffset;

        // Echo buffer
        TBuf8<KDpMaximumEchoDataSize> iEchoData;
		
		// Is request active
		TBool iRequestActive;

        // Role (EDbDataClientReader/EDbDataClientWriter)
        const TUint8 iRole;

#ifdef _DEBUG
        // Amount of total received bytes
	    TInt iTotalReceived;
#endif

    };

#include "dprx2dte.inl"   // inline methods

#endif      // DPRX2DTE_H

// End of File
