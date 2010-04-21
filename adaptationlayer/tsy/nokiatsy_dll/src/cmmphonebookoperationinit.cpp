/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/

// INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include <pn_const.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationinit.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationinitTraces.h"
#include "cmmphonebookoperationinit3G_adn.h"
#endif

// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS

// MACROS
    // None

// LOCAL CONSTANTS AND MACROS
    // None

// MODULE DATA STRUCTURES
    // None

// LOCAL FUNCTION PROTOTYPES
    // None

// ==================== LOCAL FUNCTIONS =====================================
    // None


// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::CMmPhoneBookOperationInit
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

CMmPhoneBookOperationInit::CMmPhoneBookOperationInit
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CmmPhonebookOperatorInit::CmmPhonebookOperatorInit");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_CMMPHONEBOOKOPERATIONINIT, "CMmPhoneBookOperationInit::CMmPhoneBookOperationInit" );

    iTypeOfReading = EBasicEfRead;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::~CMmPhoneBookOperationInit
// C++ destructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationInit::~CMmPhoneBookOperationInit
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::~CMmPhoneBookOperationInit");
OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT_CMMPHONEBOOKOPERATIONINIT, "CMmPhoneBookOperationInit::~CMmPhoneBookOperationInit" );
    delete iPBStoreInfoData;
    }

// -----------------------------------------------------------------------------
// CmmPhonebookOperatorInit::NewL
// Two-phased constructor.
// Creates a new CmmPhonebookOperatorInit object instance.
// -----------------------------------------------------------------------------

CMmPhoneBookOperationInit* CMmPhoneBookOperationInit::NewL
    (
    CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
    CMmUiccMessHandler* aUiccMessHandler,
    const CMmDataPackage* aDataPackage // Data
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::NewL");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_NEWL, "CMmPhoneBookOperationInit::NewL" );

    TName phonebookTypeName;

    CMmPhoneBookOperationInit* mmPhoneBookOperationInit =
        new( ELeave ) CMmPhoneBookOperationInit();

    CleanupStack::PushL( mmPhoneBookOperationInit );

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    phoneBookData->GetPhoneBookName( phonebookTypeName );
    //Store phonebook name
    mmPhoneBookOperationInit->iPhoneBookTypeName = phonebookTypeName;

    if( 0 == phonebookTypeName.CompareF( KInternalPhoneBookType ) )
        {
        mmPhoneBookOperationInit->iIniPhase = EPBIniPhase_Internal;
        }
    else // Full construction is not needed in case of internal initilization
        {
        mmPhoneBookOperationInit->ConstructL( );
        }

    mmPhoneBookOperationInit->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;

    mmPhoneBookOperationInit->iMmUiccMessHandler = aUiccMessHandler;

    // set to false by default. Will be set as true once after
    // initialization req has been generated.
    mmPhoneBookOperationInit->iInternalInit = EFalse;

    CleanupStack::Pop( mmPhoneBookOperationInit );

    return mmPhoneBookOperationInit;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationInit::ConstructL
    (
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::ConstructL");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_CONSTRUCTL, "CMmPhoneBookOperationInit::ConstructL" );

    iPBStoreInfoData = new( ELeave ) CStorageInfoData();

    iServiceType = 0;
    iNumOfPBRRecords = 0;
    iADNPbInitilized = EFalse;
    }




// TRANSMIT



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::UICCCreateReq
// Create Request for Phonebook initialization for SIM
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit::UICCCreateReq
    (
    TInt aIpc,
    const CMmDataPackage* /*aDataPackage*/,
    TUint8 aTransId
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCCreateReq");
OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_UICCCREATEREQ, "CMmPhoneBookOperationInit::UICCCreateReq" );
    TInt ret( KErrNotSupported );

    switch( aIpc )
        {
        case EMmTsyPhoneBookStoreInitIPC:
            {
            if( !iInternalInit )
                {
                // no internal init ongoing
                if ( EPBIniPhase_Internal == iIniPhase )
                    {
                    // Initialization is starting. Turn the flag on.
                    iInternalInit = ETrue;
                    }
                iServiceType = UICC_APPL_FILE_INFO;

                // Start Initialization for 2G phonebooks
                // Check for ADN Phonebook is available and activated
                if(iMmUiccMessHandler->GetServiceStatus( ICC_ADN_SERVICE_NUM))
                    {
                    iIniPhase = EPBInitPhaseADN;
                    }
                else
                    {
                    iIniPhase = GetNextAvailablePbIcc(ICC_ADN_SERVICE_NUM);
                    }
                ret = UICCInitializeReq(aTransId);
                }
            else  // else for if internalInit goign on
                {
                // Simultaneous initializations are not allowed.
                ret = KErrInUse;
                }
            break;
            }
        default:
            {
            // Nothing to do here
            TFLOGSTRING2("TSY: CMmPhoneBookOperationInit::UICCCreateReq - \
            Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT_UICCCREATEREQ, "CMmPhoneBookOperationInit::UICCCreateReq;Unknowns aIpc=%d", aIpc );
            break;
            }
        } // switch-case ends
    return ret;
    }


// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::UICCInitializeReq
// Creates phonebook initialize request data for SIM
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit::UICCInitializeReq( TUint8 aTransId )
    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq");
OstTrace0( TRACE_FATAL, CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq" );

        TInt ret( KErrNone );
        TInt appFileID ( APPL_FILE_ID );   // Application File id for DFphonebook
        TUiccReadLinearFixed cmdParams;

        cmdParams.messHandlerPtr = static_cast<MUiccOperationBase*>(iMmPhoneBookStoreMessHandler);
        cmdParams.trId = static_cast<TUiccTrId>( aTransId );

        cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
        cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
        cmdParams.filePath.Append(appFileID>>8);
        cmdParams.filePath.Append(appFileID);
        if( UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType() )
            {
            cmdParams.filePath.Append(static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
            cmdParams.filePath.Append(static_cast<TUint8>( DF_PHONEBOOK ));
            }

        switch( iIniPhase )
            {
            case EPBInitPhaseADN:
                {
                // Create Data for ADN EF read
                if( EBasicEfRead == iTypeOfReading )
                    {
                    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - ADN Init for Fileinfo OR FileData");
                    OstTrace0( TRACE_FATAL, DUP1_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - ADN Init for Fileinfo OR FileData" );
                    // Send Request to read File Info or data from EFadn (depends upon iServiceType)
                    cmdParams.fileId = PB_ADN_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                else if( EExtensionRead == iTypeOfReading )
                    {
                    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - ADN Phonebook Init Extension request");
                    OstTrace0( TRACE_FATAL, DUP2_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - ADN Phonebook Init Extension request" );

                    // Initialization for EXT1
                    cmdParams.fileId = PB_EXT1_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                break;
                }
            case EPBInitPhaseFDN:
                {
                // Create Data for FDN EF read
                if( EBasicEfRead == iTypeOfReading )
                    {
                    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - FDN Init FileInfo OR FileData request");
                    OstTrace0( TRACE_FATAL, DUP3_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq -  FDN Init FileInfo OR FileData request" );

                    cmdParams.fileId = PB_FDN_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                else if( EExtensionRead == iTypeOfReading )
                    {
                    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - FDN Init Extension request");
                    OstTrace0( TRACE_FATAL, DUP4_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - FDN Init Extension request" );

                    // Initialization for EXT2
                    cmdParams.fileId = PB_EXT2_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                break;
                }
            case EPBInitPhaseSDN:
                {
                if( EBasicEfRead == iTypeOfReading )
                    {
                    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - SDN Init FileInfo OR FileData request");
                    OstTrace0( TRACE_FATAL, DUP5_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - SDN Init FileInfo OR FileData request" );

                    cmdParams.fileId = PB_SDN_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                else if(  EExtensionRead == iTypeOfReading )
                    {
                    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - SDN Init Extension request");
                    OstTrace0( TRACE_FATAL, DUP6_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - SDN Init Extension request" );

                    // Initialization for EXT3
                    cmdParams.fileId = PB_EXT3_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                break;
                }
            case EPBInitPhaseMBDN:
                {
                // Start with MBI reading
                if( EBasicEfRead == iTypeOfReading )
                    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - MBDN Init FileInfo OR FileData request");
OstTrace0( TRACE_FATAL, DUP7_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - MBDN Init FileInfo OR FileData request" );

                    cmdParams.fileId = PB_MBDN_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                else if( EExtensionRead == iTypeOfReading )
                    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - MBDN Init Extension request");
OstTrace0( TRACE_FATAL, DUP8_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - MBDN Init Extension request" );

                    cmdParams.fileId = PB_EXT6_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                else if( EMailboxIdRead == iTypeOfReading )
                    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - MBDN Init MBI request");
OstTrace0( TRACE_FATAL, DUP14_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - MBDN Init MBI request" );

                    cmdParams.fileId = PB_MBI_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                break;
                }
            case EPBInitPhaseMSISDN:
                {
                if( EBasicEfRead == iTypeOfReading )
                    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - MSISDN Init FileInfo OR FileData request");
OstTrace0( TRACE_FATAL, DUP9_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - MSISDN Init FileInfo OR FileData request" );

                    cmdParams.fileId = PB_MSISDN_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                else if( EExtensionRead == iTypeOfReading )
                    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - MSISDN Init Extension request");
OstTrace0( TRACE_NORMAL, DUP10_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq" );

                    if ( UICC_CARD_TYPE_ICC == 
                    	       iMmUiccMessHandler->GetCardType() )
                        {
                        cmdParams.fileId = PB_EXT1_FID;
                        }
                    else if ( UICC_CARD_TYPE_UICC == 
                    	            iMmUiccMessHandler->GetCardType() )
                    	  {                    
                        cmdParams.fileId = PB_EXT5_FID;
                        }
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                break;
                }
            case EPBInitPhaseVMBX:
                {
                if( EBasicEfRead == iTypeOfReading )
                    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - VMBX Init FileInfo OR FileData request");
OstTrace0( TRACE_FATAL, DUP11_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - VMBX Init FileInfo OR FileData request" );

                    cmdParams.fileId = PB_VMBX_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                else if( EExtensionRead == iTypeOfReading )
                    {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - VMBX Init Extension request");
OstTrace0( TRACE_NORMAL, DUP13_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - VMBX Init Extension request" );

                    cmdParams.fileId = PB_EXT1_FID;
                    cmdParams.serviceType = iServiceType;
                    cmdParams.record = 0;
                    }
                break;
                }
            default:
                {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::UICCInitializeReq - PhoneBook not supported ");
OstTrace0( TRACE_NORMAL, DUP12_CMMPHONEBOOKOPERATIONINIT_UICCINITIALIZEREQ, "CMmPhoneBookOperationInit::UICCInitializeReq - PhoneBook not supported" );
                break;
                }
            }

    // if intialization has not been completed till now then send the
    // request for the correct phase

    if( EPBIniPhase_PBInitialized != iIniPhase )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->CreateUiccApplCmdReq( cmdParams );
TFLOGSTRING2("TSY: CreateUiccApplCmdReq returns %d", ret);
        }
    return ret;
    }





/// RECEPTION



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::HandleUICCPbResp
// Handle Response for SIM
// -----------------------------------------------------------------------------
//
TBool CMmPhoneBookOperationInit::HandleUICCPbRespL
    (
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8 &aFileData,
    TInt aTransId
    )
    {
    TInt ret( KErrNone );
    TBool complete( EFalse );
    // break immediatelly in case of internal init
    if ( iInternalInit )
        {
TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleUICCPbRespL. Internal Init->Break");
OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONINIT_HANDLEUICCPBRESPL, "CMmPhoneBookOperationInit::HandleUICCPbRespL, Internal Init->Break" );

        iIniPhase = EPBIniPhase_Unknown;

        // set flag to indicate that we can remove this operation from array
        complete = ETrue;
        iInternalInit = EFalse;
        return KErrNone;
        }

    // Handle recponse from UICC server
        ret = HandlePBRespL(aFileData,aStatus, aTransId);


         // Complete, if phonebook initalization is complete or there is some error in UICC server
         if (KErrNone != ret || EPBIniPhase_PBInitialized == iIniPhase )
             {
             CPhoneBookDataPackage phoneBookData;
             phoneBookData.SetPhoneBookName( iPhoneBookTypeName );
             phoneBookData.PackData( iPBStoreInfoData );

             if ( KErrNone != ret )
                 {
                 ret = CMmStaticUtility::UICCCSCauseToEpocError( ret );
                 }

             iMmPhoneBookStoreMessHandler->MessageRouter()->Complete(
                     EMmTsyPhoneBookStoreInitIPC,
                     &phoneBookData,
                     ret );
             complete = ETrue;
             }
    return complete;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::HandlePBRespL
// Further Handle Response for SIM
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit::HandlePBRespL(const TDesC8& aFileData, TInt aStatus, TUint8 aTransId )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandlePBRespL");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_HANDLEPBRESPL, "CMmPhoneBookOperationInit::HandlePBRespL" );

    TInt ret(KErrNone);

    if( EBasicEfRead == iTypeOfReading )
        {
        ret = HandleFileResp(aFileData, aStatus);
        }
    else if( EExtensionRead == iTypeOfReading )
        {
        // For Extension File
        ret = HandleEXTFileResp( aFileData, aStatus );
        }
    else
        {
        ret = HandleMBIFileResp( aFileData, aStatus );
        }

    if(KErrNone == ret)
        {
        ret = UICCInitializeReq( aTransId );
        }

    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::HandleFileResp
// Further Handle Response for phonebook EF Files
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit::HandleFileResp(const TDesC8 &aFileData, TInt aStatus)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp" );


    TInt ret(KErrNone);
    TInt numOfEntries(0);
    TInt textLength(0);
    TInt numLength(0);
    TInt recordLength(0);

    if(UICC_STATUS_OK == aStatus)
        {
        if(UICC_APPL_FILE_INFO == iServiceType)
            {
            // get the record length and number of entries
            TFci fci( aFileData );
            recordLength = fci.GetRecordLength();
            numOfEntries = fci.GetNumberOfRecords();
 
            // get Text length
            textLength = (recordLength - 14);

            // To get the total length of number, need to read Ext2 file
            // IF EXT2 is not present then only 20 BCD digits can be stored
            numLength =  UICC_NO_EXT_MAX_NUM_LEN;
            }
        }

        switch(iIniPhase)
            {
            case EPBInitPhaseADN:
                {
                if( UICC_STATUS_OK == aStatus )
                    {
                    if(UICC_APPL_FILE_INFO == iServiceType)
                        {
                        TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for ADN Phonebook");
                        OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for ADN Phonebook" );
                        // Check for ADN phone book is incalidated by Fdn phonebook or not
                        TInt status(0);
                        TFci fci( aFileData );
                        status = fci.GetFileStatus();

                        // check for ADN is invalidated or not
                        if(!(status & 0x01))
                            {
                            // Check for ADN is updatable or not when ADN is invalidated
                            if( 0x04 == (status & 0x04))
                                {
                                iPBStoreInfoData->iADNNumOfEntries = numOfEntries;
                                iPBStoreInfoData->iADNNumberLengthMax = numLength;
                                iPBStoreInfoData->iADNTextLengthMax = textLength;
                                }
                            else   // when ADN is not updatable
                                {
                                iServiceType = UICC_APPL_FILE_INFO;
                                iIniPhase = EPBInitPhaseFDN;
                                }
                            }
                        else   // When ADN Phoenbook is not invalidated
                            {
                            iPBStoreInfoData->iADNNumOfEntries = numOfEntries;
                            iPBStoreInfoData->iADNNumberLengthMax = numLength;
                            iPBStoreInfoData->iADNTextLengthMax = textLength;
                            }
                        }
                    if( !iMmUiccMessHandler->GetServiceStatus( ICC_EXT1_SERVICE_NUM))
                        {
                        // Store ADN Phone book configuration in local array if EXT is not present
                        TPrimitiveInitInfo primConfAdn;
                        primConfAdn.iAlphaStringlength = iPBStoreInfoData->iADNTextLengthMax;
                        primConfAdn.iExtension = EFalse;
                        primConfAdn.iNumlength = iPBStoreInfoData->iADNNumberLengthMax;
                        primConfAdn.iNoOfRecords = iPBStoreInfoData->iADNNumOfEntries;

                        iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeAdn] = primConfAdn;

                        iTypeOfReading = EBasicEfRead;
                        iIniPhase = GetNextAvailablePbIcc(ICC_ADN_SERVICE_NUM);
                        }
                    else
                        {
                        // When Service is available
                        iTypeOfReading = EExtensionRead;
                        }
                    // ADN Phonebook is initilized
                    iADNPbInitilized = ETrue;
                    }
                else
                    {
                    // reset all the data is init is incomplete
                    iPBStoreInfoData->iADNNumOfEntries = -1;
                    iPBStoreInfoData->iADNNumberLengthMax = -1;
                    iPBStoreInfoData->iADNTextLengthMax = -1;

                    // Set ADN intialized flag to False
                    iADNPbInitilized = EFalse;
                    }
                }
                break;
            case EPBInitPhaseFDN:
                {
                if( UICC_STATUS_OK == aStatus )
                    {
                    if(UICC_APPL_FILE_INFO == iServiceType)
                        {
                        TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for FDN Phonebook");
                        OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for FDN Phonebook" );

                        iPBStoreInfoData->iFDNNumOfEntries = numOfEntries;
                        iPBStoreInfoData->iFDNNumberLengthMax = numLength;
                        iPBStoreInfoData->iFDNTextLengthMax = textLength;
                        }
                    if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                        {
                        // Check for Exension
                        // Check for EXT2 Service is present or not
                        if( !iMmUiccMessHandler->GetServiceStatus( ICC_EXT2_SERVICE_NUM ) )
                            {
                            // Store FDN Phone book configuration in local array if EXT is not present in ICC Card
                            TPrimitiveInitInfo primConfFdn;
                            primConfFdn.iAlphaStringlength = iPBStoreInfoData->iFDNTextLengthMax;
                            primConfFdn.iExtension = EFalse;
                            primConfFdn.iNumlength = iPBStoreInfoData->iFDNNumberLengthMax;
                            primConfFdn.iNoOfRecords = iPBStoreInfoData->iFDNNumOfEntries;

                            iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeFdn] = primConfFdn;

                            iTypeOfReading = EBasicEfRead;
                            iIniPhase = GetNextAvailablePbIcc(ICC_FDN_SERVICE_NUM);
                            }
                        else
                            {
                            iTypeOfReading = EExtensionRead ;
                            }
                        }
                    else if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                        {
                        // Chekc for Extension
                        // Check for EXT1 Service is present or not
                        if( !iMmUiccMessHandler->GetServiceStatus( UICC_EXT2_SERVICE_NUM ) )
                            {
                            // Store FDN Phone book configuration in local array if EXT is not present in UICC Card
                            TPrimitiveInitInfo primConfFdn;
                            primConfFdn.iAlphaStringlength = iPBStoreInfoData->iFDNTextLengthMax;
                            primConfFdn.iExtension = EFalse;
                            primConfFdn.iNumlength = iPBStoreInfoData->iFDNNumberLengthMax;
                            primConfFdn.iNoOfRecords = iPBStoreInfoData->iFDNNumOfEntries;

                            iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeFdn] = primConfFdn;

                            iTypeOfReading = EBasicEfRead;
                            iIniPhase = GetNextAvailablePbUicc(UICC_FDN_SERVICE_NUM);
                            }
                        else
                            {
                            iTypeOfReading = EExtensionRead;
                            }
                        }
                    iServiceType = UICC_APPL_FILE_INFO;
                    }
                else
                    {
                    // Reset all the Data if initilized incomplete
                    iPBStoreInfoData->iFDNNumOfEntries = -1;
                    iPBStoreInfoData->iFDNNumberLengthMax = -1;
                    iPBStoreInfoData->iFDNTextLengthMax = -1;

                    // Check for ADN Phonebook is Initilized or not
                    if(EFalse == iADNPbInitilized)
                        {
                        ret = KErrGeneral;
                        }
                    }
                }
                break;
            case EPBInitPhaseSDN:
                {
                if( UICC_STATUS_OK == aStatus )
                    {
                    if(UICC_APPL_FILE_INFO == iServiceType)
                        {
                        TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for SDN Phonebook");
                        OstTrace0( TRACE_NORMAL, DUP5_CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for SDN Phonebook" );

                        iPBStoreInfoData->iSDNNumOfEntries = numOfEntries;
                        iPBStoreInfoData->iSDNNumberLengthMax = numLength;
                        iPBStoreInfoData->iSDNTextLengthMax = textLength;
                        }
                    if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                        {
                        // Check for Exension
                        // Check for EXT1 Service is present or not
                        if( !iMmUiccMessHandler->GetServiceStatus( ICC_EXT3_SERVICE_NUM ) )
                            {
                            // Store SDN Phone book configuration in local array if EXT is not present in UICC Card
                            TPrimitiveInitInfo primConfSdn;
                            primConfSdn.iAlphaStringlength = iPBStoreInfoData->iSDNTextLengthMax;
                            primConfSdn.iExtension = EFalse;
                            primConfSdn.iNumlength = iPBStoreInfoData->iSDNNumberLengthMax;
                            primConfSdn.iNoOfRecords = iPBStoreInfoData->iSDNNumOfEntries;

                            iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeSdn] = primConfSdn;

                            iTypeOfReading = EBasicEfRead;
                            iIniPhase = GetNextAvailablePbIcc(ICC_SDN_SERVICE_NUM);
                            }
                        else
                            {
                            iTypeOfReading = EExtensionRead;
                            }
                        }
                    else if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                        {
                        // Chekc for Extension
                        // Check for EXT3 Service is present or not
                        if( !iMmUiccMessHandler->GetServiceStatus( UICC_EXT3_SERVICE_NUM ) )
                            {
                            // Store SDN Phone book configuration in local array if EXT is not present in UICC Card
                            TPrimitiveInitInfo primConfSdn;
                            primConfSdn.iAlphaStringlength = iPBStoreInfoData->iSDNTextLengthMax;
                            primConfSdn.iExtension = EFalse;
                            primConfSdn.iNumlength = iPBStoreInfoData->iSDNNumberLengthMax;
                            primConfSdn.iNoOfRecords = iPBStoreInfoData->iSDNNumOfEntries;

                            iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeSdn] = primConfSdn;

                            iTypeOfReading = EBasicEfRead;
                            iIniPhase = GetNextAvailablePbUicc(UICC_SDN_SERVICE_NUM);
                            }
                        else
                            {
                            iTypeOfReading = EExtensionRead;
                            }
                        }
                    iServiceType = UICC_APPL_FILE_INFO;
                    }
                else
                    {
                    // Reset all the parameters if init incomplete
                    iPBStoreInfoData->iSDNNumOfEntries = -1;
                    iPBStoreInfoData->iSDNNumberLengthMax = -1;
                    iPBStoreInfoData->iSDNTextLengthMax = -1;
                    }
                }
                break;
            case EPBInitPhaseMBDN:
                {
                if( UICC_STATUS_OK == aStatus )
                    {
                    if(UICC_APPL_FILE_INFO == iServiceType)
                        {
                        TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for MBDN Phonebook");
                        OstTrace0( TRACE_NORMAL, DUP7_CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for MBDN Phonebook" );

                        iPBStoreInfoData->iMBDNNumOfEntries = numOfEntries;
                        iPBStoreInfoData->iMBDNNumberLengthMax = numLength;
                        iPBStoreInfoData->iMBDNTextLengthMax = textLength;
                        }
                    iTypeOfReading = EExtensionRead;
                    iServiceType = UICC_APPL_FILE_INFO;
                    }
                else
                    {
                    // reset all the paramenters if init ic incomplete
                    iPBStoreInfoData->iMBDNNumOfEntries = -1;
                    iPBStoreInfoData->iMBDNNumberLengthMax = -1;
                    iPBStoreInfoData->iMBDNTextLengthMax = -1;
                    }
                }
                break;
            case EPBInitPhaseMSISDN:
                {
                if( UICC_STATUS_OK == aStatus )
                    {
                    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for MSISDN Phonebook");
                    OstTrace0( TRACE_NORMAL, DUP9_CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for MSISDN Phonebook" );

                    if(UICC_APPL_FILE_INFO == iServiceType)
                        {
                        iPBStoreInfoData->iMSISDNNumOfEntries = numOfEntries;
                        iPBStoreInfoData->iMSISDNNumberLengthMax = numLength;
                        iPBStoreInfoData->iMSISDNTextLengthMax = textLength;
                        }
                        if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                            {
                            // Check for Exension
                            // Check for EXT1 Service is present or not
                            if( !iMmUiccMessHandler->GetServiceStatus( ICC_EXT1_SERVICE_NUM ) )
                                {
                                // Store MSISDN Phone book configuration in local array if EXT is not present in ICC Card
                                TPrimitiveInitInfo primConfMsisdn;
                                primConfMsisdn.iAlphaStringlength = iPBStoreInfoData->iMSISDNTextLengthMax;
                                primConfMsisdn.iExtension = EFalse;
                                primConfMsisdn.iNumlength = iPBStoreInfoData->iMSISDNNumberLengthMax;
                                primConfMsisdn.iNoOfRecords = iPBStoreInfoData->iMSISDNNumOfEntries;

                                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeMSISDN] = primConfMsisdn;

                                iTypeOfReading = EBasicEfRead;
                                iIniPhase = GetNextAvailablePbIcc(ICC_MSISDN_SERVICE_NUM);
                                }
                            else
                                {
                                iTypeOfReading = EExtensionRead;
                                }
                            }
                        else if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                            {
                            // Chekc for Extension
                            // Check for EXT1 Service is present or not
                            if( !iMmUiccMessHandler->GetServiceStatus( UICC_EXT5_SERVICE_NUM ) )
                                {
                                // Store MSISDN Phone book configuration in local array if EXT is not present in UICC Card
                                TPrimitiveInitInfo primConfMsisdn;
                                primConfMsisdn.iAlphaStringlength = iPBStoreInfoData->iMSISDNTextLengthMax;
                                primConfMsisdn.iExtension = EFalse;
                                primConfMsisdn.iNumlength = iPBStoreInfoData->iMSISDNNumberLengthMax;
                                primConfMsisdn.iNoOfRecords = iPBStoreInfoData->iMSISDNNumOfEntries;

                                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeMSISDN] = primConfMsisdn;

                                iTypeOfReading = EBasicEfRead;
                                iIniPhase = GetNextAvailablePbUicc(UICC_MSISDN_SERVICE_NUM);
                                }
                            else
                                {
                                iTypeOfReading = EExtensionRead;
                                }
                            }
                        iServiceType = UICC_APPL_FILE_INFO;
                    }
                else
                    {
                    // reste all the parameters if init is incomplete
                    iPBStoreInfoData->iMSISDNNumOfEntries = -1;
                    iPBStoreInfoData->iMSISDNNumberLengthMax = -1;
                    iPBStoreInfoData->iMSISDNTextLengthMax = -1;
                    }
                }
                break;
            case EPBInitPhaseVMBX:
                {
                if( UICC_STATUS_OK == aStatus )
                    {
                    if(UICC_APPL_FILE_INFO == iServiceType)
                        {
                        TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for VMBX Phonebook");
                        OstTrace0( TRACE_NORMAL, DUP13_CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp - Handle File Info for VMBX Phonebook" );

                        // update Data in CommonTSY buffer
                        iPBStoreInfoData->iVMBXNumOfEntries = numOfEntries;
                        iPBStoreInfoData->iVMBXNumberLengthMax = numLength;
                        iPBStoreInfoData->iVMBXTextLengthMax = textLength;
                        }
                    // Check for Exension
                    // Check for EXT1 Service is present or not
                    if( !iMmUiccMessHandler->GetServiceStatus( ICC_EXT1_SERVICE_NUM))
                        {
                        // Store VMBX Phone book configuration in local array if EXT is not present
                        TPrimitiveInitInfo primConfVmbx;
                        primConfVmbx.iAlphaStringlength = iPBStoreInfoData->iVMBXTextLengthMax;
                        primConfVmbx.iExtension = EFalse;
                        primConfVmbx.iNumlength = iPBStoreInfoData->iVMBXNumberLengthMax;
                        primConfVmbx.iNoOfRecords = iPBStoreInfoData->iVMBXNumOfEntries;

                        iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeVMBX] = primConfVmbx;
                                
                        iTypeOfReading = EBasicEfRead;
                        iIniPhase = EPBIniPhase_PBInitialized;
                        }
                    else
                        {
                        iTypeOfReading = EExtensionRead;
                        }
                    iServiceType = UICC_APPL_FILE_INFO;
                    }
                else
                    {
                    // Reset all the parameters if init incomplete
                    iPBStoreInfoData->iVMBXNumOfEntries = -1;
                    iPBStoreInfoData->iVMBXNumberLengthMax = -1;
                    iPBStoreInfoData->iVMBXTextLengthMax = -1;
                    }
                }
                break;
            default:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleFileResp - PhoneBook not supported");
                OstTrace0( TRACE_NORMAL, DUP12_CMMPHONEBOOKOPERATIONINIT_HANDLEFILERESP, "CMmPhoneBookOperationInit::HandleFileResp - PhoneBook not supported" );
                }
                break;
            }

    if( ( UICC_STATUS_OK != aStatus ) && ( KErrNone == ret ) )
        {
        iServiceType = UICC_APPL_FILE_INFO;
        GetNextPhoneBookInitPhase(iIniPhase);
        }
    return ret;

    }



// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::HandleEXTFileResp
// Further handles response for Phonebook EXT Files
// -----------------------------------------------------------------------------
//


TInt CMmPhoneBookOperationInit::HandleEXTFileResp( const TDesC8 &aFileData , TInt aStatus)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp" );


    TInt ret(KErrNone);
    if(UICC_STATUS_OK == aStatus)
        {
        // no of records in EXT2 EF
        TInt noOfRecords(0);
        // get the no of records
        TFci fci( aFileData );
        noOfRecords = fci.GetNumberOfRecords();

        // To get the total length of number
        // Where multiply by 11 is the no of bytes in extension data and multiply by 2 is for BCD coding while
        //storing data in EXT2 file
        TInt numLength =  UICC_NO_EXT_MAX_NUM_LEN + (11*noOfRecords*2);

        // Change Extension present to read PhoneBook EF file info
        iTypeOfReading = EBasicEfRead;
        iServiceType = UICC_APPL_FILE_INFO;

        switch(iIniPhase)
            {
            case EPBInitPhaseADN:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp - For ADN Phone book");
                OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp - For ADN Phone book" );

                iPBStoreInfoData->iADNNumberLengthMax = numLength;
                
                // Store ADN Phone book configuration in local array if EXT is present
                TPrimitiveInitInfo primConfAdn;
                primConfAdn.iAlphaStringlength = iPBStoreInfoData->iADNTextLengthMax;
                primConfAdn.iExtension = ETrue;
                primConfAdn.iNumlength = iPBStoreInfoData->iADNNumberLengthMax;
                primConfAdn.iNoOfRecords = iPBStoreInfoData->iADNNumOfEntries;
                primConfAdn.iExtNoOfRec = noOfRecords;
                
                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeAdn] = primConfAdn;

                // Change the Initialization Phase to next
                // Get next available phonebook in UST table
                iIniPhase =  GetNextAvailablePbIcc(UICC_FDN_SERVICE_NUM);
                }
                break;
            case EPBInitPhaseFDN:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp - For FDN PhoneBook");
                OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp  -For FDN PhoneBoo" );

                iPBStoreInfoData->iFDNNumberLengthMax = numLength;

                // Store FDN Phone book configuration in local array if EXT is present
                TPrimitiveInitInfo primConfFdn;
                primConfFdn.iAlphaStringlength = iPBStoreInfoData->iFDNTextLengthMax;
                primConfFdn.iExtension = ETrue;
                primConfFdn.iNumlength = iPBStoreInfoData->iFDNNumberLengthMax;
                primConfFdn.iNoOfRecords = iPBStoreInfoData->iFDNNumOfEntries;
                primConfFdn.iExtNoOfRec = noOfRecords;
                
                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeFdn] = primConfFdn;

                if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                    {
                    iIniPhase = GetNextAvailablePbIcc(ICC_FDN_SERVICE_NUM);
                    }
                else if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                    {
                    iIniPhase = GetNextAvailablePbUicc(UICC_FDN_SERVICE_NUM);
                    }
                }
                break;
            case EPBInitPhaseSDN:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp - For SDN PhoneBook");
                OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp - For SDN PhoneBook" );

                iPBStoreInfoData->iSDNNumberLengthMax = numLength;

                // Store SDN Phone book configuration in local array if EXT is present
                TPrimitiveInitInfo primConfSdn;
                primConfSdn.iAlphaStringlength = iPBStoreInfoData->iSDNTextLengthMax;
                primConfSdn.iExtension = ETrue;
                primConfSdn.iNumlength = iPBStoreInfoData->iSDNNumberLengthMax;
                primConfSdn.iNoOfRecords = iPBStoreInfoData->iSDNNumOfEntries;
                primConfSdn.iExtNoOfRec = noOfRecords;
                
                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeSdn] = primConfSdn;

                if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                    {
                    iIniPhase = GetNextAvailablePbIcc(ICC_SDN_SERVICE_NUM);
                    }
                else if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                {
                iIniPhase = GetNextAvailablePbUicc(UICC_SDN_SERVICE_NUM);
                }

                }
                break;
            case EPBInitPhaseMBDN:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp - for MBDN PhoneBook");
                OstTrace0( TRACE_NORMAL, DUP4_CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp - for MBDN PhoneBook" );

                iPBStoreInfoData->iMBDNNumberLengthMax = numLength;

                // Store MBDN Phone book configuration in local array if EXT is present
                TPrimitiveInitInfo primConfMbdn;
                primConfMbdn.iAlphaStringlength = iPBStoreInfoData->iMBDNTextLengthMax;
                primConfMbdn.iExtension = ETrue;
                primConfMbdn.iNumlength = iPBStoreInfoData->iMBDNNumberLengthMax;
                primConfMbdn.iNoOfRecords = iPBStoreInfoData->iMBDNNumOfEntries;
                primConfMbdn.iExtNoOfRec = noOfRecords;
                primConfMbdn.iMbiRecLen = iMbiRecLen;
                
                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeMBDN] = primConfMbdn;

                if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                    {
                    iIniPhase = GetNextAvailablePbIcc(ICC_MBDN_SERVICE_NUM);
                    }
                else if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                    {
                    iIniPhase = GetNextAvailablePbUicc(UICC_MBDN_SERVICE_NUM);
                    }
                }
                break;
            case EPBInitPhaseMSISDN:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp - for MSISDN Phonebook");
                OstTrace0( TRACE_NORMAL, DUP5_CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp - for MSISDN Phonebook" );

                iPBStoreInfoData->iMSISDNNumberLengthMax = numLength;
                
                // Store MSISDN Phone book configuration in local array if EXT is present
                TPrimitiveInitInfo primConfMsisdn;
                primConfMsisdn.iAlphaStringlength = iPBStoreInfoData->iMSISDNTextLengthMax;
                primConfMsisdn.iExtension = ETrue;
                primConfMsisdn.iNumlength = iPBStoreInfoData->iMSISDNNumberLengthMax;
                primConfMsisdn.iNoOfRecords = iPBStoreInfoData->iMSISDNNumOfEntries;
                primConfMsisdn.iExtNoOfRec = noOfRecords;
                
                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeMSISDN] = primConfMsisdn;


                if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
                    {
                    iIniPhase = GetNextAvailablePbIcc(ICC_MSISDN_SERVICE_NUM);
                    }
                else if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
                    {
                    iIniPhase = GetNextAvailablePbUicc(UICC_MSISDN_SERVICE_NUM);
                    }
                }
                break;
            case EPBInitPhaseVMBX:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp  - For VMBX PhoneBook");
                OstTrace0( TRACE_NORMAL, DUP6_CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp - For VMBX PhoneBook" );

                iPBStoreInfoData->iVMBXNumberLengthMax = numLength;
                
                // Store VMBX Phone book configuration in local array if EXT is present
                TPrimitiveInitInfo primConfVmbx;
                primConfVmbx.iAlphaStringlength = iPBStoreInfoData->iVMBXTextLengthMax;
                primConfVmbx.iExtension = ETrue;
                primConfVmbx.iNumlength = iPBStoreInfoData->iVMBXNumberLengthMax;
                primConfVmbx.iNoOfRecords = iPBStoreInfoData->iVMBXNumOfEntries;
                primConfVmbx.iExtNoOfRec = noOfRecords;
                
                iMmPhoneBookStoreMessHandler->iPBStoreConf[EPhonebookTypeVMBX] = primConfVmbx;

                
                iIniPhase = EPBIniPhase_PBInitialized;
                }
                break;
            default:
                {
                TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleEXTFileResp  - PhoneBook not supported");
                OstTrace0( TRACE_NORMAL, DUP7_CMMPHONEBOOKOPERATIONINIT_HANDLEEXTFILERESP, "CMmPhoneBookOperationInit::HandleEXTFileResp - PhoneBook not supported" );
                }
                break;
            }

        }
    else
        {
        iServiceType = UICC_APPL_FILE_INFO;
        GetNextPhoneBookInitPhase(iIniPhase);
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::HandleMBIFileResp
// Handle MBI File response
// by index
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit::HandleMBIFileResp( const TDesC8 &aFileData , TInt aStatus )
    {
    
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::HandleMBIFileResp");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_HANDLEMBIFILERESP, "CMmPhoneBookOperationInit::HandleMBIFileResp" );
    
    TInt ret(KErrNone);
    if(UICC_STATUS_OK == aStatus)
        {
        // get the no of records
        TFci fci( aFileData );
        iMbiRecLen = fci.GetRecordLength();
        iTypeOfReading = EBasicEfRead;
        }
    return ret ;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::GetNextAvailablePbUicc
// Gets next phonebook to be initialized in SST Servicetable
// by index
// -----------------------------------------------------------------------------
//

TUint8 CMmPhoneBookOperationInit::GetNextAvailablePbIcc(TUint8 aPBook)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::GetNextAvailablePbIcc");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT_GETNEXTAVAILABLEPBICC, "CMmPhoneBookOperationInit::GetNextAvailablePbIcc" );

    switch(aPBook)
        {
        case ICC_ADN_SERVICE_NUM:
            {
            if(iMmUiccMessHandler->GetServiceStatus( ICC_FDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseFDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus( ICC_SDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseSDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus( ICC_MBDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMBDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus( ICC_MSISDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMSISDN;
                }
            else if( iMmUiccMessHandler->GetCphsInformationStatus(ICC_MAILBOX_NUM)) // Service no need to be changed
                {
                aPBook = EPBInitPhaseVMBX;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
                }
               // aPBook = EPBInitPhaseVMBX;
            }
            break;
        case ICC_FDN_SERVICE_NUM:
            {
            if(iMmUiccMessHandler->GetServiceStatus( ICC_SDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseSDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus( ICC_MBDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMBDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus( ICC_MSISDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMSISDN;
                }
            else if(iMmUiccMessHandler->GetCphsInformationStatus( ICC_MAILBOX_NUM ))  // Service no need to changed
                {
                aPBook = EPBInitPhaseVMBX;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
               // aPBook = EPBInitPhaseVMBX;
                }
            }
            break;
        case ICC_SDN_SERVICE_NUM:
            {
            if(iMmUiccMessHandler->GetServiceStatus(ICC_MBDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMBDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus(ICC_MSISDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMSISDN;
                }
            else if( iMmUiccMessHandler->GetCphsInformationStatus( ICC_MAILBOX_NUM ))
                {
                aPBook = EPBInitPhaseVMBX;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
                //aPBook = EPBInitPhaseVMBX;
                }
            }
            break;
        case ICC_MBDN_SERVICE_NUM:
            {
            if(iMmUiccMessHandler->GetServiceStatus( ICC_MSISDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMSISDN;
                }
            else if( iMmUiccMessHandler->GetCphsInformationStatus( ICC_MAILBOX_NUM ))
                {
                aPBook = EPBInitPhaseVMBX;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
                //aPBook = EPBInitPhaseVMBX;
                }
            }
            break;
        case ICC_MSISDN_SERVICE_NUM:
            // Check for CPHS flas to get the VMBX Phonebook configuration other wise complete the phonebook
            // init for 2G card
            if( iMmUiccMessHandler->GetCphsInformationStatus( ICC_MAILBOX_NUM ))
                {
                aPBook = EPBInitPhaseVMBX;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
                }
            break;
        default:
            {
            TFLOGSTRING("TSY: CMmPhoneBookOperationInit::GetNextAvailablePbIcc - Not Supported");
            OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT_GETNEXTAVAILABLEPBICC, "CMmPhoneBookOperationInit::GetNextAvailablePbIcc - Not Supported" );
            }
            break;
        }
    
    if( EPBInitPhaseMBDN == aPBook )
        {
        iTypeOfReading = EMailboxIdRead;
        }
    return aPBook;
    }





// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::GetNextAvailablePbUicc
// Gets next phonebook to be initiilized from UST service table
// -----------------------------------------------------------------------------
//

TUint8 CMmPhoneBookOperationInit::GetNextAvailablePbUicc(TUint8 aPBook)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit::GetNextAvailablePbUicc");
    OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT_GETNEXTAVAILABLEPBUICC, "CMmPhoneBookOperationInit::GetNextAvailablePbUicc" );

    switch(aPBook)
        {
        case UICC_FDN_SERVICE_NUM:
            {
            if(iMmUiccMessHandler->GetServiceStatus( UICC_SDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseSDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus( UICC_MBDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMBDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus( UICC_MSISDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMSISDN;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
                }
            }
            break;
        case UICC_SDN_SERVICE_NUM:
            {
            if(iMmUiccMessHandler->GetServiceStatus(UICC_MBDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMBDN;
                }
            else if(iMmUiccMessHandler->GetServiceStatus(UICC_MSISDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMSISDN;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
                }
            }
            break;
        case UICC_MBDN_SERVICE_NUM:
            {
            if(iMmUiccMessHandler->GetServiceStatus( UICC_MSISDN_SERVICE_NUM))
                {
                aPBook = EPBInitPhaseMSISDN;
                }
            else
                {
                aPBook = EPBIniPhase_PBInitialized;
                }
            }
            break;
        case UICC_MSISDN_SERVICE_NUM:
            aPBook = EPBIniPhase_PBInitialized;
            break;
        default:
            break;
        }
    
    if( EPBInitPhaseMBDN == aPBook )
        {
        iTypeOfReading = EMailboxIdRead;
        }

    return aPBook;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit3G_adn::GetNextPhoneBookInitPhase
// Get next Phonebook init phase if Init for one Phonebook fails
// -----------------------------------------------------------------------------
//

void CMmPhoneBookOperationInit::GetNextPhoneBookInitPhase(TUint8& aInitPhase)
    {
    if(UICC_CARD_TYPE_ICC == iMmUiccMessHandler->GetCardType())
        {
        switch(aInitPhase)
            {
            case EPBInitPhaseADN:
                {
                aInitPhase = GetNextAvailablePbIcc(ICC_ADN_SERVICE_NUM);
                }
                break;
            case EPBInitPhaseFDN:
                {
                aInitPhase = GetNextAvailablePbIcc(ICC_FDN_SERVICE_NUM);
                }
                break;
            case EPBInitPhaseSDN:
                {
                aInitPhase = GetNextAvailablePbIcc(ICC_SDN_SERVICE_NUM);
                }
                break;
            case EPBInitPhaseMBDN:
                {
                aInitPhase = GetNextAvailablePbIcc(ICC_MBDN_SERVICE_NUM);
                }
                break;
            case EPBInitPhaseMSISDN:
                {
                aInitPhase = GetNextAvailablePbIcc(ICC_MSISDN_SERVICE_NUM);
                }
                break;
            default:
                {
                aInitPhase = EPBIniPhase_PBInitialized;
                }
                break;
            }
        }
    else
        {
        if(UICC_CARD_TYPE_UICC == iMmUiccMessHandler->GetCardType())
            {
            switch(aInitPhase)
                {
                case EPBInitPhase_3GADN_PBR:
                    {
                    iServiceType = UICC_APPL_FILE_INFO;
                    aInitPhase = EPBIniPhase_3GADNDone;
                    }
                    break;
                case EPBInitPhase_3GADN_Type1:
                case EPBInitPhase_3GADN_Type2:
                case EPBInitPhase_3GADN_Type3:
                    {
                    iServiceType = UICC_APPL_FILE_INFO;
                    }
                    break;
                case EPBInitPhaseFDN:
                    {
                    aInitPhase = GetNextAvailablePbUicc(UICC_FDN_SERVICE_NUM);
                    }
                    break;
                case EPBInitPhaseSDN:
                    {
                    aInitPhase = GetNextAvailablePbUicc(UICC_SDN_SERVICE_NUM);
                    }
                    break;
                case EPBInitPhaseMBDN:
                    {
                    aInitPhase = GetNextAvailablePbUicc(UICC_MBDN_SERVICE_NUM);
                    }
                    break;
                case EPBInitPhaseMSISDN:
                    {
                    aInitPhase = GetNextAvailablePbUicc(UICC_MSISDN_SERVICE_NUM);
                    }
                    break;
                default:
                    {
                    aInitPhase = EPBIniPhase_PBInitialized;
                    }
                    break;
                }
            }
       }
    }

// End of file

