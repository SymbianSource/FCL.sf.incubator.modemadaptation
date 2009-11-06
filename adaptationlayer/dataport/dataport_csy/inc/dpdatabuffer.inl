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



// ---------------------------------------------------------
// CDpDataBuffer::ReadElement
// This method gets the read element.
// ---------------------------------------------------------
//
inline CDpDataElement& CDpDataBuffer::ReadElement()
    {
    return iRE;
    }

// ---------------------------------------------------------
// CDpDataBuffer::WriteElement
// This method gets the write element.
// ---------------------------------------------------------
//
inline CDpDataElement& CDpDataBuffer::WriteElement()
    {
    return iWE;
    }

// ---------------------------------------------------------
// CDpDataBuffer::Size
// This method gets the size of the buffer.
// ---------------------------------------------------------
//
inline TInt CDpDataBuffer::Size()
    {
    return( iBufSize-iMaxReservationSize );
    }

// ---------------------------------------------------------
// CDpDataBuffer::MaxReservationSize
// This method gets the maximum reservation size (MMRS) of
// the buffer.
// ---------------------------------------------------------
//
inline TInt CDpDataBuffer::MaxReservationSize()
    {
    return iMaxReservationSize;
    }

// ---------------------------------------------------------
// CDpDataBuffer::IsReaderWaiting
// This method gets the status if the reader is waiting for 
// more data.
// ---------------------------------------------------------
//
inline TBool CDpDataBuffer::IsReaderWaiting()
	{
	return iIsReaderWaiting;
	}

//  End of File
