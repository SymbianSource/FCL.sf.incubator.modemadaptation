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




// INCLUDE FILES
#include    "dpdef.h"                   // dataport definitions
#include    "dpdatabuffer.h"            // base class for rx and tx buffers
#include    "dpdataelement.h"           // data elements
#include    "dpdataclient.h"            // data client that access buffer
#include    "dplog.h"                   // dataport logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpdatabufferTraces.h"
#endif

// LOCAL FUNCTION PROTOTYPES
// none

// ==================== LOCAL FUNCTIONS ====================
// none

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDpDataBuffer::CDpDataBuffer
// C++ default constructor.
// ---------------------------------------------------------
CDpDataBuffer::CDpDataBuffer(
    const TInt aMaximumReservationSize,
    const TInt aBufferInitialSize):
    iRE(*this,CDpDataElement::EDeRead),
    iWE(*this,CDpDataElement::EDeWrite),
    iMaxReservationSize( aMaximumReservationSize ),
    iBufferInitialSize( aBufferInitialSize )
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_CDPDATABUFFER, "CDpDataBuffer::CDpDataBuffer" );
    LOGM(" CDpDataBuffer::CDpDataBuffer");
    }

// ---------------------------------------------------------
// CDpDataBuffer::ConstructL
// Symbian 2nd phase constructor.
// ---------------------------------------------------------
void CDpDataBuffer::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_CONSTRUCTL, "CDpDataBuffer::ConstructL" );
    LOGM(" CDpDataBuffer::ConstructL");

    iBufSize = iBufferInitialSize + iMaxReservationSize;

    // obey configuration pbs+ebs in design document
    iBuf=HBufC8::NewL( iBufSize );

    // length to the max
    iBuf->Des().SetLength( iBufSize );
    iBuf->Des().Fill( KDpFillChar );

    iTreshold = iBufferInitialSize;
    }

// ---------------------------------------------------------
// CDpDataBuffer::NewL
// Static constructor.
// ---------------------------------------------------------
CDpDataBuffer* CDpDataBuffer::NewL(
    const TInt aMaximumReservationSize,
    const TInt aBufferInitialSize)
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_NEWL, "CDpDataBuffer::NewL" );
    LOGM(" CDpDataBuffer::NewL");

    CDpDataBuffer* self = new (ELeave) CDpDataBuffer(
         aMaximumReservationSize, aBufferInitialSize );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CDpDataBuffer::~CDpDataBuffer
// Destructor
// ---------------------------------------------------------
CDpDataBuffer::~CDpDataBuffer()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPDATABUFFER_CDPDATABUFFER, "CDpDataBuffer::~CDpDataBuffer" );
    LOGM(" CDpDataBuffer::~CDpDataBuffer");

    if ( iBuf )
        {
        delete iBuf;
        }

    iReader = NULL;
    iWriter = NULL;
    }

// ---------------------------------------------------------
// CDpDataBuffer::SetLength
// This method sets length of buffer. This is called from
// CDpDataPort::SetReceiveBufferLength() method.
// ---------------------------------------------------------
//
TInt CDpDataBuffer::SetLength(
    const TInt aLen)
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_SETLENGTH, "CDpDataBuffer::SetLength" );
    LOGM(" CDpDataBuffer::SetLength");

    //Initialization
    TInt max( 0 );
    TInt ret( KErrNone );

    if ( iHead + iWE.iSize > iEnd )
        {
        max = iHead + iWE.iSize;
        }
    else
        {
        max = iEnd;
        }

        OstTraceExt2( TRACE_NORMAL, DUP1_CDPDATABUFFER_SETLENGTH, "CDpDataBuffer:: iHead: %d, iWE.iSize: %d", iHead, iWE.iSize );
        OstTraceExt2( TRACE_NORMAL, DUP2_CDPDATABUFFER_SETLENGTH, "CDpDataBuffer:: iEnd : %d, aLen     : %d", iEnd, aLen );

        LOG2("  iHead: %d, \t iWE.iSize: %d", iHead, iWE.iSize );
        LOG2("  iEnd : %d, \t aLen     : %d", iEnd, aLen );

    if ( ( max >= ( aLen + iMaxReservationSize ) ) ||
            ( aLen < 2*iMaxReservationSize ) )
        {
        // buffer may be filled further than length

        OstTrace0( TRACE_NORMAL, DUP3_CDPDATABUFFER_SETLENGTH, "CDpDataBuffer:: ERROR, Client tried to reserve too big or too small Receive Buffer Size" );
        OstTraceExt2( TRACE_NORMAL, DUP4_CDPDATABUFFER_SETLENGTH, "CDpDataBuffer:: aLen: %d, iMaxReservationSize: %d", aLen, iMaxReservationSize );
        OstTrace1( TRACE_NORMAL, DUP5_CDPDATABUFFER_SETLENGTH, "CDpDataBuffer:: max: %d", max );

        LOG("  ERROR, Client tried to reserve too big or too small Receive Buffer Size");
        LOG2("  aLen: %d, \t iMaxReservationSize: %d", aLen, iMaxReservationSize );
        LOG1("  max: %d", max );

        ret = KErrArgument;
        }
    else
        {
        iBuf->Des().SetLength( max );
        HBufC8* tmp = NULL;

        // ReAllocL returns address of the expanded or contracted HBufC descriptor.
        // ReAllocL() leaves, if there is insufficient memory.
        TRAP(ret, ( tmp = iBuf->ReAllocL( aLen+iMaxReservationSize ) ));

        if ( KErrNone == ret )
            {
            iBuf=tmp;
            iBuf->Des().SetLength( aLen + iMaxReservationSize );
            iBufSize = aLen + iMaxReservationSize;
            iTreshold = aLen;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataBuffer::RegisterDataClient
// This method registers data client to the buffer.
// ---------------------------------------------------------
//
TInt CDpDataBuffer::RegisterDataClient(
    MDpDataClient& aClient )
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_REGISTERDATACLIENT, "CDpDataBuffer::RegisterDataClient" );
    LOGM(" CDpDataBuffer::RegisterDataClient");
    TInt ret( KErrNone );

    switch ( aClient.Role() )
        {
        case EDbReader:
            {
            if ( iReader )
                {
                LOG("  CDpDataBuffer::RegisterDataClient, iReader already exists");
                OstTrace0( TRACE_NORMAL, DUP1_CDPDATABUFFER_REGISTERDATACLIENT, "CDpDataBuffer:: iReader already exists" );

                ret = KErrGeneral;
                }
            else
                {
                iReader = &aClient;
                }
            break;
            }
        case EDbWriter:
            {
            if ( iWriter )
                {
                LOG("  CDpDataBuffer::RegisterDataClient, iWriter already exists");
                OstTrace0( TRACE_NORMAL, DUP2_CDPDATABUFFER_REGISTERDATACLIENT, "CDpDataBuffer:: iWriter already exists" );

                ret = KErrGeneral;
                }
            else
                {
                iWriter = &aClient;
                }
            break;
            }
        default:
            {
            LOG("  CDpDataBuffer::RegisterDataClient, default, return KErrGeneral");
            OstTrace0( TRACE_NORMAL, DUP3_CDPDATABUFFER_REGISTERDATACLIENT, "CDpDataBuffer:: default, return KErrGeneral" );

            ret = KErrGeneral;
            break;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataBuffer::UsedBytes
// This method gets the amount of used bytes.
// Note that when virtual circular buffer has wrapped around
// there might be iTailWhenWrappedAround > 0 and therefore we
// have to notice that also in calculation. Normally
// iTailWhenWrappedAround == 0.
//
// In other words: When we have data in ebs (enhanced buffer
// space) we have to calculate it too. When
// iTailWhenWrappedAround>0 iTail==0.
//
// t     h               twr
// |     |              n |
// |-------pbs--------|--ebs--|
//
// ---------------------------------------------------------
//
TInt CDpDataBuffer::UsedBytes()
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_USEDBYTES, "CDpDataBuffer::UsedBytes" );
    LOGM(" CDpDataBuffer::UsedBytes");
    TInt ret( 0 );

    if ( iHead != iTail  )
        {
        if ( iHead > ( iTail + iTailWhenWrappedAround )  )
            {
            ret = iHead - iTail;
            }
        else
            {
            ret = iEnd - iTail - iTailWhenWrappedAround + iHead;
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpDataBuffer::FreeBytes
// This method gets the amount of free bytes.
// ---------------------------------------------------------
//
TInt CDpDataBuffer::FreeBytes()
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_FREEBYTES, "CDpDataBuffer::FreeBytes" );
    LOGM(" CDpDataBuffer::FreeBytes");

    return ( iBufSize - UsedBytes() - iMaxReservationSize );
    }

// ---------------------------------------------------------
// CDpDataBuffer::Flush
// This method empties the buffer and notifies waiting reader
// and writer about the flush.
// ---------------------------------------------------------
//
void CDpDataBuffer::Flush()
    {
    OstTrace0( TRACE_NORMAL, CDPDATABUFFER_FLUSH, "CDpDataBuffer::Flush" );
    LOGM(" CDpDataBuffer::Flush");

    // throw reader out
    //signal waiting reader
    if ( iIsReaderWaiting && iReader )
        {
        iReader->FlushNotify();
        iIsReaderWaiting = EFalse;
        }

    // throw writer out
    //signal waiting writer
    if ( iIsWriterWaiting && iWriter )
        {
        iWriter->FlushNotify();
        iIsWriterWaiting = EFalse;
        }

    // As (currently) data accesses to buffer are synchronous, there can be no
    // data client accessing the buffer.
    iEnd = 0;
    iHead = 0;
    iTail = 0;
    iTailWhenWrappedAround = 0;

    iBuf->Des().Fill( KDpFillChar );
    }

//=============================================================================

//  OTHER EXPORTED FUNCTIONS
//  none

//  End of File
