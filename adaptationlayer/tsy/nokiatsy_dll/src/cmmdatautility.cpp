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



// Include Files
#include "cmmdatautility.h"
#include "cmmdataporthandler.h"
#include "tsylogger.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdatautilityTraces.h"
#endif
// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
    //None

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
    //None

// ==================== LOCAL FUNCTIONS =======================================

    //None

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CDataPortWriter::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CDataPortWriter* CDataPortWriter::NewL(
    CMmDataPortHandler* aMmDataPortHandler,
    const RComm& aDataPort )
    {

    TFLOGSTRING("TSY: CDataPortWriter::NewL");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_NEWL, "CDataPortWriter::NewL" );

    CDataPortWriter* writer = new ( ELeave ) CDataPortWriter();
    CleanupStack::PushL( writer );

    writer->iDataPortHandler = aMmDataPortHandler;
    writer->iDataPort = aDataPort;
    writer->ConstructL();

    CActiveScheduler::Add( writer );

    CleanupStack::Pop( writer );

    return writer;
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::CDataPortWriter
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CDataPortWriter::CDataPortWriter()
    : CActive( EPriorityStandard ), iDataToDpPtr( NULL, 0 )
    {
    //None
    TFLOGSTRING("TSY: CDataPortWriter::CDataPortWriter");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_CDATAPORTWRITER, "CDataPortWriter::CDataPortWriter" );

    }

// ----------------------------------------------------------------------------
// CDataPortWriter::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CDataPortWriter::ConstructL()
    {

    TFLOGSTRING("TSY: CDataPortWriter::ConstructL");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_CONSTRUCTL, "CDataPortWriter::ConstructL" );

    // initialisation of menber variables
    iWriteCompleted = ETrue;
    iContinueWrite = ETrue;
    iWriteBuffer = new ( ELeave ) CArrayPtrSeg<HBufC8>(
        KSentAtCommandLimit );
    iAtCommandBuffer = new (ELeave) CArrayFixFlat<TUint>(
        KSentAtCommandLimit );
    }

// -----------------------------------------------------------------------------
// CDataPortWriter::~CDataPortWriter
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CDataPortWriter::~CDataPortWriter()
    {

    TFLOGSTRING("TSY: CDataPortWriter::~CDataPortWriter");
OstTrace0( TRACE_NORMAL, DUP1_CDATAPORTWRITER_CDATAPORTWRITER, "CDataPortWriter::~CDataPortWriter" );

    Cancel();

    if ( iWriteBuffer )
        {
        iWriteBuffer->ResetAndDestroy();
        delete iWriteBuffer;
        }
    //no else

    if ( iAtCommandBuffer )
        {
        iAtCommandBuffer->Reset();
        delete iAtCommandBuffer;
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::Init
// This method sends TSY init strings to modem.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortWriter::Init()
    {

    TFLOGSTRING("TSY: CDataPortWriter::Init Modem initialisation");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_INIT, "CDataPortWriter::Init" );

    // Initalise modem
    TBuf8<KDataBufLength> ATString;

    // Common Initialisations
    ATString.Copy( KModemInitString );
    ATString.Append( KReturn );
    Write( ATString, EModemInit );
    // VT initialisation
    ATString.Copy( KHscsdMultimedia_64000 );
    Write( ATString, EModemInit );
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::AppendCommandL
// This method appends enum for AT command to be sent into iAtCommandBuffer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortWriter::AppendCommandL(
    TLastAtCommandSent aCommand )
    {

    TFLOGSTRING("TSY: CDataPortWriter::AppendCommandL");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_APPENDCOMMANDL, "CDataPortWriter::AppendCommandL" );

    iAtCommandBuffer->AppendL( aCommand );
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::GetAtBuffer
// Returns iAtCommandBuffer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CArrayFixFlat<TUint>* CDataPortWriter::GetAtBuffer()
    {

    TFLOGSTRING("TSY: CDataPortWriter::GetAtBuffer");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_GETATBUFFER, "CDataPortWriter::GetAtBuffer" );

    return iAtCommandBuffer;
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::Write
// Writes data into dataport.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortWriter::Write(
    const TDesC8& aData,
    TLastAtCommandSent aCommand )
    {

    TFLOGSTRING("TSY: CDataPortWriter::Write");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_WRITE, "CDataPortWriter::Write" );

    // We are writing data so set the flag to ETrue.
    iContinueWrite = ETrue;

#ifdef TF_LOGGING_ENABLED
    // If aData is longer than KDataBufLength, copy only KDataBufLength
    TBuf< KDataBufLength > data16bit;
    if ( KDataBufLength < aData.Length() )
        {
        data16bit.Copy( aData.Left( KDataBufLength ) );
        }
    else
        {
        data16bit.Copy( aData.Left( aData.Length() ) );
        }

    TFLOGSTRING2("TSY: CDataPortWriter::Write - Stored into FIFO buffer: %S", &data16bit );

OstTraceExt1( TRACE_NORMAL, DUP1_CDATAPORTWRITER_WRITE, "CDataPortWriter::Write;Stored into FIFO buffer=%S", data16bit );
#endif // TF_LOGGING_ENABLED

    HBufC8* tmp = NULL;
    TInt commandError( KErrNone );

    // Trap possible leave in AppendLs and AllocL.
    TRAPD( trapError,
        // Create tmp ptr for data, allocate heap and copy the aData parameter.
        tmp = aData.AllocL();
        // Append the ptr into buffer
        iWriteBuffer->AppendL( tmp );
        // Append AT command enum into buffer so that we know
        // what is the AT command.
        AppendCommandL( aCommand ) );

    // Check if allocation and appends were succesful.
    if ( ( KErrNone == trapError ) && ( KErrNone == commandError ) )
        {
        // If already sent data and pending for completition, continue to wait.
        // If the last write thas been already completed, write new data.
        if ( iWriteCompleted )
            {
            // Write the data.
            iStatus = KRequestPending;
            iWriteCompleted = EFalse;

            // As iWriteBuffer->AppendL() can reorganise the iWriteBuffer,
            // we need to use a separate iDataToDPPtr with Write.
            // Otherwise it is possible that the data written before
            // new CDataPortWriter::Write is called has wrong address.
            iDataToDpPtr.Set( iWriteBuffer->At( 0 )->Left(
                iWriteBuffer->At( 0 )->Length() ) );
            // Write the data to modem
            iDataPort.Write( iStatus, iDataToDpPtr );
            // DO NOT DELETE ALLOCATED DATA FROM HEAP AS IT IS DONE IN RUNL

#ifdef TF_LOGGING_ENABLED
            // If aData is longer than KDataBufLength, copy only KDataBufLength
            if ( KDataBufLength < aData.Length() )
                {
                data16bit.Copy( iDataToDpPtr.Left( iDataToDpPtr.Length() ) );
                }
            else
                {
                data16bit.Copy( aData.Left( aData.Length() ) );
                }

            TFLOGSTRING2("TSY: CDataPortWriter::Write - From FIFO buffer: %S", &data16bit);

OstTraceExt1( TRACE_NORMAL, DUP2_CDATAPORTWRITER_WRITE, "CDataPortWriter::Write;From FIFO buffer=%S", data16bit );
#endif // TF_LOGGING_ENABLED

            if ( !IsActive() )
                {
                SetActive();
                }
            //no else
            }
        //no else
        }

    // If TRAP caught error, call line to handle the error.
    else
        {
        // If AllocL or AppendL left for the actual data string,
        // delete tmp if it exists.
        if ( NULL != tmp )
            {
            delete tmp;
            }

        // If AllocL or AppendL left, handle corresponding error.
        iDataPortHandler->HandleDataPortError( trapError );
        }
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::RunL
// Activates active object when request is completed.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortWriter::RunL()
    {

    TFLOGSTRING("TSY: CDataPortWriter::RunL");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_RUNL, "CDataPortWriter::RunL" );

#ifdef TF_LOGGING_ENABLED

    TBuf< KDataBufLength > data16bit;
    data16bit.AppendNum( iStatus.Int() );

    TFLOGSTRING2("TSY: CDataPortWriter::RunL - iStatus: %S", &data16bit);
OstTrace1( TRACE_NORMAL, DUP2_CDATAPORTWRITER_RUNL, "CDataPortWriter::RunL;iStatus: %d", iStatus.Int() );
#endif // TF_LOGGING_ENABLED

    if ( KRequestPending != iStatus.Int() )
        {
        // If write buffer has data, delete first element.
        if ( 0 < iWriteBuffer->Count() )
            {

#ifdef TF_LOGGING_ENABLED
            // If write buffer entry length is longer than KDataBufLength,
            // log only KDataBufLength bytes of data
            if ( KDataBufLength < iWriteBuffer->At( 0 )->Length() )
                {
                data16bit.Copy( iWriteBuffer->At( 0 )->
                    Left( KDataBufLength ) );
                }
            // Else log the whole buffer
            else
                {
                data16bit.Copy( iWriteBuffer->At( 0 )->Left(
                    iWriteBuffer->At( 0 )->Length() ) );
                }

            TFLOGSTRING2("TSY: CDataPortWriter::RunL - AT command deleted from FIFO buffer: %S", &data16bit);
OstTraceExt1( TRACE_NORMAL, DUP1_CDATAPORTWRITER_RUNL, "CDataPortWriter::RunL;AT command deleted from FIFO buffer=%S", data16bit );
#endif // TF_LOGGING_ENABLED

            if ( iWriteBuffer->At( 0 ) )
                {
                // Delete the data from heap
                delete iWriteBuffer->At( 0 );
                iWriteBuffer->At( 0 ) = NULL;
                }
            //no else

            // Delete and compress the buffer
            iWriteBuffer->Delete( 0 );
            iWriteBuffer->Compress();
            // Leave if error, will fall back to RunError.
            User::LeaveIfError( iStatus.Int() );
            }
        //no else
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::RunError
// Called when RunL leaves.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CDataPortWriter::RunError(
    TInt aError )
    {

    TFLOGSTRING2("TSY: CDataPortWriter::RunError %d", aError);
OstTrace1( TRACE_NORMAL, CDATAPORTWRITER_RUNERROR, "CDataPortWriter::RunError;aError=%d", aError );

    // Line object completes client requests in case of an error.
    iDataPortHandler->HandleDataPortError( aError );
    // Cancel writes if active.
    Cancel();
    // If write buffer has data, delete it.
    TInt count( iWriteBuffer->Count() );

    if ( 0 < count )
        {
        // Loop through each element and delete data from heap.
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iWriteBuffer->At( 0 ) )
                {
                delete iWriteBuffer->At( 0 );
                iWriteBuffer->At( 0 ) = NULL;
                }
            //no else
            }
        }
    //no else

    // Delete elements and compress buffer.
    iWriteBuffer->Delete( 0, iWriteBuffer->Count() );
    iWriteBuffer->Compress();

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::WriteNextL
// Writes next AT command from iWriteBuffer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortWriter::WriteNextL()
    {

    TFLOGSTRING("TSY: CDataPortWriter::WriteNextL - Write next command from FIFO");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_WRITENEXTL, "CDataPortWriter::WriteNextL, Write next command from FIFO" );

    if ( !IsActive() )
        {
        iWriteCompleted = ETrue;
        User::LeaveIfError( iStatus.Int() );

        // Check if we have AT commands waiting in the buffer and if we are
        // still supposed to write commands
        if ( ( 0 < iWriteBuffer->Count() ) &&
             ( EFalse != iContinueWrite ) )
            {
            iWriteCompleted = EFalse;
            // As iWriteBuffer->AppendL() can reorganise the iWriteBuffer,
            // we need to use a separate iDataToDPPtr with Write.
            // Otherwise it is possible that the data written before
            // new CDataPortWriter::Write is called has wrong address
            // because of the AppendL().
            iDataToDpPtr.Set( iWriteBuffer->At( 0 )->Left(
                iWriteBuffer->At( 0 )->Length() ) );
            iDataPort.Write( iStatus, iDataToDpPtr );

#ifdef TF_LOGGING_ENABLED

            TBuf<KDataBufLength> data16bit;
            // If write buffer entry length is longer than KDataBufLength,
            // log only KDataBufLength bytes of data
            if ( KDataBufLength < iDataToDpPtr.Length() )
                {
                data16bit.Copy( iDataToDpPtr.Left( KDataBufLength ) );
                }
            else
                {
                data16bit.Copy( iDataToDpPtr.Left( iDataToDpPtr.Length() ) );
                }

            TFLOGSTRING2("TSY: CDataPortWriter::WriteNextL - AT command: %S", &data16bit);

OstTraceExt1( TRACE_NORMAL, DUP1_CDATAPORTWRITER_WRITENEXTL, "CDataPortWriter::WriteNextL;AT command=%S", data16bit );
#endif //TF_LOGGING_ENABLED

            // Set active as write is pending
            if ( !IsActive() )
                {
                SetActive();
                }
            //no else
            }
        //no else
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::DoCancel
// Cancels active object request.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortWriter::DoCancel()
    {

    TFLOGSTRING("CDataPortWriter::DoCancel");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_DOCANCEL, "CDataPortWriter::DoCancel" );

    // Stop writing into DP
    iContinueWrite = EFalse;
    iDataPort.WriteCancel();
    iWriteCompleted = ETrue;
    }

// ----------------------------------------------------------------------------
// CDataPortWriter::BlockWrite
// Blocks writing to dataport
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortWriter::BlockWrite()
    {

    TFLOGSTRING("TSY: CDataPortWriter::BlockWrite - Blocking writes to DP");
OstTrace0( TRACE_NORMAL, CDATAPORTWRITER_BLOCKWRITE, "CDataPortWriter::BlockWrite" );

    // Stop writing into DP until DP reader has flushed RX buffer
    iContinueWrite = EFalse;
    iWriteCompleted = EFalse;
    }

// ----------------------------------------------------------------------------
// CDataPortReader::NewL
// Two-phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CDataPortReader* CDataPortReader::NewL(
    CMmDataPortHandler* aDataPortHandler,
    const RComm& aDataPort,
    CDataPortWriter* aDataPortWriter )
    {

    TFLOGSTRING("TSY: CDataPortReader::NewL");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_NEWL, "CDataPortReader::NewL" );

    CDataPortReader* reader = new ( ELeave ) CDataPortReader();
    CleanupStack::PushL( reader );

    reader->iDataPortHandler = aDataPortHandler;
    reader->iDataPort = aDataPort;
    reader->iDataPortWriter = aDataPortWriter;

    CActiveScheduler::Add( reader );

    CleanupStack::Pop( reader );

    return reader;
    }

// ----------------------------------------------------------------------------
// CDataPortReader::CDataPortReader
// Dataport reader default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CDataPortReader::CDataPortReader()
    : CActive( EPriorityStandard )
    {

    TFLOGSTRING("TSY: CDataPortReader::CDataPortReader");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_CDATAPORTREADER, "CDataPortReader::CDataPortReader" );

    // initialisation of meneber variables
    iModemInitComplete = EFalse;
    iContinueRead = ETrue;
    iReaAllComplete = ETrue;
    }

// -----------------------------------------------------------------------------
// CDataPortReader::~CDataPortReader
// Destructor, deletes all allocated resources.
// -----------------------------------------------------------------------------
//
CDataPortReader::~CDataPortReader()
    {

    TFLOGSTRING("TSY: CDataPortReader::~CDataPortReader");
OstTrace0( TRACE_NORMAL, DUP1_CDATAPORTREADER_CDATAPORTREADER, "CDataPortReader::~CDataPortReader" );

    Cancel();
    }

// ----------------------------------------------------------------------------
// CDataPortReader::DoCancel
// Cancels active object request.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortReader::DoCancel()
    {

    TFLOGSTRING("CDataPortReader::DoCancel");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_DOCANCEL, "CDataPortReader::DoCancel" );

    // Stop reading from DP
    iContinueRead = EFalse;
    iDataPort.ReadCancel();
    }

// ----------------------------------------------------------------------------
// CDataPortReader::Read
// Reads data from dataport.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortReader::Read()
    {

    TFLOGSTRING("TSY: CDataPortReader::Read Start reading");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_READ, "CDataPortReader::Read" );

    iContinueRead = ETrue;
    // If already active, don't make a new request
    if ( !IsActive() )
        {
        iResp.SetLength( 0 );

        iDataPort.Read( iStatus, iResp );
        SetActive();
        }
    //no else
    }

// ----------------------------------------------------------------------------
// CDataPortReader::ReadAll
// Reads all data from dataport (flushing).
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortReader::ReadAll()
    {

    TFLOGSTRING("TSY: CDataPortReader::ReadAll - Read all data from DP");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_READALL, "CDataPortReader::ReadAll, Read all data from DP" );

    iContinueRead = ETrue;

    // First get remaining data length in DP
    TInt dataLen = iDataPort.QueryReceiveBuffer();
    // If there is data remaining in RX buffer, read it all first.
    // If modem init is not yet done, then do not empty RX buffer
    // as line object is doing initialisations before first dial.
    if ( ( 0 < dataLen ) && ( EFalse != iModemInitComplete ) )
        {

TFLOGSTRING2("TSY: CDataPortReader::ReadAll - Data remaining: %d",
              dataLen );
OstTrace1( TRACE_NORMAL, DUP1_CDATAPORTREADER_READALL, "CDataPortReader::ReadAll;Data remaining=%d", dataLen );
        // Block DP writer, so that it does not generate more
        // responses from modem. Continue when RX buffer is empty.
        iDataPortWriter->BlockWrite();
        // Set read all complete flag to false
        iReaAllComplete = EFalse;
        // If already active, don't make a new request. Wait for RunL
        if ( !IsActive() )
            {

            TFLOGSTRING("TSY: CDataPortReader::ReadAll - Starting to read remaining data (flushing)");
OstTrace0( TRACE_NORMAL, DUP2_CDATAPORTREADER_READALL, "CDataPortReader::ReadAll, Starting to read remaining data (flushing)" );

            iResp.SetLength( 0 );
            iDataPort.Read( iStatus, iResp );
            SetActive();
            }
        else
            {

            TFLOGSTRING("TSY: CDataPortReader::ReadAll - Read still pending!");
OstTrace0( TRACE_NORMAL, DUP3_CDATAPORTREADER_READALL, "CDataPortReader::ReadAll, Read still pending!" );
            }
        }
    else
        {

        TFLOGSTRING("TSY: CDataPortReader::ReadAll - Init going or no data in RX");
OstTrace0( TRACE_NORMAL, DUP4_CDATAPORTREADER_READALL, "CDataPortReader::ReadAll, Init going or no data in RX" );
        if ( 0 >= dataLen )
            {
            // Set read all complete flag to true, there is no data waiting
            iReaAllComplete = ETrue;
            }
        //no else

        // Read data if not already
        if ( !IsActive() )
            {

            TFLOGSTRING("TSY: CDataPortReader::ReadAll - Start normal reading");
OstTrace0( TRACE_NORMAL, DUP5_CDATAPORTREADER_READALL, "CDataPortReader::ReadAll, Start normal reading" );

            iResp.SetLength( 0 );
            iDataPort.Read( iStatus, iResp );
            SetActive();
            }
        //no else
        }
    }

// ----------------------------------------------------------------------------
// CDataPortReader::ReadCancel
// Cancels reading.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortReader::ReadCancel()
    {

    TFLOGSTRING("TSY: CDataPortReader::ReadCancel");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_READCANCEL, "CDataPortReader::ReadCancel" );

    // Stop reading from DP
    iContinueRead = EFalse;
    iDataPort.ReadCancel();
    }

// ----------------------------------------------------------------------------
// CDataPortReader::ModemInitComplete
// Called by line to inform DP reader that modem initialisation is completed.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortReader::ModemInitComplete()
    {

    TFLOGSTRING("TSY: CDataPortReader::ModemInitComplete");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_MODEMINITCOMPLETE, "CDataPortReader::ModemInitComplete" );

    iModemInitComplete = ETrue;
    }

// ----------------------------------------------------------------------------
// CDataPortReader::RunL
// Activates active object when request is completed.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CDataPortReader::RunL()
    {

    TFLOGSTRING("TSY: CDataPortReader::RunL");
OstTrace0( TRACE_NORMAL, CDATAPORTREADER_RUNL, "CDataPortReader::RunL" );

#ifdef TF_LOGGING_ENABLED

    TBuf< KDataBufLength > data16bit;
    data16bit.AppendNum( iStatus.Int() );

    TFLOGSTRING2("TSY: CDataPortReader::RunL - iStatus:%S", &data16bit);
OstTrace1( TRACE_NORMAL, DUP1_CDATAPORTREADER_RUNL, "CDataPortReader::RunL;iStatus=%d", iStatus.Int() );
#endif // TF_LOGGING_ENABLED

    // KErrNone and KErrCancel are ok, others mean real error
    TInt ret( iStatus.Int() );
    if (  KErrNone == ret || KErrCancel == ret )
        {

#ifdef TF_LOGGING_ENABLED
        data16bit.SetLength( 0 );
        // If iResp lenght is more than KDataBufLength,
        // log only KDataBufLength bytes
        if ( KDataBufLength < iResp.Length() )
            {
            data16bit.Copy( iResp.Left( KDataBufLength ) );
            }
        else
            {
            data16bit.Copy( iResp.Left( iResp.Length() ) );
            }
        const TDesC16 help = data16bit;
        TFLOGSTRING2("TSY: CDataPortReader::Read Data: %S", &data16bit);
OstTraceExt1( TRACE_NORMAL, DUP2_CDATAPORTREADER_RUNL, "CDataPortReader::RunL;data16bit=%S", help );
#endif // TF_LOGGING_ENABLED

        // If flushing the dataport and modem from last connection,
        // continue until RX buffer is empty
        if ( EFalse == iReaAllComplete )
            {
            // To ensure that NO CARRIER and +CRING (in case of waiting call)
            // responces are not missed.
            if ( 0 == iResp.MatchF( KModemNoCarrier ) ||
                 0 == iResp.MatchF( KModemCRing ) )
                {
                iDataPortHandler->ReadResponseFromDataport( iResp );
                }
            //no else

            TInt dataLen( iDataPort.QueryReceiveBuffer() );
            // If there is data remaining in RX buffer, read it all first
            if ( 0 < dataLen )
                {

                TFLOGSTRING2("TSY: CDataPortReader::RunL - Data remaining: %d", dataLen );

OstTrace1( TRACE_NORMAL, DUP3_CDATAPORTREADER_RUNL, "CDataPortReader::RunL;Data remaining=%d", dataLen );
                }
            else
                {

                TFLOGSTRING("TSY: CDataPortReader::RunL - RX buffer empty, normal read continuing");
OstTrace0( TRACE_NORMAL, DUP4_CDATAPORTREADER_RUNL, "CDataPortReader::RunL, RX buffer empty, normal read continuing" );

                iReaAllComplete = ETrue;
                // Request next write from iDataPortWriter
                iDataPortWriter->WriteNextL();
                }
            // Set length to zero
            iResp.SetLength( 0 );
            // Make the reader read again...
            iStatus = KRequestPending;
            iDataPort.Read( iStatus, iResp );
            SetActive();
            }
        // Normal read prosessing, forward data to line or call object
        else
            {
            // Check what is received from modem
            if ( 0 < iResp.Length() )
                {
                // Skip over line changes and unnecessary data.
                // CR is in byte 0 and LF is in byte 1.
                if ( ( KMinModemRespLength < iResp.Length() ) &&
                    ( KCarriageReturn != iResp[0] ) &&
                    ( KLineFeed != iResp[1] ) )
                    {
                    iDataPortHandler->ReadResponseFromDataport( iResp );
                    // If not echo data, send new commands.
                    // Otherwise skip over
                    if ( ( 0 > iResp.MatchF( KInitEcho ) ) )
                        {
                        // Request next write from iDataPortWriter
                        iDataPortWriter->WriteNextL();
                        }
                    //no else
                    }
                //no else

                // Set length to zero
                iResp.SetLength( 0 );

                if ( EFalse == iContinueRead )
                    {

                    TFLOGSTRING("TSY: CDataPortReader::RunL - Cancel reading");
OstTrace0( TRACE_NORMAL, DUP5_CDATAPORTREADER_RUNL, "CDataPortReader::RunL, Cancel reading" );

                    iDataPort.ReadCancel();
                    }
                else if ( !IsActive() )
                    {

                    TFLOGSTRING("TSY: CDataPortReader::RunL - Continue reading");
OstTrace0( TRACE_NORMAL, DUP6_CDATAPORTREADER_RUNL, "CDataPortReader::RunL, Continue reading" );
                    // Make the reader read again...
                    iStatus = KRequestPending;
                    iDataPort.Read( iStatus, iResp );
                    SetActive();
                    }
                }
            else if ( !IsActive() && iContinueRead )
                {

                TFLOGSTRING("TSY: CDataPortReader::RunL - Continue reading");
OstTrace0( TRACE_NORMAL, DUP7_CDATAPORTREADER_RUNL, "CDataPortReader::RunL, Continue reading" );
                // Make the reader read again...
                iStatus = KRequestPending;
                iDataPort.Read( iStatus, iResp );
                SetActive();
                }
            }
        }
    // If in use, do no do anything but wait for others to
    // end using dataport.
    else if ( KErrInUse == ret )
        {

        TFLOGSTRING("TSY: CDataPortReader::RunL - Dataport used by other client");
OstTrace0( TRACE_NORMAL, DUP8_CDATAPORTREADER_RUNL, "CDataPortReader::RunL, Dataport used by other client" );
        }
    // These will make the next call object creation fail in
    // CMmLineTsy::OpenNewObjectL as we can't handle these errors in TSY.
    else
        {
        User::LeaveIfError( ret );
        }
    }

// ----------------------------------------------------------------------------
// CDataPortReader::RunError
// Called when RunL leaves.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CDataPortReader::RunError(
    TInt aError )
    {

    TFLOGSTRING2("TSY: CDataPortReader::RunError %d", aError);
OstTrace1( TRACE_NORMAL, CDATAPORTREADER_RUNERROR, "CDataPortReader::RunError;aError=%d", aError );

    iDataPortHandler->HandleDataPortError( aError );

    return KErrNone;
    }

//=============================================================================

// OTHER EXPORTED FUNCTIONS
    //none

// End of File
