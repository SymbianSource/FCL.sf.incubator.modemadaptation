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
#include <iscapi.h>         // isc api
#include <pipeisi.h>        // isi pipe

#include "dpdef.h"          // dataport definitions
#include "dpdataport.h"     // dataport main and c32 interface
#include "dppif.h"          // dcs pipe interface
#include "dpescdetect.h"    // escape sequence detection
#include "dpdataconfig.h"   // configuration store
#include "pep_comm_types.h" // structures for isi-message interface
#include "dpmif.h"          // dcs pipe interface
#include "dplog.h"          // dataport logging

#include <pipe_sharedisi.h>

#ifdef USE_FILE_DEBUG
#include <f32file.h>
extern RFile DpLogFile;
#endif // USE_FILE_DEBUG

#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "dpescdetecttraces.h"
#endif


// LOCAL FUNCTION PROTOTYPES
// none

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CDpEscDetect::CDpEscDetect
// C++ default constructor.
// ---------------------------------------------------------
CDpEscDetect::CDpEscDetect(
    CDpDataPort& aDataPort ) :
    CActive( KDpEscDetectPriority ),
    iDataPort( aDataPort ),
    iDataConfig( iDataPort.DataConfig() )
    {
    OstTrace0( TRACE_NORMAL, CDPESCDETECT_CDPESCDETECT, "CDpEscDetect::CDpEscDetect" );
    LOGM(" CDpEscDetect::CDpEscDetect");

    // Create escape sequence detection timer
    iEscTimer.CreateLocal();

    iEscCharTimeArray[ 0 ] = 0;
    iEscCharTimeArray[ 1 ] = 0;
    iEscCharTimeArray[ 2 ] = 0;

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CDpEscDetect::~CDpEscDetect
// Destructor
// ---------------------------------------------------------
CDpEscDetect::~CDpEscDetect()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CDPESCDETECT_CDPESCDETECT, "CDpEscDetect::~CDpEscDetect" );
    LOGM(" CDpEscDetect::~CDpEscDetect");

    iEscTimer.Close();
    }

// ---------------------------------------------------------
// CDpEscDetect::RunL
// This method is used with escape detection time checking.
// This active object is signalled when timer between last
// escape sequence character (e.g. '+') has expired. If this
// happens we are sure that it was correct escape sequence
// and we can send escapesequence indication to DCS.
// ---------------------------------------------------------
//
void CDpEscDetect::RunL()
    {
    OstTrace0( TRACE_NORMAL, CDPESCDETECT_RUNL, "CDpEscDetect::RunL" );
    LOGM("CDpEscDetect::RunL");

    if ( iTimerRunning )
        {
        SendEscapeIndication();
        iTimerRunning = EFalse;
        }
    // no else
    }

// ---------------------------------------------------------
// CDpEscDetect::RunError
// Leave in RunL() is handled here. Error code is returned,
// when error has occured.
// ---------------------------------------------------------
//
TInt CDpEscDetect::RunError(
    TInt aError )
    {
    OstTrace0( TRACE_NORMAL, CDPESCDETECT_RUNERROR, "CDpEscDetect::RunError" );
    OstTrace1( TRACE_NORMAL, DUP1_CDPESCDETECT_RUNERROR, "CDpEscDetect::Error code: %d", aError );

    LOGM1("CDpEscDetect::RunError, error code: %d", aError );

    return aError;
    }

// ---------------------------------------------------------
// CDpEscDetect::SendEscapeIndication
// This method sends the escape indication to DCS.
// ---------------------------------------------------------
//
void CDpEscDetect::SendEscapeIndication()
    {
    OstTrace0( TRACE_NORMAL, CDPESCDETECT_SENDESCAPEINDICATION, "CDpEscDetect::SendEscapeIndication" );
    LOGM(" CDpEscDetect::SendEscapeIndication");

    TBuf8<SIZE_PNS_PEP_STATUS_IND> messageData;
    // Pipe Handle
    messageData.Append( iDataPort.Pif().PipeHandle() );
    // PEP Type
    messageData.Append( PN_PEP_TYPE_COMM );
    // Indication ID
    messageData.Append( PEP_COMM_IND_ID_ESCAPE );
    // Filler bytes
    messageData.Append( KDpPadding );
    messageData.Append( KDpPadding );

    // Send message
#ifdef _DEBUG
    TInt ret =
#endif
    iDataPort.Mif().SendMessage(
        PN_PIPE,
        iDataPort.CreateTransactionId(),
        PNS_PEP_STATUS_IND,
        messageData );

    LOG("  <== PNS_PEP_STATUS_IND sent (PEP_COMM_IND_ID_ESCAPE)");
    OstTrace0( TRACE_NORMAL, DUP1_CDPESCDETECT_SENDESCAPEINDICATION, "CDpEscDetect:: <== PNS_PEP_STATUS_IND sent (PEP_COMM_IND_ID_ESCAPE)" );

#if defined(_DEBUG)
    if ( KErrNone != ret )
        {
        LOG1("Error isc api send %d", ret );
        OstTrace1( TRACE_NORMAL, DUP2_CDPESCDETECT_SENDESCAPEINDICATION, "CDpEscDetect:: Error isc api send %d", ret );
        }
    // no else
#endif

    iHowManyEscCharsReceived = 0;
    }

// ---------------------------------------------------------
// CDpEscDetect::DoCancel
// This method cancels active request.
// Status : Proposal
// ---------------------------------------------------------
//
void CDpEscDetect::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CDPESCDETECT_DOCANCEL, "CDpEscDetect::DoCancel" );
    LOGM(" CDpEscDetect::DoCancel");

    if ( IsActive() )
        {
        // do the required signaling
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        }
    // no else
    }

// ---------------------------------------------------------
// CDpEscDetect::Scan
// Scans escape sequence.
//
// There must be time (>gt) between last received data and
// first escape character. Escape characters must come with
// time between them, which is smaller than gt. And finally
// time between last escape character and next data must be
// larger than gt.
//
// Example:
// "DATADATADATA" t>gt "+" t<gt "+" t<gt "+" t>gt "DATADATA"
//
// So it is also applicable escape sequence if there comes
// three escape character in same message and time between
// previous and next message is large enough.
// Status : Proposal
// ---------------------------------------------------------
//
TInt CDpEscDetect::Scan(
    TPtr8& aDes )
    {
    OstTrace0( TRACE_NORMAL, CDPESCDETECT_SCAN, "CDpEscDetect::Scan" );
    LOGM(" CDpEscDetect::Scan");

    TInt len( aDes.Length() );
    TInt ret( KErrNone );

    // New data, cancel timer
    if ( iTimerRunning )
        {
        iEscTimer.Cancel();
        iTimerRunning = EFalse;
        iHowManyEscCharsReceived = 0;
        iLatestIndex = 0;
        iTimeToPrevReceivedMessage.HomeTime();
        ret = KErrNotFound;
        }
    else if ( ( 3 < len ) || ( 0 == len  ) )
        {
        // if the length of the string is greater than 3 then it can't hold
        // escape sequence because the time before the first (or time after the
        // last)  escape char is not greater than gt. Actually the time can't be
        // calculated so it is assumed to be 0.
        iHowManyEscCharsReceived = 0;
        iLatestIndex = 0;
        iTimeToPrevReceivedMessage.HomeTime();
        ret = KErrNotFound;
        }
    else
        {
        TInt ref( 0 );
        TTime timeNow( 0 );

        timeNow.HomeTime();

        TUint8 escChar( iDataConfig.EscChar() );
        TUint8 escTime( iDataConfig.EscTime() );

        // Interval time in microseconds. EscTime is defined in 20 milliseconds so
        // it has to be calculated

        TTimeIntervalMicroSeconds intervalEscTime(
            escTime * KDpEscapeTimeMultiplier * KDpEscapeSecondAsMilliseconds );

        TTimeIntervalMicroSeconds32 intervalEscTime32(
            escTime * KDpEscapeTimeMultiplier * KDpEscapeSecondAsMilliseconds );

        // length is 3 or less
        for ( TInt i( 0 ); i < len; i++ )
            {
            // non escape char was found => can't be escape sequence or even part of it.
            if ( aDes[i] != escChar )
                {
                iHowManyEscCharsReceived = 0;
                iLatestIndex = 0;
                iTimeToPrevReceivedMessage.HomeTime();
                ret = KErrNotFound;
                break;
                }
            // no else
            }

        if ( KErrNone == ret )
            {
            // the whole string is full of escape chars. There can be one escape sequence
            // (3 escape chars) or
            // just part of the sequence (2 or 1 escape chars)
            for ( TInt j( 0 ); j < len; j++ )
                {
                //if j>0 more than one "+" received in same message
                if ( timeNow < TTime( iTimeToPrevReceivedMessage + intervalEscTime )
                    || 0 < j )
                    {
                    // smaller than gt
                    iEscCharTimeArray[(iLatestIndex + 1) % 3] = ( - 1);
                    iLatestIndex = ( iLatestIndex + 1 ) % 3;
                    }
                else
                    {
                    // greater than gt
                    iEscCharTimeArray[(iLatestIndex + 1) % 3] = 1;
                    iLatestIndex = ( iLatestIndex + 1 ) % 3;
                    }
                iHowManyEscCharsReceived++;
                }

            if ( KDpEscapeSeqLength == iHowManyEscCharsReceived )
                {
                if ( CheckEscCharIntervals( iLatestIndex ) )
                    {
                    if ( !IsActive() )
                        {
                        iEscTimer.After( iStatus, intervalEscTime32 );
                        iTimerRunning = ETrue;

                        SetActive();
                        }
                    // no else

                    ret = KErrCompletion;
                    }
                else
                    {
                    iHowManyEscCharsReceived = 0;
                    iLatestIndex = 0;
                    iTimeToPrevReceivedMessage.HomeTime();
                    ret = KErrNotFound;
                    }
                }
            else
                {
                if ( ( 1 == iHowManyEscCharsReceived ) ||
                    ( 2 == iHowManyEscCharsReceived ) )
                    {
                    if ( 0 == iLatestIndex )
                        {
                        ref = ( -1 );
                        }
                    else if ( 1 == iLatestIndex )
                        {
                        ref= 1;
                        }
                    else if ( 2 == iLatestIndex )
                        {
                        ref = ( -1 );
                        }
                    //no else

                    if ( ( 3 > iLatestIndex ) &&
                        ( ( 1 == aDes.Length() ) || ( 2 == aDes.Length() ) ) )
                        {
                        if ( iEscCharTimeArray[ iLatestIndex ] != ref )
                            {
                            iHowManyEscCharsReceived = 0;
                            iLatestIndex = 0;
                            }
                        // no else
                        }
                    // no else

                    iTimeToPrevReceivedMessage.HomeTime();
                    }
                else
                    {
                    iTimeToPrevReceivedMessage.HomeTime();
                    }
                ret = KErrNotFound;
                }
            }
        //no else
        }

    return ret;
    }

// ---------------------------------------------------------
// CDpEscDetect::CheckEscCharIntervals
// This method checks latest timing of escape characters.
// ---------------------------------------------------------
//
TBool CDpEscDetect::CheckEscCharIntervals(
    const TInt aLatestIndex )
    {
    OstTrace0( TRACE_NORMAL, CDPESCDETECT_CHECKESCCHARINTERVALS, "CDpEscDetect::CheckEscCharIntervals" );
    LOGM(" CDpEscDetect::CheckEscCharIntervals");

    TBool ret( EFalse );

    if ( 3 > aLatestIndex )
        {
        if ( 0 > iEscCharTimeArray[ aLatestIndex ] &&
            0 < iEscCharTimeArray[ ( aLatestIndex + 1 ) % 3 ] &&
            0 > iEscCharTimeArray[ ( aLatestIndex + 2 ) % 3 ] )
            {
            ret = ETrue;
            }
        //no else
        }
    //no else

    return ret;
    }

//  OTHER EXPORTED FUNCTIONS
//  none

//  End of File
