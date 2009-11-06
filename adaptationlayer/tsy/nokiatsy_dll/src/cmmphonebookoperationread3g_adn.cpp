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
#include "tsylogger.h"
#include "cmmphonebookoperationread3g_adn.h"
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookoperationreadtraces.h"
#include "OstTraceDefinitions.h"
#include "cmmphonebookoperationread3g_adnTraces.h"
#endif


// EXTERNAL DATA STRUCTURES
    // None

// EXTERNAL FUNCTION PROTOTYPES
    // None

// CONSTANTS
//const TUint16 KSimPbLocationMBDN = 0x0100;

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
// CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn
// C++ default CMmPhoneBookOperationRead3g_adn can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn()
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD3G_ADN_CMMPHONEBOOKOPERATIONREAD3G_ADN, "CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn" );


    }

// -----------------------------------------------------------------------------
//CMmPhoneBookOperationRead3g_adn::CMmPhoneBookOperationRead3g_adn C++ destructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationRead3g_adn::~CMmPhoneBookOperationRead3g_adn
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::~CMmPhoneBookOperationRead3g_adn");
    OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_CMMPHONEBOOKOPERATIONREAD3G_ADN, "CMmPhoneBookOperationRead3g_adn::~CMmPhoneBookOperationRead3g_adn" );

    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::NewL
// Creates a new CMmPhonebookOperationRead object instance.
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmPhoneBookOperationRead3g_adn* CMmPhoneBookOperationRead3g_adn::NewL
    (
// NONE
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::NewL");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD3G_ADN_NEWL, "CMmPhoneBookOperationRead3g_adn::NewL" );


    TName phonebookTypeName;

    CMmPhoneBookOperationRead3g_adn* mmPhoneBookOperationRead3g_adn =
        new( ELeave ) CMmPhoneBookOperationRead3g_adn();

    return mmPhoneBookOperationRead3g_adn;
    }

// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::ConstructL
// Initialises object attributes.
// -----------------------------------------------------------------------------
//
void CMmPhoneBookOperationRead3g_adn::ConstructL
    (
    // None
    )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::ConstructL");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD3G_ADN_CONSTRUCTL, "CMmPhoneBookOperationRead3g_adn::ConstructL" );

    }


// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq
// Handles Read Resp
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq
        (
        TInt aFileId,
        TInt aFileSFI
        )
    {
    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq");
    OstTrace0( TRACE_NORMAL, CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCHANDLEDATA3GADNREADREQ, "CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq" );

    TInt ret(KErrNone);

    // get the index to be read from phonebook
    TInt index( iIndexToRead );
    TInt appFileId(APPL_FILE_ID);

    TUiccReadLinearFixed cmdParams;
    //cmdParams.messHandlerPtr  = static_cast<MUiccOperationBase*> iMmPhoneBookStoreMessHandler;

    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( MF_FILE ));
    cmdParams.filePath.Append(appFileId >> 8);
    cmdParams.filePath.Append(appFileId);
    cmdParams.filePath.Append(static_cast<TUint8>( DF_PHONEBOOK >> 8 ));
    cmdParams.filePath.Append(static_cast<TUint8>( DF_PHONEBOOK ));

    index = iIndexToRead && 0x00FF;

    if( PB_PBR_FID == aFileId )
        {
        // Find which record no should be read
        // get ADN file ID from PBR EF data according to which entry needs to be read
        TUint8 pbrRecordNo(0);
        do
            {
            pbrRecordNo++;
            index = index-255;
            }while(index > 255);

        cmdParams.fileId = PB_PBR_FID;
        cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
        cmdParams.record = pbrRecordNo;
        }
    else
        {
        cmdParams.fileId = aFileId;
        cmdParams.serviceType = UICC_APPL_READ_LINEAR_FIXED;
        cmdParams.record = index;
        cmdParams.fileIdSfi = aFileSFI;
        }
    return ret;
    }




// -----------------------------------------------------------------------------
// CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL
// HAndles Response for ADN phoneBook
// -----------------------------------------------------------------------------
//
TInt CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL
    (
    const TInt aStatus,
    const TDes8 &/*aFileData*/,
    const TInt /*aTransId*/
    )
    {

    TFLOGSTRING("TSY: CMmPhoneBookOperationRead3g_adn::HandleUICC3gADNRespL");
    OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD3G_ADN_UICCHANDLEDATA3GADNREADREQ, "CMmPhoneBookOperationRead3g_adn::UICCHandleData3gADNReadReq" );


    TInt ret(KErrNone);
    // Initialize Application file ID to send in next request

    // Handle possible error case
    if ( UICC_STATUS_OK != aStatus )
        {
        TFLOGSTRING("TSY: CMmPhoneBookOperationRead::HandleUICCPbRespRead-\
            Unsuccessfully completed by SIMSON");
//OstTrace0( TRACE_NORMAL, DUP1_CMMPHONEBOOKOPERATIONREAD_HANDLEUICCPBRESPL, "CMmPhoneBookOperationRead::HandleUICCPbRespL, Unsuccessfully completed by UICC Server" );
        ret = CMmStaticUtility::UICCCSCauseToEpocError(aStatus );
        }


    if(UICC_STATUS_OK == aStatus)
        {

        // Case: <complete request>
        if ( KErrNone != ret || 0 == iNumOfEntriesToRead )
            {
            // If no entries found
            if ( 0 == iNumOfEntriesFilled )
                {
                ret = KErrNotFound;
                }

            }
          }
    return ret;
    }



