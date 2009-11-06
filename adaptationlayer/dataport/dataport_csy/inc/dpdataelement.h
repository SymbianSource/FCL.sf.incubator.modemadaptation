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



#ifndef DPDATAELEMENT_H
#define DPDATAELEMENT_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
// none

// MACROS
// none

// DATA TYPES
// none

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
class CDpDataBuffer;

// CLASS DECLARATION

/**
*  CDpDataElement provides buffer elements for data handling inside DataPort.
*  There are two types of class objects: 'read' and 'write' types. 'read' type
*  object is used to read from RX/TX buffer. 'write' type object is used to
*  write to  RX/TX buffer. CDpDataElement provides methods for buffer element
*  reservation and and releasing.
*
*  CDpDataElement data members are protected, and CDpDataBuffer is friend class
*  of CDpDataElement. This allows efficient use of data members from other
*  classes.
*/
class CDpDataElement : public CBase
    {

    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        ~CDpDataElement();

    public:  // New methods

        /**
        * NOTE: Reserve element before using it and use return value correctly.
        * This method reserves an data element of requested length from
        * the buffer.
        * @param aSize Reservation size
        * @param aDes Reserved buffer area
        * @param aTail Second part of reserved buffer area, when tail is wrapped around
        * @param aOverrideMRS Allow bigger data element reservation from buffer
        * ( > Maximum reservation size )
        * @return Error code
        */
        TInt Reserve(
            const TInt aSize, TPtr8& aDes,
            TPtr8& aTail, TBool aOverrideMRS = EFalse );

        /**
        * NOTE: Release element after using it and give used length.
        * This method releases the data element after using it.
        * If state is not 'reserved' or 'length used' is not between zero and
        * size, leave with KErrGeneral.
        * In 'read' type element:
        * 1. If reserve operation generated a wrap around, calculate length of
        *    the tailing bytes i.e. the used bytes from tail, when wrapped
        *    around to the end.
        * 2. In case this length is less or equal to 'length used', fill
        *    released bytes at the end with a 'fill character'. Then fill
        *    released bytes in the beginning of the buffer, reset tail when
        *    wrapped around and set tail to a correct value. Update end value to
        *    head value.
        * 3. Else (length is more than 'length used'), fill only 'length used'
        *    from tail when wrapped around. Set tail to correct value and reset
        *    tail when wrapped around.
        * 4. Else (no wrap around), fill with a 'fill character' from the tail
        *    on the 'length used'. Set tail to tail + 'length used'.
        * 5. Set state to 'free'.
        * 6. If there is a writer waiting and writer size is equal or less than
        *    free bytes i.e. there is enough space in the buffer, signal the
        *    writer and reset writer waiting.
        * 7. Return KErrNone.
        * In 'write' type element:
        * 1. Increase head by 'length used'
        * 2. If end is less than head, set end to head.
        * 3. Set state to 'free'.
        * 4. If there is a reader waiting and reader size is equal or less than
        *    used bytes i.e. there is enough data in the buffer, signal the
        *    reader and reset reader waiting.
        * 5. Return KErrNone.
        * @param aUsedLength Used length from the reserved size
        * @return Error code
        */
        TInt Release( const TInt aUsedLength );

    public:  // Methods from base classes

        // none

    protected:  // Enumerations

        enum TDataElementType
            {
            EDeRead,
            EDeWrite
            }; // write/read type element

        enum TDataElementState
            {
            EDeFree,
            EDeReserved
            };

    protected:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aDataBuffer databuffer
        * @param aType type EDeRead/EDeWrite
        */
        CDpDataElement( CDpDataBuffer& aDataBuffer, TDataElementType aType );

    protected:  // New functions

        // none

    protected:  // Methods from base classes

        // none

    private:  // Methods

        /**
        * This method handles read type reserve:
        * 1. Store parameter size.
        * 2. If there is not enough data (used bytes < size), set reader waiting
        *    and return KErrNotReady. If writer is already waiting, return
        *    KErrGeneral.
        * 3. Set state to 'reserved' and set aDes.
        * 4. If wanted length wraps around, i.e. tail+size exceeds end, set
        *    also aTail.
        * 5. Return KErrNone.
        * @param aSize Reservation size
        * @param aDes Reserved buffer area
        * @param aTail Second part of reserved buffer area, when tail is wrapped around
        * @return Error code
        */
        TInt HandleReadTypeReserve(
            const TInt aSize, TPtr8& aDes, TPtr8& aTail );

        /**
        * This method handles write type reserve:
        * 1. Store parameter size.
        * 2. If head is over threshold, set head to zero.
        * 3. If there is not enough free space (free bytes < size), set writer
        *    waiting and return KErrNotReady. If reader is already waiting,
        *    return KErrGeneral.
        * 4. Set state to 'reserved' and set aDes.
        * 5. Return KErrNone.
        * @param aSize Reservation size
        * @param aDes Reserved buffer area
        * @return Error code
        */
        TInt HandleWriteTypeReserve( const TInt aSize, TPtr8& aDes );

    public:  // Data

        // none

    protected:  // Data

        // Data buffer.
        CDpDataBuffer& iDB;

        // Data element type (EDeRead/EDeWrite).
        TDataElementType iType;

        // Reserved size.
        TInt iSize;

        // Data element state (EDeFree/EDeReserved).
        TDataElementState iState;

    private:    // Data

        // none

    public:     // Friend classes

        // none

    protected:  // Friend classes

        friend class CDpDataBuffer;

    private:    // Friend classes

        // none

    };

#endif // DPDATAELEMENT_H

// End of File
