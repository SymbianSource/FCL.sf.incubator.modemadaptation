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
* Description:   ?Description
*
*/

// INCLUDE FILES
#include <etelmm.h>
#include <tisi.h>
#include <pn_const.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmessagerouter.h"
#include "cmmphonebookoperationinit3G_adn.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationinitTraces.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationinit3g_adnTraces.h"
#endif

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

CMmPhoneBookOperationInit3G_adn::CMmPhoneBookOperationInit3G_adn
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::CMmPhoneBookOperationInit3G_adn");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_CMMPHONEBOOKOPERATIONINIT3G_ADN, "CMmPhoneBookOperationInit3G_adn::CMmPhoneBookOperationInit3G_adn" );

    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::~CMmPhoneBookOperationInit
// C++ destructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationInit3G_adn::~CMmPhoneBookOperationInit3G_adn
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::CMmPhoneBookOperationInit3G_adn");
    OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT3G_ADN_CMMPHONEBOOKOPERATIONINIT3G_ADN, "CMmPhoneBookOperationInit3G_adn::~CMmPhoneBookOperationInit3G_adn" );

    }

// -----------------------------------------------------------------------------
// CmmPhonebookOperatorInit::NewL
// Two-phased constructor.
// Creates a new CmmPhonebookOperatorInit object instance.
// -----------------------------------------------------------------------------

CMmPhoneBookOperationInit3G_adn* CMmPhoneBookOperationInit3G_adn::NewL
    (
    CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
    CMmUiccMessHandler* aUiccMessHandler,
    const CMmDataPackage* aDataPackage // Data
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::NewL");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_NEWL, "CMmPhoneBookOperationInit3G_adn::NewL" );


    TName phonebookTypeName;

    CMmPhoneBookOperationInit3G_adn* mmPhoneBookOperationInit3G_adn =
        new( ELeave ) CMmPhoneBookOperationInit3G_adn();

    CleanupStack::PushL( mmPhoneBookOperationInit3G_adn );

    const CPhoneBookDataPackage* phoneBookData =
        static_cast<const CPhoneBookDataPackage*>( aDataPackage );

    phoneBookData->GetPhoneBookName( phonebookTypeName );
    // Store phonebook name
    mmPhoneBookOperationInit3G_adn->iPhoneBookTypeName = phonebookTypeName;

    if( 0 == phonebookTypeName.CompareF( KInternalPhoneBookType ) )
        {
        mmPhoneBookOperationInit3G_adn->iIniPhase = EPBIniPhase_Internal;
        }
    else // Full construction is not needed in case of internal initilization
        {
        mmPhoneBookOperationInit3G_adn->ConstructL();
        }
    mmPhoneBookOperationInit3G_adn->iMmPhoneBookStoreMessHandler =
        aMmPhoneBookStoreMessHandler;

    mmPhoneBookOperationInit3G_adn->iMmUiccMessHandler = aUiccMessHandler;

    // set to false by default. Will be set as true once after
    // initialization req has been generated.
    mmPhoneBookOperationInit3G_adn->iInternalInit = EFalse;


    CleanupStack::Pop( mmPhoneBookOperationInit3G_adn );

    return mmPhoneBookOperationInit3G_adn;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationInit3G_adn::ConstructL
    (
    // None
    )
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::ConstructL" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_CONSTRUCTL, "CMmPhoneBookOperationInit3G_adn::ConstructL" );

    iPBStoreInfoData = new( ELeave ) CStorageInfoData();

    iIniPhase = 0;
    iServiceType = 0;
    iNumOfPBRRecords = 0;



#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler. \
        INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS is ON.");

    iMmPhoneBookStoreMessHandler->SetMaxNumOfAasEntries( 0 );
    iMmPhoneBookStoreMessHandler->SetMaxNumberOfGAS( 0 );
#else
    TFLOGSTRING("TSY: CMmPhoneBookStoreMessHandler. \
        INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS is OFF.");
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS


    }



// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit3G_adn::UICCCreateReq
// For USIM Separate different request to correct function
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit3G_adn::UICCCreateReq
    (
    TInt aIpc,
    const CMmDataPackage* /*aDataPackage*/,
    TUint8 aTransId
    )
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::UICCCreateReq" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_UICCCREATEREQ, "CMmPhoneBookOperationInit3G_adn::UICCCreateReq" );

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

                // Start Initialization for ADN phonebook 3G
                iIniPhase = EPBInitPhase_3GADN_PBR;
                ret = UICCInitializeReq3GADN( aTransId );
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
            TFLOGSTRING2( "TSY: CMmPhoneBookOperationInit3G_adn::UICCCreateReq - \
            Unknown IPC: %d", aIpc );
            OstTrace1( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT3G_ADN_UICCCREATEREQ, "CMmPhoneBookOperationInit3G_adn::UICCCreateReq;Unknown IPC =%d", aIpc );

            break;
            }
        } // switch-case ends
    return ret;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationInit3G_adn::UICCCreateReq
// For USIM Separate different request to correct function
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit3G_adn::UICCInitializeReq3GADN( TUint8 aTransId)
    {
    TInt ret(KErrNone);

    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::UICCInitializeReq3GADN" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_UICCINITIALIZEREQ3GADN, "CMmPhoneBookOperationInit3G_adn::UICCInitializeReq3GADN" );

        TInt appFileID( APPL_FILE_ID );
        TUiccReadLinearFixed cmdParams;
        cmdParams.messHandlerPtr =
            static_cast<MUiccOperationBase*>( iMmPhoneBookStoreMessHandler );
        cmdParams.trId = static_cast<TUiccTrId>( aTransId );

        cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE >> 8 ));
        cmdParams.filePath.Append( static_cast<TUint8>( MF_FILE ));
        cmdParams.filePath.Append( appFileID>>8 );
        cmdParams.filePath.Append( appFileID );
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
        cmdParams.filePath.Append( static_cast<TUint8>( DF_PHONEBOOK ));

    switch( iIniPhase )
        {
        case EPBInitPhase_3GADN_PBR:
            {
            // Send Request to UICC Server to Read file Info of PBR file
            // No. of records in PBR file
            TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::UICCInitializeReq3GADN - \
            File Info Read Request for EF-PBR " );
            OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT3G_ADN_UICCINITIALIZEREQ3GADN, "CMmPhoneBookOperationInit3G_adn::UICCInitializeReq3GADN - File Info Read Request for EF-PBR" );

            cmdParams.fileId = PB_PBR_FID;
            cmdParams.serviceType = iServiceType;
            cmdParams.record = 0;
            break;
            }
        case EPBInitPhase_3GADN_Type1:
            {
            // read Type 1 Files File descriptor, File Data and Extension File data
            ret = CreateReqFetchTypeFile( cmdParams, UICC_TYPE1_FILE );
            break;
            }
        case EPBInitPhase_3GADN_Type2:
            {
            // read Type 2 Files File descriptor, File Data and Extension File data
            ret = CreateReqFetchTypeFile( cmdParams, UICC_TYPE2_FILE );
            break;
            }
        case EPBInitPhase_3GADN_Type3:
            {
            // read Type 3 Files File descriptor, File Data and Extension File data
            ret = CreateReqFetchTypeFile( cmdParams, UICC_TYPE3_FILE );
            break;
            }
        default:
            {
            break;
            }
        }

    // if intialization for USIM ADN phonebook has not been completed till now then send the
    // request for the correct phase

    if( EPBIniPhase_3GADNDone != iIniPhase )
        {
        ret = iMmPhoneBookStoreMessHandler->UiccMessHandler()->
            CreateUiccApplCmdReq( cmdParams );
        TFLOGSTRING2( "TSY: CreateUiccApplCmdReq returns %d", ret );
        }

    return ret;
    }



// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::CreateReqFetchTypeFile
// Creates data to fetch EF of type 1, Type2 and Type3 in PBR file TLV for File
//Info or File Data
// -----------------------------------------------------------------------------
//


TInt CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile(
        TUiccReadLinearFixed & aparams,
        const TUint8 aFileType)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_CREATEREQFETCHTYPEFILE, "CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile" );


    TInt ret( KErrNone );
    TUint16 fileFID ( 0 );
    TUint8 fileSFI ( 0 );

    switch( aFileType )
        {
        case UICC_TYPE1_FILE:
                {
                TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile - Create req for Type 1 File " );
                OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT3G_ADN_CREATEREQFETCHTYPEFILE, "CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile - Create req for Type 1 File  " );

                fileFID = iType1FileArray[iCurrentArrayCounter].tagFID;
                fileSFI = iType1FileArray[iCurrentArrayCounter].tagSFI;
                }
            break;
        case UICC_TYPE2_FILE:
            {
                TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile - Create req for Type 2 File  " );
                OstTrace0( TRACE_NORMAL, DUP3_CMMPHONEBOOKOPERATIONINIT3G_ADN_CREATEREQFETCHTYPEFILE, "CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile - Create req for Type 2 File " );

                fileFID = iType2FileArray[iCurrentArrayCounter].tagFID;
                fileSFI = iType2FileArray[iCurrentArrayCounter].tagSFI;
                }
            break;
        case UICC_TYPE3_FILE:
                {
                TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile - Create req for Type 3 File  " );
                OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONINIT3G_ADN_CREATEREQFETCHTYPEFILE, "CMmPhoneBookOperationInit3G_adn::CreateReqFetchTypeFile - Create req for Type 3 File " );

                fileFID = iType3FileArray[iCurrentArrayCounter].tagFID;
                fileSFI = iType3FileArray[iCurrentArrayCounter].tagSFI;
                }
            break;
        default:
            {
            break;
            }
        }

    // FCP req for EFadn
    aparams.fileId = fileFID;
    aparams.fileIdSfi = fileSFI;
    aparams.serviceType = iServiceType;
    aparams.record = 0;

    return ret;
    }


// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::HandleUICCPbRespL
// HAndle response for all phase init response in USIM
// -----------------------------------------------------------------------------
//

TBool CMmPhoneBookOperationInit3G_adn::HandleUICCPbRespL
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
        TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::HandleUICCPbRespL. Internal Init->Break");
        OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONINIT3G_ADN_HANDLEUICCPBRESPL, "CMmPhoneBookOperationInit3G_adn::HandleUICCPbRespL. Internal Init-> Break" );


        iIniPhase = EPBIniPhase_Unknown;

        iInternalInit = EFalse;
        complete = ETrue;
        return KErrNone;
        }

        switch(iIniPhase)
            {
            case EPBInitPhase_3GADN_PBR:
            case EPBInitPhase_3GADN_Type1:
            case EPBInitPhase_3GADN_Type2:
            case EPBInitPhase_3GADN_Type3:
            {
            TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::HandleUICCPbRespL. USIM ADN PhoneBook Init" );
            OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_HANDLEUICCPBRESPL, "CMmPhoneBookOperationInit3G_adn::HandleUICCPbRespL. USIM ADN Phonebook init" );

            ret = HandleUICCPbResp3GADN( aFileData, aStatus, aTransId );
            }
            break;
            case EPBInitPhaseFDN:
            case EPBInitPhaseSDN:
            case EPBInitPhaseMBDN:
            case EPBInitPhaseMSISDN:
            {
            TFLOGSTRING( "TSY: CMmPhoneBookOperationInit::HandleUICCPbRespL. Other PhoneBook Init" );
            OstTrace0( TRACE_NORMAL, DUP2_CMMPHONEBOOKOPERATIONINIT3G_ADN_HANDLEUICCPBRESPL, "CMmPhoneBookOperationInit3G_adn::HandleUICCPbRespL. Other Phonebook Init" );

            // For all other phoenbook handling is same as 2g Phonebook
            ret = HandlePBRespL( aFileData, aStatus, aTransId );
            }
            break;
            default:
                // Not to handle anything if card type is not available
                break;

            }

         // Complete, if phonebook initalization is complete or there is some error in UICC server
         if ( KErrNone != ret || EPBIniPhase_PBInitialized == iIniPhase )
             {
             CPhoneBookDataPackage phoneBookData;
             phoneBookData.SetPhoneBookName( iPhoneBookTypeName );
             phoneBookData.PackData( iPBStoreInfoData );

             if ( UICC_STATUS_OK != aStatus )
                 {
                 ret = CMmStaticUtility::UICCCSCauseToEpocError( aStatus );
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
// CmmPhonebookOperationInit::HandleUICCPbResp3GADN
// Handle response for ADN phone book init in USIM
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit3G_adn::HandleUICCPbResp3GADN
    (
    const TDesC8& aFileData,
    TInt aStatus,
    TUint8 aTransId )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::HandleUICCPbResp3GADN");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_HANDLEUICCPBRESP3GADN, "CMmPhoneBookOperationInit3G_adn::HandleUICCPbResp3GADN" );

    TInt ret( KErrNone );

        switch(iIniPhase)
            {
            case EPBInitPhase_3GADN_PBR:
                {
                if(UICC_STATUS_OK == aStatus)
                    {
                    TInt offSet( 0 );
                    // For File Info , get the no of records in PBR file to get the total no of entries in 3G ADN phonebook
                    if( UICC_APPL_FILE_INFO == iServiceType )
                        {
                        TFci fci( aFileData );
                        iNumOfPBRRecords = fci.GetNumberOfRecords();

                        ret = KErrNone;
                        iServiceType = UICC_APPL_READ_LINEAR_FIXED;
                        }
                    else if( UICC_APPL_READ_LINEAR_FIXED == iServiceType )
                        {
                        // Get the offset for  Type 1 file tag "KTagConstructedTagA8"
                        offSet = aFileData.Find( &KTagConstructedTagA8, 1 );
                        if( offSet != KErrNotFound )
                            {
                            // Fetch Type 1 files and store fileId , File tag, and SFI value in iType1FileArray
                            ret = FetchType1FileFromPBR( aFileData );
                            }

                        // Get the offset for type 2 Files
                        offSet = aFileData.Find( &KTagConstructedTagA9, 1 );
                        if( offSet != KErrNotFound )
                            {
                            // Fetch Type2 files and store fileId , File tag, and SFI value in iType2FileArray
                            ret = FetchType2FileFromPBR( aFileData );
                            }

                        // Get the offset for type 3 Files
                        offSet = aFileData.Find( &KTagConstructedTagAA, 1 );
                        if( offSet != KErrNotFound )
                            {
                            // Fetch Type 3 files and store fileId , File tag, and SFI value in iType3FileArray
                            ret = FetchType3FileFromPBR( aFileData );
                            }

                        // Consistency check for File ids and if there is same file IDs then remove those File Ids
                        ret = CheckForFileId();

                        // Consistency Check for Type 1 file should not be type 2 or type3
                        ret = CheckForSameType1AndType2Files();

                        iServiceType = UICC_APPL_FILE_INFO;
                        iIniPhase = EPBInitPhase_3GADN_Type1;
                        }
                    }
                else
                    {
                    iADNPbInitilized = EFalse;
                    GetNextPhoneBookInitPhase(iIniPhase);
                    }
                }
                break;
            case EPBInitPhase_3GADN_Type1:
                {
                HandleType1FileResp( aFileData, aStatus );
                }
                break;
            case EPBInitPhase_3GADN_Type2:
                {
                HandleType2FileResp( aFileData, aStatus );
                }
                break;
            case EPBInitPhase_3GADN_Type3:
                {
                HandleType3FileResp( aFileData, aStatus );
                }
                break;
            default:
                break;
            }

    if(KErrNone == ret)
        {
        if( iIniPhase == EPBIniPhase_3GADNDone )
            {
            // Call Create requuest for otyher phone books
            // Check for FDN enabled or disabled
            if( iMmUiccMessHandler->GetServiceStatus(UICC_FDN_SERVICE_NUM) )
                iIniPhase = EPBInitPhaseFDN;
            else
                {
                // get the next Phone book available in UST OR SST Table (for 2G)table
                iIniPhase = GetNextAvailablePbUicc( UICC_FDN_SERVICE_NUM );
                }
            iServiceType = UICC_APPL_FILE_INFO;
            ret = UICCInitializeReq( aTransId );
            }
        else
            {
            // Call Create request for next phase
            ret = UICCInitializeReq3GADN( aTransId );
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::FetchType1FileFromPBR
// Fetch Type 1 File ID data and store that data in iType1FileArray
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit3G_adn::FetchType1FileFromPBR( const TDesC8 &aFileData )
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::FetchType1FileFromPBR" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_FETCHTYPE1FILEFROMPBR, "CMmPhoneBookOperationInit3G_adn::FetchType1FileFromPBR" );

    TInt ret( KErrNone );
    TPrimitiveTag primTag;

    // Get the type 1 file Tag offset
    TInt offset = aFileData.Find( &KTagConstructedTagA8, 1 );

    // Get the Type 1 File Tag Length
    TUint8 aTagLength = aFileData[offset + 1];

    // Offset+2 to get offset for first File tag
    offset = offset+2;

    // Store Type 1 file data in iType1FileArray array
    for( TInt i = 0; i < aTagLength; )
        {
        // Get the Tag name
        primTag.tagValue = aFileData[offset + i];

        // get the File ID which is 2byte long
        primTag.tagFID = CMmStaticUtility::Get16Bit( aFileData, ( offset+i+2 ));

        // if file Tag length is 3 the SFI is available and if file tag length is 2 the only File ID is present
        if(KLengthWithSFI == aFileData[offset + i+1])
            {
            primTag.tagSFI = aFileData[offset+i+4];
            // get the offset for next file tag
            i = i+5;
            }
        else if(KLengthWithOutSFI == aFileData[offset+i+1])
            {
            primTag.tagSFI = 0;
            // get the offset for next file tag
            i = i+4;
            }
        else
            {
            ret = KErrGeneral;
            break;
            }
        iType1FileArray.Append( primTag );
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::FetchType2FileFromPBR
// Fetch Type 2 File ID data and store that data in iType2FileArray
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit3G_adn::FetchType2FileFromPBR(const TDesC8 &aFileData)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::FetchType2FileFromPBR");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_FETCHTYPE2FILEFROMPBR, "CMmPhoneBookOperationInit3G_adn::FetchType2FileFromPBR" );


    TInt ret( KErrNone );
    TPrimitiveTag primTag;

    // Get the type 1 file Tag offset
    TInt offset = aFileData.Find( &KTagConstructedTagA9,1 );
    // Get the Type 1 File Tag Length
    TUint8 aTagLength = aFileData[offset+1];

    offset = offset + 2;

    // Store Type 1 file data in iType1FileArray array
    for( TInt i = 0; i < aTagLength; )
        {
        // Get the Tag name
        primTag.tagValue = aFileData[offset+i];
        // get the File ID which is 2byte long
        primTag.tagFID = CMmStaticUtility::Get16Bit( aFileData, ( offset+i+2 ));

        // if file Tag length is 3 the SFI is available and if file tag length is 2 the only File ID is present
        if(KLengthWithSFI == aFileData[offset+i+1])
            {
            primTag.tagSFI = aFileData[offset+i+4];
            // get the offset for next file tag
            i = i+5;
            }
        else if(KLengthWithOutSFI == aFileData[offset+i+1])
            {
            primTag.tagSFI = 0;
            // get the offset for next file tag
            i = i+4;
            }
        else
            {
            ret = KErrGeneral;
            break;
            }
        iType2FileArray.Append( primTag );
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::FetchType3FileFromPBR
// Fetch Type 3 File ID data and store that data in iType3FileArray
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationInit3G_adn::FetchType3FileFromPBR(const TDesC8& aFileData)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::FetchType3FileFromPBR");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_FETCHTYPE3FILEFROMPBR, "CMmPhoneBookOperationInit3G_adn::FetchType3FileFromPBR" );

        TInt ret( KErrNone );
        TPrimitiveTag primTag;

        // Get the type 1 file Tag offset
        TInt offset = aFileData.Find( &KTagConstructedTagAA,1 );

        // Get the Type 1 File Tag Length
        TUint8 aTagLength = aFileData[offset+1];

        // To get the offset fot first File tag value
        offset = offset + 2;

        // Store Type 1 file data in iType1FileArray array
        for( TInt i=0; i < aTagLength; )
            {
            // Get the Tag name
            primTag.tagValue = aFileData[offset+i];

            // get the File ID which is 2byte long
            primTag.tagFID = CMmStaticUtility::Get16Bit( aFileData,
            	                                          ( offset + i + 2 ));

            // if file Tag length is 3 the SFI is available and if file tag length is 2 the only File ID is present
            if(KLengthWithSFI == aFileData[offset+i+1])
                {
                primTag.tagSFI = aFileData[offset+i+4];
                i = i+5;
                }
            else if(KLengthWithOutSFI == aFileData[offset+i+1])
                {
                primTag.tagSFI = 0;
                i = i+4;
                }
            else
                {
                ret = KErrGeneral;
                break;
                }
            iType3FileArray.Append( primTag );
            }
        return ret;

    }



// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::HandleType1FileResp
// HAndles Data for type 1 File response
// -----------------------------------------------------------------------------
//


void CMmPhoneBookOperationInit3G_adn::HandleType1FileResp( const TDesC8 &aFileData, TInt aStatus )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::HandleType1FileResp");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_HANDLETYPE1FILERESP, "CMmPhoneBookOperationInit3G_adn::HandleType1FileResp" );

    TInt ret( KErrNone );
    TInt recordLength( 0 );
    TInt noOfRecords(0);

    // Get the Tag Value from Type1 File Array
    TUint8 tagValue = iType1FileArray[iCurrentArrayCounter].tagValue;

    if( UICC_STATUS_OK  == aStatus )
        {
        // Get the Record length for all linear fixed and circular EF's
        TFci fci( aFileData );
        recordLength = fci.GetRecordLength();
        noOfRecords = fci.GetNumberOfRecords();
			
        // Check for No of records in Type 1 file
        if( UICC_EF_ADN_TAG != tagValue )
            {
            ret = CheckForNoOfRecords( noOfRecords );
            }
        }


    if( iCurrentArrayCounter < iType1FileArray.Count() )
        {
        if( UICC_STATUS_OK  == aStatus )
            {
            switch( tagValue )
                {
                case UICC_EF_ADN_TAG:
                    {
                    // get total no of entries in all PBR records
                    iPBStoreInfoData->iADNNumOfEntries = noOfRecords* iNumOfPBRRecords;

                    iPBStoreInfoData->iADNTextLengthMax = ( recordLength - 14 );

                    // To get the total length of number, need to read Ext1 file
                    iPBStoreInfoData->iADNNumberLengthMax =  UICC_NO_EXT_MAX_NUM_LEN;
                    iADNPbInitilized = ETrue;
                    }
                    break;
                case UICC_EF_ANR_TAG:
                    {
                    // Check for if No of records for this File is same as ADN phonebook no of records
                    if(KErrNone == ret)
                        {
                        // Check for fixed size record length
                        ret = CheckRecordSizeForFixedLength( 1, UICC_EF_ANR_TAG,recordLength );
                        if( KErrNone == ret )
                            {
                            // total no of entries in all ANR files
                            iPBStoreInfoData->iANRNumOfEntries = ( noOfRecords* iNumOfPBRRecords );
                            // For this need to know about the ext files
                            iPBStoreInfoData->iANRNumLengthMax = UICC_NO_EXT_MAX_NUM_LEN;
                            // No of entries in every EF
                            iPBStoreInfoData->iANRNumOfEntriesPerEntry = noOfRecords;
                            // no ot ANR file per ADN phonebook
                            iType1ANRFiles++;
                            }
                        }
                    }
                    break;
                case UICC_EF_EMAIL_TAG:
                    {
                    if(KErrNone == ret)
                        {
                        // get the total no of records in Email EF
                        iPBStoreInfoData->iEmailNumOfEntries = ( noOfRecords* iNumOfPBRRecords );

                        // get record Length from filedata, In Type1 file case email text length will be same as record length
                        iPBStoreInfoData->iEmailTextLengthMax = recordLength;

                        iPBStoreInfoData->iEmailNumOfEntriesPerEntry = noOfRecords;

                        // no of type1 email files per ADN Phonebook
                        iType1EmailFiles++;
                        }
                    }
                    break;
                case UICC_EF_GRP_TAG:
                    {
                    if(KErrNone == ret)
                        {
                        // get the record record no from file info data
                        iPBStoreInfoData->iGRPNumOfEntriesPerEntry = noOfRecords;
                        }
                    }
                    break;
                case UICC_EF_IAP_TAG:
                    {
                    // This File is mandatory only when there are Type 2 Files
                    if( iType2FileArray.Count()== 0 )
                        ret = KErrGeneral;
                    }
                    break;
                case UICC_EF_PBC_TAG:
                    {
                    if( KErrNone == ret )
                        {
                        if( KErrNone != CheckRecordSizeForFixedLength(1, UICC_EF_PBC_TAG,recordLength) )
                            {
                            // remove this entry from Array
                            iType1FileArray[iCurrentArrayCounter].tagFID = 0x00;
                            iType1FileArray[iCurrentArrayCounter].tagSFI = 0x00;
                            iType1FileArray[iCurrentArrayCounter].tagValue = 0x00;
                            }
                        }
                    }
                    break;
                case UICC_EF_SNE_TAG:
                    {
                    if( KErrNone == ret )
                        {
                        iPBStoreInfoData->iSNENumOfEntries = noOfRecords * iNumOfPBRRecords;
                        iPBStoreInfoData->iSNENumOfEntriesPerEntry = noOfRecords;
                        iPBStoreInfoData->iSNETextLengthMax = recordLength;
                        // Get the no of Type 1 SNE Files per ADN phonebook
                        iType1SNEFiles++;
                        }
                    }
                    break;
                case UICC_EF_UID_TAG:
                    break;
                default:
                    break;
                }

            // Check if error is there then remove that file ID and tag from iType1FileArray
            if( KErrNone != ret )
                {
                iType1FileArray.Remove(iCurrentArrayCounter);
                }
            else
                {
                iCurrentArrayCounter++;
                }
            }
        else
            {
            // Check if the fail is for EF adn then phonebook is not valid
            if( UICC_EF_ADN_TAG == tagValue)
                {
                iADNPbInitilized = EFalse;
                // ADN phonebook is not valid and check for other phonebooks parameters
                iIniPhase = EPBIniPhase_3GADNDone;
                return;
                }
            iCurrentArrayCounter++;
            }


        // Check for next Phase
        if( iCurrentArrayCounter == iType1FileArray.Count() )
            {
            iCurrentArrayCounter = 0;

            // Change Phase to read other Files
            if( 0 != iType2FileArray.Count() )
                iIniPhase = EPBInitPhase_3GADN_Type2;

            if( 0 != iType3FileArray.Count() )
                iIniPhase = EPBInitPhase_3GADN_Type3;
            else
                iIniPhase = EPBIniPhase_3GADNDone;
            }
        } // End of Type 1 file array handling
    }


// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::HandleType2FileResp
// HAndles Data for Type 2 file Response
// -----------------------------------------------------------------------------
//


void CMmPhoneBookOperationInit3G_adn::HandleType2FileResp(const TDesC8 &aFileData, TInt aStatus)
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::HandleType2FileResp");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_HANDLETYPE2FILERESP, "CMmPhoneBookOperationInit3G_adn::HandleType2FileResp" );

    TInt ret(KErrNone);
    TInt recordLength( 0 );
    TInt noOfRecords(0);

    if( UICC_STATUS_OK  == aStatus )
        {
        // Get the Record length for all linear fixed and circular EF's
        TFci fci( aFileData );
        recordLength = fci.GetRecordLength();
        noOfRecords = fci.GetNumberOfRecords();
        }

    // Get the tag value for current File
    TUint tagValue = iType2FileArray[iCurrentArrayCounter].tagValue;

    if( iCurrentArrayCounter < iType2FileArray.Count() )
        {
        if( UICC_STATUS_OK  == aStatus )
            {
            switch(tagValue)
                {
                case UICC_EF_ANR_TAG:
                    {
                    ret = CheckRecordSizeForFixedLength( 2, UICC_EF_PBC_TAG,recordLength );

                    if(KErrNone == ret)
                        {
                        // total no of entries in all ANR files
                        iPBStoreInfoData->iANRNumOfEntries = noOfRecords* iNumOfPBRRecords;
                        // For this need to know about the ext files
                        iPBStoreInfoData->iANRNumLengthMax = UICC_NO_EXT_MAX_NUM_LEN;
                        // No of entries in every EF
                        iPBStoreInfoData->iANRNumOfEntriesPerEntry = noOfRecords;
                        // no of ANR Files type2 per ADN phonebook
                        iType2ANRFiles++;
                        }
                    }
                    break;
                case UICC_EF_EMAIL_TAG:
                    {
                    // get the total no of entries in all EF in PBR Files
                    iPBStoreInfoData->iEmailNumOfEntries = noOfRecords * iNumOfPBRRecords;
                    // get no of entries per Email EF
                    iPBStoreInfoData->iEmailNumOfEntriesPerEntry = noOfRecords;

                    // get record Length from filedata, In Type2 and type3 files case email text length will be 2 less than record length
                    iPBStoreInfoData->iEmailTextLengthMax = recordLength - 2;

                    // get the no of type2 email files in every ADN Phonebook
                    iType2EmailFiles++;
                    }
                    break;
                case UICC_EF_SNE_TAG:
                    {
                    iPBStoreInfoData->iSNENumOfEntries = noOfRecords * iNumOfPBRRecords;
                    iPBStoreInfoData->iSNENumOfEntriesPerEntry = noOfRecords;

                    // get record Length from filedata, In Type2 and type 3 file case SNE text length will be 2 less than record length
                    iPBStoreInfoData->iSNETextLengthMax = recordLength - 2;

                    // Get the no of Type 2 SNE Files per ADN phonebook
                    iType2SNEFiles++;
                    }
                    break;
                default:
                    break;
                }
            if(KErrNone != ret)
                {
                iType2FileArray.Remove(iCurrentArrayCounter);
                }
            else
                {
                iCurrentArrayCounter++;
                }
            }
        else
            {
            iCurrentArrayCounter++;
            }

        // Check for next Phase
        if( iCurrentArrayCounter == iType2FileArray.Count() )
            {
            iCurrentArrayCounter = 0;
            if( 0 != iType3FileArray.Count() )
                iIniPhase = EPBInitPhase_3GADN_Type3;
            else
                iIniPhase = EPBIniPhase_3GADNDone;
            }
        }

    }


// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::HandleType3FileResp
// HAndles Data received for Type 3 file request
// -----------------------------------------------------------------------------
//


void CMmPhoneBookOperationInit3G_adn::HandleType3FileResp(const TDesC8 &aFileData, TInt aStatus)
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::HandleType3FileResp" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_HANDLETYPE3FILERESP, "CMmPhoneBookOperationInit3G_adn::HandleType3FileResp" );


    TInt ret( KErrNone );
    TInt recordLength( 0 );
    TInt noOfRecords(0);


    if( UICC_STATUS_OK  == aStatus )
        {
        TFci fci( aFileData );
        recordLength = fci.GetRecordLength();
        noOfRecords = fci.GetNumberOfRecords();
        }

    // Get the tag value for current File
    TUint tagValue ( iType3FileArray[iCurrentArrayCounter].tagValue );

    if( iCurrentArrayCounter < iType3FileArray.Count() )
        {
        if( UICC_STATUS_OK  == aStatus )
            {
            switch( tagValue )
                {
                case UICC_EF_AAS_TAG:
                    break;
                case UICC_EF_EXT1_TAG:
                {
                    // Check for Fixed record length
                    ret = CheckRecordSizeForFixedLength( 3, UICC_EF_EXT1_TAG,recordLength );

                    // check for Error if Length doesn'nt match
                    if( KErrNone == ret )
                        {
                        // Get the file file length for extension data only (remove record type and Identifier 2 bytes)
                        TInt fileLength ( ( recordLength - 2 ) * noOfRecords );
                            iPBStoreInfoData->iADNNumberLengthMax = UICC_NO_EXT_MAX_NUM_LEN+(2*fileLength);

                            // Check file is not invalid
                            if( UICC_NO_EXT_MAX_NUM_LEN == iPBStoreInfoData->iANRNumLengthMax )
                            {
                            iPBStoreInfoData->iANRNumLengthMax = UICC_NO_EXT_MAX_NUM_LEN+(2*fileLength);
                            }
                        }
                    break;
                  }
                case UICC_EF_GAS_TAG:
                    break;
                case UICC_EF_CCP1_TAG:
                    break;
                default:
                    break;
                }
            }


        if( KErrNone != ret )
            {
            // remove from teh Array
            iType3FileArray.Remove(iCurrentArrayCounter);
            }
        else
            {
            iCurrentArrayCounter++;
            }

        // Check for next Phase
        if( iCurrentArrayCounter == iType3FileArray.Count() )
            {
            iCurrentArrayCounter = 0;
            iIniPhase = EPBIniPhase_3GADNDone;
            }
        }
    }




// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::CheckForFileId
// Check for no any 2 file id scould be same in any type 1, Type2 and tyoe3 files
// -----------------------------------------------------------------------------
//

TInt CMmPhoneBookOperationInit3G_adn::CheckForFileId()
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationInit3G_adn::CheckForFileId");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_CHECKFORFILEID, "CMmPhoneBookOperationInit3G_adn::CheckForFileId" );


    TInt ret( KErrNone );

    // Check for Type1 File Id's with all Type1  , Type2 and Type3 file Id's
        for( TInt j=0; j< iType1FileArray.Count(); j++ )
            {
            // Check for Type 1 files
            for( TInt i = (j+1); i< (iType1FileArray.Count()-j); i++ )
                {
                if( iType1FileArray[i].tagFID == iType1FileArray[j].tagFID )
                    {
                    // Check if it is a ADN EF then it will be a failure case
                    if( iType1FileArray[i].tagValue == UICC_ADN_PRIM_TAG )
                    ret = KErrGeneral;
                    else
                        {
                        // remove Entry
                        iType1FileArray.Remove(i);
                        iType1FileArray.Remove(j);
                        }
                    }
                }
            // Check for Type2 file id's
            for(TInt i=0; i<iType2FileArray.Count(); i++)
                {
                if( iType2FileArray[i].tagFID == iType1FileArray[j].tagFID )
                    {
                    // Check for ADN file
                    if( iType1FileArray[j].tagValue == UICC_ADN_PRIM_TAG )
                        ret = KErrGeneral;
                    else
                        {
                        // remove thise Entries
                        iType1FileArray.Remove(j);
                        iType2FileArray.Remove(i);
                        }
                    }
                }
            // Check for Type 3 File Id's
            for( TInt i=0; i<iType3FileArray.Count(); i++ )
                {
                if( iType3FileArray[i].tagFID == iType1FileArray[j].tagFID )
                    // Check for ADN file
                    if( iType1FileArray[j].tagValue == UICC_ADN_PRIM_TAG )
                        ret = KErrGeneral;
                    else
                        {
                        // remove those entries
                        iType1FileArray.Remove(j);
                        iType3FileArray.Remove(i);
                        }
                    }
                }

        // Check for Type2 File Id's with all Type2 and Type3 file Id's
        for( TInt j=0; j< iType2FileArray.Count(); j++ )
            {
            // Check for Type 2 files
            for( TInt i = (j+1); i< ( iType2FileArray.Count()-j ); i++ )
                {
                if( iType2FileArray[i].tagFID == iType2FileArray[j].tagFID)
                        {
                        iType2FileArray.Remove(j);
                        iType2FileArray.Remove(i);
                        }
                    }
            // Check for Type 3 File Id's
            for( TInt i=0; i<iType3FileArray.Count(); i++ )
                {
                if( iType3FileArray[i].tagFID == iType2FileArray[j].tagFID )
                        {
                        iType2FileArray.Remove(j);
                        iType3FileArray.Remove(i);
                        }
                }
            }

        //Check for Type3 File Id's with all other Type3 file Id's

        for( TInt j=0; j< iType3FileArray.Count(); j++ )
            {
            // Check for Type 2 files
            for( TInt i = (j+1); i< (iType3FileArray.Count()-j); i++ )
                {
                if( iType3FileArray[i].tagFID == iType3FileArray[j].tagFID )
                        {
                        iType3FileArray.Remove(j);
                        iType3FileArray.Remove(i);

                        }
                }
            }

    return ret;
    }

// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::CheckForNoOfRecords
// check for no of record for type1 file should be same as ADN file
// -----------------------------------------------------------------------------
//

TInt CMmPhoneBookOperationInit3G_adn::CheckForNoOfRecords( TInt aNoOfRecord )
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::CheckForNoOfRecords" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_CHECKFORNOOFRECORDS, "CMmPhoneBookOperationInit3G_adn::CheckForNoOfRecords" );


    TInt ret( KErrNone );

            if( iPBStoreInfoData->iADNNumOfEntries != aNoOfRecord )
                ret = KErrGeneral;
    return ret;
    }


// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::CheckForSameType1AndType2Files
// Check for no any type 1 file should be type define in PBR file
// -----------------------------------------------------------------------------
//

TInt CMmPhoneBookOperationInit3G_adn::CheckForSameType1AndType2Files()
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::CheckForSameType1AndType2Files" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_CHECKFORSAMETYPE1ANDTYPE2FILES, "CMmPhoneBookOperationInit3G_adn::CheckForSameType1AndType2Files" );

    TInt ret( KErrNone );

    // Check for No any File can be both Type1 and Type2

    for( TInt i=0; i<iType1FileArray.Count();i++ )
        {
        for( TInt j=0; j<iType2FileArray.Count(); j++ )
            {
            if( iType2FileArray[j].tagValue == iType1FileArray[i].tagValue )
                {
                // Check for if it is ADN File
                if( iType1FileArray[i].tagValue == UICC_ADN_PRIM_TAG )
                    ret = KErrGeneral;
                else
                    {
                    iType1FileArray.Remove(i);
                    iType2FileArray.Remove(j);
                    }
                }
            }
        }
    return ret;
    }



// -----------------------------------------------------------------------------
// CmmPhonebookOperationInit::CheckRecordSizeForFixedLength
//Check for if Fixed length file correct or not
// -----------------------------------------------------------------------------
//

TInt CMmPhoneBookOperationInit3G_adn::CheckRecordSizeForFixedLength( TInt aFileType, TInt aFileId, TInt aRecordLength)
    {
    TFLOGSTRING( "TSY: CMmPhoneBookOperationInit3G_adn::CheckRecordSizeForFixedLength" );
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONINIT3G_ADN_CHECKRECORDSIZEFORFIXEDLENGTH, "CMmPhoneBookOperationInit3G_adn::CheckRecordSizeForFixedLength" );

    TInt ret( KErrNone );

    // Check for Record Size for Fixed length record size
    switch( aFileId )
        {
        case UICC_EF_ANR_TAG:
            {
                if(( 1 == aFileType ) && ( 15 != aRecordLength ))
                    ret = KErrGeneral;
                else
                    {
                    if(( 2== aFileType )&& ( 17!=aRecordLength ))
                        ret =KErrNone;
                    }
            break;
            }
        case UICC_EF_PBC_TAG:
            {
            if( 2!=aRecordLength )
                ret = KErrGeneral;
            break;
            }
        case UICC_EF_UID_TAG:
            {
            if( 2!= aRecordLength )
                ret = KErrGeneral;
            break;
            }
        case UICC_EF_EXT1_TAG:
            {
            if( 13 != aRecordLength )
                ret = KErrGeneral;
            break;
            }
        }
    return ret;
    }



