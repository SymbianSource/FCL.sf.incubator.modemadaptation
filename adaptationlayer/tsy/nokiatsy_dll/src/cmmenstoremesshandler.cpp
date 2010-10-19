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


// INCLUDE FILES

#include <tisi.h>
//#include <simisi.h>

#include "cmmenstoremesshandler.h"
#include "cmmmessagerouter.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmstaticutility.h"
#include "tsylogger.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmenstoremesshandlertraces.h"
#endif

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS

// These are doubled from CMmEnStoreTsy.h
const TUint8 KEccStorageNumberMaxCount = 5; // ETSI 11.11
const TUint8 KEmergencyNbrLenInBytes = 3;
const TUint8 KDigitMask = 0x0F;

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// ==================== LOCAL FUNCTIONS ========================================
    //None

// ==================== MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::CMmENStoreMessHandler
// C++ constructor.
// -----------------------------------------------------------------------------
//
CMmENStoreMessHandler::CMmENStoreMessHandler()
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::CMmENStoreMessHandler() - Start");
OstTrace0( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_CMMENSTOREMESSHANDLER_TD, "CMmENStoreMessHandler::CMmENStoreMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::~CMmENStoreMessHandler
// C++ destructor.
// -----------------------------------------------------------------------------
//
CMmENStoreMessHandler::~CMmENStoreMessHandler()
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::~CMmENStoreMessHandler() - End");
OstTrace0( TRACE_NORMAL,  DUP1_CMMENSTOREMESSHANDLER_CMMENSTOREMESSHANDLER_TD, "CMmENStoreMessHandler::~CMmENStoreMessHandler" );

    if( iEnNumberArray )
        {
        iEnNumberArray->ResetAndDestroy();
        delete iEnNumberArray;
        }
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::NewL
// Creates a new ENStoreMessageHandler object instance.
// Create EN store message handler
// Set internal variables
// Return Values: CMmENStoreMessHandler*: Pointer to object
// -----------------------------------------------------------------------------
//
CMmENStoreMessHandler* CMmENStoreMessHandler::NewL(
    CMmMessageRouter* aMessageRouter, // pointer to the message router
    CMmUiccMessHandler* aUiccMessHandler // pointer to UICC message handler
    )
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::NewL");
OstTrace0( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_NEWL_TD, "CMmENStoreMessHandler::NewL" );

    // Create EN store message handler
    CMmENStoreMessHandler* enStoreMessHandler =
        new( ELeave ) CMmENStoreMessHandler();

    // Set internal variables
    CleanupStack::PushL( enStoreMessHandler );
    enStoreMessHandler->iMessageRouter = aMessageRouter;
    enStoreMessHandler->iMmUiccMessHandler = aUiccMessHandler;
    enStoreMessHandler->ConstructL();


    CleanupStack::Pop( enStoreMessHandler );

    return enStoreMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmENStoreMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::ConstructL()");
OstTrace0( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_CONSTRUCTL_TD, "CMmENStoreMessHandler::ConstructL" );

    iEnNumberArray = new ( ELeave ) CArrayPtrFlat<TENStoreResponse>( 1 );
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::ProcessUiccMsg
// Handles data received from UICC server
// -----------------------------------------------------------------------------
//
TInt CMmENStoreMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY:CMmENStoreMessHandler::ProcessUiccMsg, aTraId: %d, status: %d", aTraId, aStatus );
OstTraceExt2( TRACE_NORMAL,  DUP2_CMMENSTOREMESSHANDLER_PROCESSUICCMSG_TD, "CMmENStoreMessHandler::ProcessUiccMsg;aTraId=%d;aStatus=%d", aTraId, aStatus );


    TInt ret( KErrNone );

    switch( aTraId )
        {
        case ETrIdEnStoreRead:
            {
            ret = UiccReadEmergCodesResp( aStatus, aFileData );
            break;
            }
        case ETrIdEnStoreGetInfo:
            {
            ret = UiccENStoreGetInfoResp( aStatus, aFileData );
            break;
            }
        default:
            {
TFLOGSTRING("TSY:CMmENStoreMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_PROCESSUICCMSG_TD, "CMmENStoreMessHandler::ProcessUiccMsg - unknown transaction ID" );
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::UiccReadEmergCodesResp
// Handles received ISI message from UICC Server.
// -----------------------------------------------------------------------------
//
TInt CMmENStoreMessHandler::UiccReadEmergCodesResp
        (
        TInt aStatus,
        const TDesC8& aFileData
        )
    {
TFLOGSTRING2("TSY:CMmENStoreMessHandler::UiccReadEmergCodesResp, status: %d", aStatus );
OstTrace1( TRACE_NORMAL,  DUP2_CMMENSTOREMESSHANDLER_UICCREADEMERGCODESRESP_TD, "CMmENStoreMessHandler::UiccReadEmergCodesResp;aStatus=%d", aStatus );

    TInt ret( KErrNone );
    // Create data package
    CMmDataPackage numberData;

    // Check the requesting function using transaction type and
    // process the matching function call
    switch ( iCurrentIpc )
        {
        case EMmTsyENStoreReadAllPhase1IPC:
            {
            if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                if ( UICC_STATUS_OK == aStatus )
                    {
                    // If we are reading first record,
                    // iEnNumberArray must be reseted
                    if ( 1 == iRecordNbr )
                        {
                        iEnNumberArray->ResetAndDestroy();
                        }
                    TENStoreResponse* enStoreEntry = NULL;
                    TBuf8<( RMobileENStore::KEmergencyNumberSize )> numberBuf;
                    TBuf8< KEmergencyNbrLenInBytes > readIn;

                    readIn.Copy( aFileData );

                    GetEmergencyNumberDigits( readIn, numberBuf );

                    if ( 0 < numberBuf.Length() )
                        {
                        enStoreEntry = new ( ELeave ) TENStoreResponse();
                        // Copy the number
                        enStoreEntry->iECCNumber.Copy( numberBuf );
                        enStoreEntry->iLocation = ( TUint8 )iRecordNbr;
                        iEnNumberArray->AppendL( enStoreEntry );
                        }
                    iRecordNbr++;
                    if ( iRecordNbr <= iEntryCount )
                        {
                        ret = UiccReadEmergCodesReq(
                            iRecordNbr,
                            ETrIdEnStoreRead );
                        }
                    else
                        {
                        iEntryCount = 0;
                        }
                    }
                else
                    {
                    iEntryCount = 0;
                    iEnNumberArray->ResetAndDestroy();

                    if( UICC_STATUS_NOT_READY == aStatus )
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotReady,
                            KErrGsmMMServiceOptionTemporaryOutOfOrder );
                        }
                    else
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotFound,
                            KErrGsm0707NotFound );
                        }
                    }
                }
            else if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType() )
                {
                if ( UICC_STATUS_OK == aStatus )
                    {
                    iEnNumberArray->ResetAndDestroy();

                    TENStoreResponse* enStoreEntry = NULL;

                    for( TInt index = 1;
                         index <= ( aFileData.Length() / KEmergencyNbrLenInBytes );
                         index++ )
                        {
                        TBuf8<( RMobileENStore::KEmergencyNumberSize )> numberBuf;
                        TBuf8< KEmergencyNbrLenInBytes > readIn;

TFLOGSTRING3("TSY: CMmENStoreMessHandler::UiccReadEmergCodesResp. length:%d, index:%d", aFileData.Length(), index);
OstTraceExt2( TRACE_NORMAL,  DUP4_CMMENSTOREMESSHANDLER_UICCREADEMERGCODESRESP_TD, "CMmENStoreMessHandler::UiccReadEmergCodesResp;aFileData.Length()=%d;index=%d", aFileData.Length(), index );

                        readIn.Copy(aFileData.Mid(
                            ((index * KEmergencyNbrLenInBytes ) - 3),
                            KEmergencyNbrLenInBytes));

                        GetEmergencyNumberDigits( readIn, numberBuf );

                        if ( 0 < numberBuf.Length() )
                            {
                            enStoreEntry = new ( ELeave ) TENStoreResponse();
                            // Copy the number
                            enStoreEntry->iECCNumber.Copy( numberBuf );
                            enStoreEntry->iLocation = ( TUint8 ) index;
                            iEnNumberArray->AppendL( enStoreEntry );
                            }
                        }
                    }
                else
                    {
                    iEntryCount = 0;

                    if( UICC_STATUS_NOT_READY == aStatus )
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotReady,
                            KErrGsmMMServiceOptionTemporaryOutOfOrder );
                        }
                    else
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotFound,
                            KErrGsm0707NotFound );
                        }
                    }
                }
            if ( iEntryCount == 0 )
                {
                // Complete response with the array of EN entries with ret
                // Pack the data for sending to the manager
                numberData.PackData( &iEnNumberArray );

                // Complete request
                iMessageRouter->Complete( iCurrentIpc, &numberData, ret );

                // Reset current IPC.
                iCurrentIpc = 0;
                }
            break;
            }

        // Read ECC from certain location
        case EMmTsyENStoreReadIPC:
            {
            // Create TENStoreResponse structure
            TENStoreResponse enStoreNumber;
            if ( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                if ( 1 <= iReadECCLocation )
                    {
                    if ( UICC_STATUS_OK == aStatus )
                        {
                        TBuf8<( RMobileENStore::KEmergencyNumberSize )> numberBuf;
                        // Read location
                        enStoreNumber.iLocation = ( TUint8 )iReadECCLocation;

                        TBuf8< KEmergencyNbrLenInBytes > readIn;

                        readIn.Copy( aFileData );

                        GetEmergencyNumberDigits( readIn, numberBuf );
                        enStoreNumber.iECCNumber.Copy( numberBuf );
                        }
                    else if( UICC_STATUS_NOT_READY == aStatus )
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotReady,
                            KErrGsmMMServiceOptionTemporaryOutOfOrder );
                        }
                    else
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotFound,
                            KErrGsm0707NotFound );
                        }
                    }
                else
                    {
                    // Location index is not valid. Complete request with error.
TFLOGSTRING2("TSY: CMmENStoreMessHandler::UiccReadEmergCodesResp\
              - Invalid index: %d", iReadECCLocation);
OstTrace1( TRACE_NORMAL,  DUP1_CMMENSTOREMESSHANDLER_UICCREADEMERGCODESRESP_TD, "CMmENStoreMessHandler::UiccReadEmergCodesResp - Invalid index:;iReadECCLocation=%d", iReadECCLocation );

                    ret = KErrArgument;
                    }
                }
            else if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType() )
                {
                // Check that location index is valid
                if ( 1 <= iReadECCLocation
                     && KEccStorageNumberMaxCount >= iReadECCLocation )
                    {
                    if ( UICC_STATUS_OK == aStatus )
                        {
                        TBuf8<( RMobileENStore::KEmergencyNumberSize )> numberBuf;
                        // Read location
                        enStoreNumber.iLocation = ( TUint8 )iReadECCLocation;

                        TBuf8< KEmergencyNbrLenInBytes > readIn;

                        readIn.Copy(aFileData.Mid(
                            ((iReadECCLocation * KEmergencyNbrLenInBytes) - 3),
                            ((iReadECCLocation * KEmergencyNbrLenInBytes) - 1 )));

                        GetEmergencyNumberDigits( readIn, numberBuf );
                        enStoreNumber.iECCNumber.Copy( numberBuf );
                        }
                    else if( UICC_STATUS_NOT_READY == aStatus )
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotReady,
                            KErrGsmMMServiceOptionTemporaryOutOfOrder );
                        }
                    else
                        {
                        ret = CMmStaticUtility::EpocErrorCode( KErrNotFound,
                            KErrGsm0707NotFound );
                        }
                    }
                else
                    {
                    // Location index is not valid. Complete request with error.
TFLOGSTRING2("TSY: CMmENStoreMessHandler::UiccReadEmergCodesResp - Invalid index: %d", iReadECCLocation);
OstTrace1( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_UICCREADEMERGCODESRESP_TD, "CMmENStoreMessHandler::UiccReadEmergCodesResp - Invalid index:;iReadECCLocation=%d", iReadECCLocation );
                    ret = KErrArgument;
                    }
                }
            // Complete response with ret
            // Pack the data for sending to the manager
            numberData.PackData( &enStoreNumber );

            // Complete failed request
            iMessageRouter->Complete( iCurrentIpc, &numberData, ret );

            // Reset current IPC.
            iCurrentIpc = 0;
            break;
            }

        // Read function for ECC info purposes
        case EMmTsyENStoreGetInfoIPC:
            {
            if ( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                if ( UICC_STATUS_OK == aStatus )
                    {
                    TBuf8< KEmergencyNbrLenInBytes > readIn;

                    readIn.Copy(aFileData.Mid( 0, KEmergencyNbrLenInBytes ));

                    TUint8 read_value = readIn[0] & KDigitMask;
                    if( KDigitMask != read_value )
                        {
                        iUsedEntries++;
                        }
                    iRecordNbr++;
                    if ( iRecordNbr <= iEntryCount )
                        {
                        ret = UiccReadEmergCodesReq(
                            iRecordNbr,
                            ETrIdEnStoreRead );
                        }
                    else
                        {
                        iEntryCount = 0;
                        }
                    }
                else if( UICC_STATUS_NOT_READY == aStatus )
                    {
                    ret = CMmStaticUtility::EpocErrorCode( KErrNotReady,
                        KErrGsmMMServiceOptionTemporaryOutOfOrder );
                    }
                else
                    {
                    ret = CMmStaticUtility::EpocErrorCode( KErrNotFound,
                        KErrGsm0707NotFound );
                    }
                }
            else if( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType() )
                {
                iUsedEntries = 0;
                if( UICC_STATUS_OK == aStatus )
                    {
                    for( TInt index = 1;
                        index <= ( aFileData.Length() / KEmergencyNbrLenInBytes );
                        index++ )
                        {
                        TBuf8< KEmergencyNbrLenInBytes > readIn;

                        readIn.Copy(aFileData.Mid(
                        ((index * KEmergencyNbrLenInBytes) - 3),
                        KEmergencyNbrLenInBytes));

                        TUint8 read_value = readIn[0] & KDigitMask;
                        if( KDigitMask != read_value )
                            {
                            iUsedEntries++;
                            }
                        }
                    iEntryCount = 0;
                    if( 0 == iUsedEntries )
                        {
                        ret = KErrNotFound;
                        }
                    }
                else if( UICC_STATUS_NOT_READY == aStatus )
                    {
                    ret = CMmStaticUtility::EpocErrorCode( KErrNotReady,
                        KErrGsmMMServiceOptionTemporaryOutOfOrder );
                    }
                else
                    {
                    ret = CMmStaticUtility::EpocErrorCode( KErrNotFound,
                        KErrGsm0707NotFound );
                    }
                }

            if ( 0 == iEntryCount )
                {
TFLOGSTRING2("TSY: CMmENStoreMessHandler::UiccReadEmergCodesResp. iUsedEntries:%d", iUsedEntries);
OstTrace1( TRACE_NORMAL,  DUP3_CMMENSTOREMESSHANDLER_UICCREADEMERGCODESRESP_TD, "CMmENStoreMessHandler::UiccReadEmergCodesResp;iUsedEntries=%d", iUsedEntries );

                // Complete response with ret
                // Pack the data for sending to the manager
                numberData.PackData( &iUsedEntries );

                iMessageRouter->Complete( iCurrentIpc, &numberData, ret );

                // Reset current IPC.
                iCurrentIpc = 0;
                // Reset entry count.
                iEntryCount = 0;
                }
            break;

            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::GetEmergencyNumberDigits
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmENStoreMessHandler::GetEmergencyNumberDigits(
        const TDesC8& aReadIn,
        TDes8& aNumberBuf)
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::GetEmergencyNumberDigits" );
OstTrace0( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_GETEMERGENCYNUMBERDIGITS_TD, "CMmENStoreMessHandler::GetEmergencyNumberDigits" );

    TUint8 read_value = 0;
    TBuf8<( RMobileENStore::KEmergencyNumberSize )> numberBuf;

    for( TInt position = 0 ; position < KEmergencyNbrLenInBytes ; position++ )
        {
        read_value = aReadIn[position] & KDigitMask;
        if( read_value != KDigitMask )
            {
            // Emergency number will be returned in ASCII
            aNumberBuf.Append( read_value + '0' );

            read_value = aReadIn[position] >> 4;
            if( read_value != KDigitMask )
                {
                // Emergency number will be returned in ASCII
                aNumberBuf.Append( read_value + '0' );
                }
            else
                {
                break;
                }
            }
        else
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::UiccENStoreGetInfoResp
// Handles received ISI message from UICC Server.
// -----------------------------------------------------------------------------
//
TInt CMmENStoreMessHandler::UiccENStoreGetInfoResp
        (
        TInt aStatus,
        const TDesC8& aFileData
        )
    {
TFLOGSTRING2("TSY:CMmENStoreMessHandler::UiccENStoreGetInfoResp, status: %d", aStatus );
OstTrace1( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_UICCENSTOREGETINFORESP_TD, "CMmENStoreMessHandler::UiccENStoreGetInfoResp;aStatus=%d", aStatus );

    TInt ret( KErrNone );

    // To be returned to ENStoreTsy, used entries
    iUsedEntries = 0;
    iRecordNbr = 1;

    if ( UICC_STATUS_OK == aStatus )
        {
        TFci fci( aFileData );
        iEntryCount = fci.GetNumberOfRecords();

        ret = UiccReadEmergCodesReq( iRecordNbr, ETrIdEnStoreRead );
        }
    else
        {
        if( UICC_STATUS_NOT_READY == aStatus )
            {
            ret = CMmStaticUtility::EpocErrorCode( KErrNotReady,
                KErrGsmMMServiceOptionTemporaryOutOfOrder );
            }
        else
            {
            ret = CMmStaticUtility::EpocErrorCode( KErrNotFound,
                KErrGsm0707NotFound );
            }
        // Create data package
        CMmDataPackage numberData;

        iUsedEntries = 0;
        // Complete response with ret
        // Pack the data for sending to the manager
        numberData.PackData( &iUsedEntries );

        iMessageRouter->Complete( iCurrentIpc, &numberData, ret );

        // Reset current IPC.
        iCurrentIpc = 0;
        // Reset entry count.
        iEntryCount = 0;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::SimReadEmergCodesReq
// Constructs an ISI-message to read information from SIM
// Create ECC number request message
// Set servicetype
// Set location, -1 used, when get info about used entries
// Data buffer for service type
// return call send message via phonet
// -----------------------------------------------------------------------------
//
TInt CMmENStoreMessHandler::UiccReadEmergCodesReq
        (
        TInt aIndex,     // Index of ECC
        TUiccTrId aTraId
        )
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::UiccReadEmergCodesReq");
OstTrace0( TRACE_NORMAL,  DUP1_CMMENSTOREMESSHANDLER_UICCREADEMERGCODESREQ_TD, "CMmENStoreMessHandler::UiccReadEmergCodesReq" );

    TInt ret( KErrNone );

    // Store location that is needed while reading single EN entry
    //iReadECCLocation = aIndex;

    // Set parameters for UICC_APPL_CMD_REQ message
    if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
        {
        TUiccReadLinearFixed cmdParams;

        cmdParams.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        cmdParams.trId = aTraId;
        cmdParams.fileId = KElemEmergencyCallCodes;
        cmdParams.fileIdSfi = UICC_ANY_SIZE;
        cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;

        // File id path
        cmdParams.filePath.Append( KMasterFileId >> 8 );
        cmdParams.filePath.Append( KMasterFileId );
        cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

        cmdParams.record = aIndex;
        cmdParams.dataAmount = KEmergencyNbrLenInBytes;
        cmdParams.dataOffset = 0;

        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( cmdParams );
        }
    else if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
        {

        TUiccReadTransparent cmdParams;

        cmdParams.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
        cmdParams.trId = aTraId;

        cmdParams.dataAmount = 0;
        cmdParams.dataOffset = 0;
        cmdParams.fileId = KElemEmergencyCallCodes;
        cmdParams.fileIdSfi = UICC_SFI_NOT_PRESENT;
        cmdParams.serviceType = UICC_APPL_READ_TRANSPARENT;

        // File id path
        cmdParams.filePath.Append( KMasterFileId >> 8 );
        cmdParams.filePath.Append( KMasterFileId );
        cmdParams.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( cmdParams );
        }
    else
        {
TFLOGSTRING("TSY: CMmENStoreMessHandler::SimReadEmergCodesReq: unknown card type.");
OstTrace0( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_UICCREADEMERGCODESREQ_TD, "CMmENStoreMessHandler::UiccReadEmergCodesReq - Unknown card type." );
        ret = KErrGeneral;
        }
TFLOGSTRING2("TSY: CMmENStoreMessHandler::UiccReadEmergCodesReq ret: %d", ret);
OstTrace1( TRACE_NORMAL,  DUP2_CMMENSTOREMESSHANDLER_UICCREADEMERGCODESREQ_TD, "CMmENStoreMessHandler::UiccReadEmergCodesReq;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::ReadEfEccFileInfo
// Sends file info request for EFecc
// -----------------------------------------------------------------------------
//
TInt CMmENStoreMessHandler::ReadEfEccFileInfo( TUiccTrId aTraId )
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::ReadEfEccFileInfo");
OstTrace0( TRACE_NORMAL,  DUP3_CMMENSTOREMESSHANDLER_READEFECCFILEINFO_TD, "CMmENStoreMessHandler::ReadEfEccFileInfo" );

    TInt ret( KErrNone );

    TUiccApplFileInfo params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.serviceType = UICC_APPL_FILE_INFO;
    params.fileId = KElemEmergencyCallCodes;
    params.fileIdSfi = 1;
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
    params.trId = aTraId;

    ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );

TFLOGSTRING2("TSY: CMmENStoreMessHandler::ReadEfEccFileInfo ret %d", ret);
OstTrace1( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_READEFECCFILEINFO_TD, "CMmENStoreMessHandler::ReadEfEccFileInfo;ret=%d", ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::ExtFuncL
// -----------------------------------------------------------------------------
//
TInt CMmENStoreMessHandler::ExtFuncL
        (
        TInt aIpc,
        const CMmDataPackage* aDataPackage
        )
    {
TFLOGSTRING2("TSY: CMmENStoreMessHandler::ExtFuncL - Ipc: %d", aIpc);
OstTrace1( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_EXTFUNCL_TD, "CMmENStoreMessHandler::ExtFuncL;aIpc=%d", aIpc );

    // Unpack parameters
    TInt index( 0 );
    TInt ret( KErrNone );
    TUiccTrId traId( ETrIdNotDefined );
    aDataPackage->UnPackData( index );

    // Save IPC value. Will be used to complete the request appropriately
    iCurrentIpc = aIpc;

    switch ( aIpc )
        {
        case EMmTsyENStoreReadAllPhase1IPC:
            {
            if ( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                traId = ETrIdEnStoreGetInfo;
                ret = ReadEfEccFileInfo( traId );
                }
            else if ( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType() )
                {
                traId = ETrIdEnStoreRead;
                ret = UiccReadEmergCodesReq( index, traId );
                }
            break;
            }
        case EMmTsyENStoreReadIPC:
            {
            traId = ETrIdEnStoreRead;
            iReadECCLocation = index;
            ret = UiccReadEmergCodesReq( index, traId );
            break;
            }
        case EMmTsyENStoreGetInfoIPC:
            {
            traId = ETrIdEnStoreGetInfo;
            if ( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
                {
                ret = ReadEfEccFileInfo( traId );
                }
            else if ( UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType() )
                {
                traId = ETrIdEnStoreRead;
                ret = UiccReadEmergCodesReq( index, traId );
                }
            else
                {
TFLOGSTRING("TSY: CMmENStoreMessHandler::ExtFuncL: unknown card type.");
OstTrace0( TRACE_NORMAL,  DUP1_CMMENSTOREMESSHANDLER_EXTFUNCL_TD, "CMmENStoreMessHandler::ExtFuncL - unknown card type." );
               ret = KErrGeneral;
                }
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmENStoreMessHandler::SimReadEmergCodesRespL. Switch aIpc case default");
OstTrace0( TRACE_NORMAL,  DUP2_CMMENSTOREMESSHANDLER_EXTFUNCL_TD, "CMmENStoreMessHandler::ExtFuncL - Switch aIpc case default" );
           // Do nothing
            break;
            }
        }


    return ret;
    }

// -----------------------------------------------------------------------------
// CMmENStoreMessHandler::HandleError
// Handles CMmENStoreMessHandler's errors that comes via PhoNetReceiver RunError
// method.
// -----------------------------------------------------------------------------
//
void CMmENStoreMessHandler::HandleError
        (
        const TIsiReceiveC& /*aIsiMessage*/,    // Isi message
        TInt /*aError*/    // Error code
        )
    {
TFLOGSTRING("TSY: CMmENStoreMessHandler::HandleError");
OstTrace0( TRACE_NORMAL,  CMMENSTOREMESSHANDLER_HANDLEERROR_TD, "CMmENStoreMessHandler::HandleError" );
    }

// ================= OTHER EXPORTED FUNCTIONS ==================================
    //None

// End of File
