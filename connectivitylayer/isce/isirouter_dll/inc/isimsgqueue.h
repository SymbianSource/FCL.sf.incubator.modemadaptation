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



#ifndef __ISIMSGQUEUE_H_
#define __ISIMSGQUEUE_H_

#include <e32def.h>             // For TUint32
#include <e32cmn.h>             // For TDesC8
#include <klib.h>               // For DBase

class NFastMutex;

/*
* Ring buffer for messages.
*/
NONSHARABLE_CLASS( DISIMsgQueue ) : public DBase
    {

    public:

        /*
        * Constructor
        * @param aSize, maximum size of the queue.
        */
        DISIMsgQueue( const TUint16 aSize );

        /*
        * Destructor
        */
        ~DISIMsgQueue();

        /* 
        * Adds the message in the end of the ringbuffer queue.
        * If queue overflows throws fault.
        * @param aMessage, message to be added
        */
        void Add( const TDesC8& aMessage );

        /* 
        * Returns the count of the messages in the queue
        * @return count of the messages in the queue.
        */
        TUint16 Count();

        /*
        * Returns a reference to first message ( iOutputIndex ) in the ring buffer.
        * If queue is messed up throws fault.
        * NOTE! Responsibility to deallocate the memory is transferred to caller.
        * @return reference to message
        */
        TDes8& Get();

        /*
        * Rolls back the ringbuffer.
        * Message must be the last message get from the queue.
        * NOTE! Responsibility to deallocate the memory is transferred to queue.
        * @param aMsgToRoll
        */
        void RollBack( const TDesC8& aMsgToRoll );

    private:

        // Added convention:
        // Prefix "iSh" means shared member variable
        // Prefix "gSh" means shared global variable

        // Index where to add next message pointer.
        TUint16     iShInputIndex;
        // Index where to get next message pointer.
        TUint16     iShOutputIndex;
        // Count of items in the array.
        TUint16     iShCount;
        // Size of the ring buffer. Written only once in constructor.
        TUint16     iSize;
        // Owned
        NFastMutex* iQueueMutex;
        // Owned, memory allocated for the buffer.
        TDes8**     iShRingBuffer;

    };

#endif /* __ISIMSGQUEUE_H_ */
