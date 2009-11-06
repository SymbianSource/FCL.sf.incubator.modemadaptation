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



#ifndef DPDATABUFFER_H
#define DPDATABUFFER_H

//  INCLUDES
#include <e32base.h>        // e32 user library
#include "dpdataelement.h"	// data element

// LOCAL CONSTANTS
// none


// DATA TYPES
// none

// FORWARD DECLARATIONS
class CDpDataElement;
class CDpDataPort;
class MDpDataClient;

/**
*  CDpDataBuffer provides base class for RX/TX buffers inside DataPort.
*
*  CDpDataBuffer is circular buffer. The size of the buffer can be changed
*  dynamically. One buffer element (CDpDataElement) is used for writing into the
*  buffer and one element is used for reading from the buffer. Buffer element
*  must be reserved before use. Buffer element must be released, before the
*  buffer element can be used again.
* 
*  The buffer is divided into two physically continuous parts:
*  primary buffer space (PBS) and extended buffer space (EBS).
*
*  |-------pbs--------|--ebs--|     - PBS and EBS.
*
*  The size of PBS is at least two times MMRS (maximum reservation size) and the
*  size of EBS equals to MMRS. EBS allows write operation to reserve continuous
*  buffer space for the buffer element.
*
*  The treshold (tr) indicates PBS/EBS boundary.
*  The tail (t) indicates the oldest byte in the buffer.
*  The head (h) indicates the newest byte in the buffer.
*  The end (e) indicates the last byte in the buffer. The end equals the head
*  only, when the data does not continue from the beginning of the buffer.
*  
*  t     h,e          tr    
*  |XXXXX|            |  
*  |-------pbs--------|--ebs--|     - Tail, head and end, data does not exceed
*                                     the treshold.
*
*           t           h,e   
*           |XXXXXXXXXXX|
*  |-------pbs--------|--ebs--|     - Tail, head and end, when data exceeds
*                                     the treshold. Data has been read from the
*                                     buffer as tail is not anymore in the
*                                     beginning of the buffer.
*
*     h     t           e   
*   XX|     |XXXXXXXXXXX|
*  |-------pbs--------|--ebs--|     - Tail, head and end, when data continues
*                                     from the beginning of the buffer.
*
*  The tail when wrapped around (twr) stores the value of the tail, when the
*  buffer is wrapped around. The buffer is wrapped around, when reserved read
*  type buffer element wraps around the end of buffer. Tail is set to zero, when
*  tail when wrapped around is being used.
*
*  t  h     twr         e   
*  |XX|     |XXXXXXXXXXX|
*  |-------pbs--------|--ebs--|     - Tail, tail when wrapped around, head and
*                                     end, when tail is wrapped around.
*
*  CDpDataBuffer provides methods for read and write element getting, data
*  client registration and flushing. CDpDataBuffer provides also set of getter
*  methods for getting the state of the buffer.
*
*  CDpDataBuffer data members are protected, and CDpDataElement and CDpDataPort
*  are friend classes of CDpDataBuffer. This allows efficient use of data
*  members from other classes
*/
class CDpDataBuffer :public CBase
    {
    //ENUMERATION
    public:
        enum 
        	{
        	EDbReader,
        	EDbWriter
        	};

    public:  // Methods

	// Constructors and destructor
        /**
        * Static constructor.
		* @param aMaximumReservationSize - maximum reservation size (ebs)
		* @param aBufferInitialSize - initial buffersize (buffer)
        */
		static CDpDataBuffer* NewL( 
			const TInt aMaximumReservationSize,
			const TInt aBufferInitialSize );
        /**
        * Destructor.
        */
        ~CDpDataBuffer();

	public:// New methods

        /**
        * This method sets length of buffer. The size of the buffer is not
        * changed, if the requested buffer size is not greater than the
        * buffer's current length or if the requested buffer size is not at
        * least 2 times MMRS.
        * @param aLen Requested length of buffer
        * @return TInt: Error code
		*/
        TInt SetLength( const TInt aLen );

        /**
        * This method registers data client to the buffer. Classes using this
        * buffer are required to register themselves. Data client can be reader
        * or writer. Only one reader and writer can be registered to the buffer.
        * @param aClient Data client
        * @return TInt :Error code
		*/
        TInt RegisterDataClient( MDpDataClient& aClient );

        /**
        * This method gets the amount of used bytes.
        * @return TInt: amount of used bytes.
		*/
        TInt UsedBytes();

        /**
        * This method gets the amount of free bytes.
        * @return TInt: amount of free bytes.
		*/
        TInt FreeBytes();

        /**
        * This method empties the buffer and notifies waiting reader and writer
        * about the flush.
        */
        void Flush();

        /**
        * This method gets the read element.
        * @return CDpDataElement&: read element.
		*/
        inline CDpDataElement& ReadElement();

        /**
        * This method gets the write element.
        * @return CDpDataElement&: write element.
		*/
        inline CDpDataElement& WriteElement();

        /**
        * This method gets the size of the buffer.
        * @return TInt: size of the buffer.
		*/
        inline TInt Size();

        /**
        * This method gets the maximum reservation size (MMRS) of the buffer.
        * @return TInt: maximum reservation size
		*/
		inline TInt MaxReservationSize();

		/**
		*
		* This method gets the status if the reader is waiting for 
		* more data.
		* @return TBool: is reader waiting for more data
		*/
		inline TBool IsReaderWaiting();

	private: // Methods
        /**
        * C++ default constructor.
		* @param aMaximumReservationSize - maximum reservation size (ebs)
		* @param aBufferInitialSize - initial buffersize (buffer)
        */
        CDpDataBuffer( const TInt aMaximumReservationSize,
                       const TInt aBufferInitialSize );


        /**
        * Symbian 2nd phase constructor.
        */
		void ConstructL();

    protected:  // Data
        /** Read type data buffer element. */
        CDpDataElement      iRE;

        /** Write type data buffer element. */
        CDpDataElement      iWE;

        /** Buffer. */
        HBufC8*             iBuf;

        /** End of buffer. */
        TInt                iEnd;

        /** Size of buffer. */
        TInt                iBufSize;

        /** Threshold of buffer. */
        TInt                iTreshold;

        /** Head of buffer. */
        TInt                iHead;

        /** Tail of buffer. */
        TInt                iTail;

        /** Tail when wrapped around. */
        TInt                iTailWhenWrappedAround;

        /** Reader data client. */
        MDpDataClient*      iReader;

        /** Writer data client. */
        MDpDataClient*      iWriter;

        /** Is reader waiting */
        TBool               iIsReaderWaiting;

        /** Is writer waiting */
        TBool               iIsWriterWaiting;

        /** Maximum reservation size of buffer. */
        const TInt          iMaxReservationSize;

        /** Initial size of buffer. */
        const TInt          iBufferInitialSize;

    protected:  // Friend classes
        friend class        CDpDataElement;
        #ifdef USE_LOGGER
        friend class        CDpDataPort;    // For debugging purposes.
        #endif                              // Used, when USE_LOGGER is defined.



    };

#include "dpdatabuffer.inl"   // inline methods

#endif      // DPDATABUFFER_H

//  End of File
