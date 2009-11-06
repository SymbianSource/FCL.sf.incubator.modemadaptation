/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef DQUEUE_H
#define DQUEUE_H

#include <e32def.h>             // For TUint32
#include <e32cmn.h>             // For TDesC8
#include <klib.h>               // For DBase

#include "iadinternaldefinitions.h" // For EIAD....

class NFastMutex;

// Implemented as ring buffer, syncronization protection by NFastMutex.
NONSHARABLE_CLASS( DQueue ) : public DBase
    {

  public:

    IMPORT_C DQueue( const TInt aSize );

    IMPORT_C ~DQueue();

    /* 
    * In case of queue overflow throw kern::fault, REQ: isaaccessdriver should not lose ISI nor data messages.
    * Adds the message in the end of the ringbuffer queue.
    * @param const TDesC8& aMessage, message to be added.
    */
    IMPORT_C void Add( const TDesC8& aMessage );

    /*
    *Returns the first pointer ( iOutputIndex ) from the ring buffer.
    */
    IMPORT_C TUint16 Count();

    /*
    *Returns the first pointer ( iOutputIndex ) from the ring buffer.
    */
    IMPORT_C TDes8& Get();

    /*
    * Rolls bakc to message given at parameter. Message must be the last message get from the queue.
    */
    IMPORT_C void RollBack( const TDesC8& aMsgToRoll );

  private:

    // Index where to add next message pointer.
    TUint16     iInputIndex;
    // Index where to get next message pointer.
    TUint16     iOutputIndex;
    // Count of items in the array.
    TUint16     iCount;
    // Size of the ring buffer.
    TUint16     iSize;
    NFastMutex* iQueueMutex;
    // The memory allocated for the buffer.
    TDes8**     iRingBuffer;

    };

NONSHARABLE_CLASS( TIADReq )
       {
       public:

       TIADReq();// not allowed and not needed

       IMPORT_C TIADReq( TIADAsyncRequest aReq, TInt aStat );
        // not owned just hold a pointer.
        TIADAsyncRequest     iRequest;
        TInt                 iCompleteStatus;

       };

// Implemented as ring buffer, syncronization protection by NFastMutex.
NONSHARABLE_CLASS( DReqQueue ) : public DBase
    {

  public:

    IMPORT_C DReqQueue();

    IMPORT_C ~DReqQueue();

    /* 
    * In case of queue overflow throw kern::fault, REQ: isaaccessdriver should not lose ISI nor data messages.
    * Adds the message in the end of the ringbuffer queue.
    * @param const TDesC8& aMessage, message to be added.
    */
    IMPORT_C TBool Add( TIADReq aReq );

    /*
    *Returns the first pointer ( iOutputIndex ) from the ring buffer.
    */
    IMPORT_C TBool Empty();

    /*
    *Returns the first pointer ( iOutputIndex ) from the ring buffer.
    */
    IMPORT_C TIADReq Get();

    /*
    * Set req active / deactive. Default deactive.
    */
    IMPORT_C void SetReq( TIADAsyncRequest aReqToSet, TRequestStatus* aStatus = NULL );

    /*
    * Get req. If NULL, request not pending.
    */
    IMPORT_C TRequestStatus* GetReq( TIADAsyncRequest aReqToGet );


  private:

    // Index where to add next message pointer.
    TUint16     iInputIndex;
    // Index where to get next message pointer.
    TUint16     iOutputIndex;
    // Count of items in the array.
    TUint16     iCount;
    // Size of the ring buffer.
    TUint16     iSize;
    NFastMutex* iQueueMutex;
    // Ring buffer must be as big as we have request. TODO: is there a danger to get more request..?
    TIADReq         iReqRingBuffer[ EIADAsyncLast ];
    // Active / Deactive requests are kept in list to make finding easier.
    TRequestStatus* iReqList[ EIADAsyncLast ];

    };


#endif /* DQUEUE_H */
