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



#include "dpdataelement.h" // data element
#include "dpdatabuffer.h"  // base class for rx and tx buffers
#include "dpdataclient.h"  // data client that access buffer
#include "dpdef.h"         // dataport definitions
#include "dplog.h"         // dataport logging
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpdataelementtraces.h"
#endif

// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES
// none

// CONSTANTS
// none

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================

// none


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDpDataElement::CDpDataElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDpDataElement::CDpDataElement(
    CDpDataBuffer& aDataBuffer,
    TDataElementType aType ) :
    iDB( aDataBuffer ),
    iType( aType ),
    iSize( 0 ),
    iState( EDeFree )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAELEMENT_CDPDATAELEMENT, "CDpDataElement::CDpDataElement" );
    LOGM("CDpDataElement::CDpDataElement");
    }

// -----------------------------------------------------------------------------
// CDpDataElement::~CDpDataElement
// Destructor
// -----------------------------------------------------------------------------
//
CDpDataElement::~CDpDataElement()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPDATAELEMENT_CDPDATAELEMENT, "CDpDataElement::~CDpDataElement" );
    LOGM("CDpDataElement::~CDpDataElement");
    }

// -----------------------------------------------------------------------------
// CDpDataElement::Reserve
// This method reserves an data element of requested length
// from the buffer. aTail is not used when reserving write
// element, it is used only in read operations.
// -----------------------------------------------------------------------------
//
TInt CDpDataElement::Reserve(
    const TInt aSize,
    TPtr8& aDes,
    TPtr8& aTail,
    TBool aOverrideMRS )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAELEMENT_RESERVE, "CDpDataElement::Reserve" );
    LOGM("CDpDataElement::Reserve");

    TInt ret( KErrNone );

    // Wrong size
    if ( ( !aOverrideMRS ) && ( aSize > iDB.iMaxReservationSize || 0 >= aSize ) )
        {
        ret = KErrArgument;
        }
    else if ( aOverrideMRS && ( aSize > iDB.Size() || 0 >= aSize ) )
        {
        ret = KErrArgument;
        }
    // Already reserved
    else if ( EDeFree != iState )
        {
        // This can happen e.g. when client makes a null read.
        LOG("  CDpDataElement::Reserve - FAILED");
        OstTrace0( TRACE_NORMAL, DUP1_CDPDATAELEMENT_RESERVE, "CDpDataElement:: Reserve - FAILED" );

        ret = KErrGeneral;
        }
    else
        {
        switch ( iType )
            {
            case EDeRead:
                {
                ret = HandleReadTypeReserve( aSize, aDes, aTail );
                break;
                }
            case EDeWrite:
                {
                ret = HandleWriteTypeReserve( aSize, aDes );
                break;
                }
            default:
                {
                // This should never happen.
                ret = KErrGeneral;
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpDataElement::Release
// This method releases the data element. When a reserved data
// element has been used it has to be released before it can be
// reserved again. If x bytes has been reserved but only y has
// been used, y has to be given in parameter (0 <= y <= x).
// -----------------------------------------------------------------------------
//
TInt CDpDataElement::Release(
    const TInt aUsedLength )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAELEMENT_RELEASE, "CDpDataElement::Release" );
    LOGM("CDpDataElement::Release");

    TInt ret( KErrNone );

    // Already reserved
    if ( EDeReserved != iState )
        {
        LOG("  DP:DataElement::Release - FAIL, not reserved");
        OstTrace0( TRACE_NORMAL, DUP1_CDPDATAELEMENT_RELEASE, "CDpDataElement:: Release - FAIL, not reserved" );

        ret = KErrGeneral;
        }
    // Wrong length
    else if ( aUsedLength > iSize || 0 > aUsedLength )
        {
        LOG("  DP:DataElement::Release - FAIL, wrong length");
        OstTrace0( TRACE_NORMAL, DUP2_CDPDATAELEMENT_RELEASE, "CDpDataElement:: Release - FAIL, wrong length" );

        ret = KErrGeneral;
        }
    else
        {
        switch ( iType )
            {
            case CDpDataElement::EDeRead:
                {
                if ( 0 != iDB.iTailWhenWrappedAround )
                    {
                    LOG("  DP:DataElement::Release - iTailWhenWrappedAround != 0 ");
                    OstTrace0( TRACE_NORMAL, DUP3_CDPDATAELEMENT_RELEASE, "CDpDataElement:: Release - iTailWhenWrappedAround != 0 " );

                    TInt tailsize( iDB.iEnd - iDB.iTailWhenWrappedAround );

                    if ( tailsize <= aUsedLength )
                        {
                        if ( 0 < tailsize )
                            {
                            TPtr8 taildesc(
                                reinterpret_cast<unsigned char*>( &iDB.iBuf->
                                    Des()[iDB.iTailWhenWrappedAround] ),
                                tailsize,
                                tailsize );
                            // Fill released space with FILLCHAR.
                            taildesc.Fill( KDpFillChar );
                            }
                        //no else

                        TPtr8 tmp(
                            reinterpret_cast<unsigned char*>( &iDB.iBuf->
                                Des()[iDB.iTail] ),
                            ( aUsedLength - tailsize ),
                            ( aUsedLength - tailsize ) );
                        // Fill released space with FILLCHAR.
                        tmp.Fill( KDpFillChar );
                        iDB.iTailWhenWrappedAround = 0;
                        iDB.iTail += ( aUsedLength - tailsize );
                        iDB.iEnd = iDB.iHead;
                        }
                    else
                        {
                        // tailsize>aUsedLength
                        TPtr8 taildesc(
                            reinterpret_cast<unsigned char*>( &iDB.iBuf->
                                Des()[iDB.iTailWhenWrappedAround] ),
                            aUsedLength,
                            aUsedLength );
                        // Fill released space with FILLCHAR.
                        taildesc.Fill( KDpFillChar );
                        iDB.iTail += iDB.iTailWhenWrappedAround + aUsedLength;
                        iDB.iTailWhenWrappedAround = 0;
                        }
                    }
                else
                    {
                    TInt index( iDB.iTail );
                    iDB.iTail += aUsedLength;
                    TInt freeSize( aUsedLength );

                    TPtr8 tmp(
                        reinterpret_cast<unsigned char*>( &iDB.iBuf->
                            Des()[index] ),
                        freeSize,
                        freeSize );
                    // Fill released space with FILLCHAR.
                    tmp.Fill( KDpFillChar );
                    }

                iState = EDeFree;
                if ( iDB.iIsWriterWaiting && iDB.iWE.iSize <= iDB.FreeBytes() )
                    {
                    iDB.iIsWriterWaiting = EFalse;
                    // Signal waiting writer.
                    iDB.iWriter->ReleaseNotify();
                    }
                //no else
                break;
                }
            case CDpDataElement::EDeWrite:
                {
                iDB.iHead += aUsedLength;

                if ( iDB.iEnd < iDB.iHead )
                    {
                    iDB.iEnd = iDB.iHead;
                    }
                //no else
                iState = EDeFree;
                if ( iDB.iIsReaderWaiting &&
                     iDB.iReader &&
                     iDB.iRE.iSize <= iDB.UsedBytes() )
                    {
                    iDB.iIsReaderWaiting = EFalse;
                    // Signal waiting reader.
                    iDB.iReader->ReleaseNotify();
                    }
                //no else
                }
                break;
            default:
                {
                break;
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpDataElement::HandleReadTypeReserve
// This method handles reserve operation for read type element.
// -----------------------------------------------------------------------------
//
TInt CDpDataElement::HandleReadTypeReserve(
    const TInt aSize,
    TPtr8 &aDes,
    TPtr8 &aTail )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAELEMENT_HANDLEREADTYPERESERVE, "CDpDataElement::HandleReadTypeReserve" );
    LOGM("CDpDataElement::HandleReadTypeReserve");

    TInt ret( KErrNone );
    iSize = aSize;

    // is there enough data
    if ( aSize > iDB.UsedBytes() )
        {
        // can't have both waiting
        if ( iDB.iIsWriterWaiting && iDB.iIsReaderWaiting)
            {
            LOG("  DP:CDpDataElement::Reserve - Fail:both waiting");
            OstTrace0( TRACE_NORMAL, DUP1_CDPDATAELEMENT_HANDLEREADTYPERESERVE, "CDpDataElement:: - Fail: both waiting" );

            ret = KErrGeneral;
            }
        else
            {
            iDB.iIsReaderWaiting = ETrue;
            // have to wait
            ret = KErrNotReady;
            }
        }
    else
        {
        iState = EDeReserved;
        if ( ( iDB.iTail + aSize ) > iDB.iEnd )
            {
            // set des
            TInt tailSize( iDB.iEnd-iDB.iTail );

            if ( tailSize > 0 )
                {
                aDes.Set(
                    reinterpret_cast<unsigned char*>(
                        &iDB.iBuf->Des()[iDB.iTail] ),
                    tailSize,
                    tailSize );
                }
            //no else

            // set tail
            aTail.Set(
                reinterpret_cast<unsigned char*>( &iDB.iBuf->Des()[0] ),
                ( aSize - tailSize ),
                ( aSize - tailSize ) );
            iDB.iTailWhenWrappedAround = iDB.iTail;
            iDB.iTail = 0;
            }
        else
            {
            // in one piece, set des
            aDes.Set(
                reinterpret_cast<unsigned char*>( &iDB.iBuf->
                    Des()[iDB.iTail] ),
                iSize,
                iSize );
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CDpDataElement::HandleWriteTypeReserve
// This method handles reserve operation for write type element.
// -----------------------------------------------------------------------------
//
TInt CDpDataElement::HandleWriteTypeReserve(
    const TInt aSize,
    TPtr8 &aDes )
    {
    OstTrace0( TRACE_NORMAL, CDPDATAELEMENT_HANDLEWRITETYPERESERVE, "CDpDataElement::HandleWriteTypeReserve" );
    LOGM("CDpDataElement::HandleWriteTypeReserve");

    TInt ret( KErrNone );
    iSize = aSize;

    if ( iDB.iHead > iDB.iTreshold )
        {
        iDB.iHead = 0;
        }
    // no else

    // There is not enough free space
    if ( aSize > iDB.FreeBytes() )
        {
        OstTrace1( TRACE_NORMAL, DUP1_CDPDATAELEMENT_HANDLEWRITETYPERESERVE, "CDpDataElement:: aSize: %d", aSize );
        OstTrace1( TRACE_NORMAL, DUP2_CDPDATAELEMENT_HANDLEWRITETYPERESERVE, "CDpDataElement:: iDB.FreeBytes: %d", iDB.FreeBytes() );

        LOG1("  CDpDataElement::HandleWriteTypeReserve, aSize = %d",
            aSize );
        LOG1("  CDpDataElement::HandleWriteTypeReserve, iDB.FreeBytes = %d",
            iDB.FreeBytes() );

        // Cannot have both waiting.
        if ( iDB.iIsReaderWaiting )
            {
            LOG("  ERROR, CDpDataElement::HandleWriteTypeReserve, Both waiting");
            OstTrace0( TRACE_NORMAL, DUP3_CDPDATAELEMENT_HANDLEWRITETYPERESERVE, "ERROR, CDpDataElement::HandleWriteTypeReserve, Both waiting" );

            ret = KErrGeneral;
            }
        else
            {
            iDB.iIsWriterWaiting = ETrue;
            // Have to wait.
            ret = KErrNotReady;
            }
        }
    else
        {
        iState = EDeReserved;
        // In one piece, set des
        aDes.Set( reinterpret_cast<unsigned char*>(
            &iDB.iBuf->Des()[iDB.iHead] ), 0, iSize );
        }

    return ret;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// none


//  End of File
