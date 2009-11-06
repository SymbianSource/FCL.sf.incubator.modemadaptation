/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for Cell Broadcast tests
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "cbs.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCbs::CCbs
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCbs::CCbs( CTestModuleIf& aTestModuleIf ):CCore( aTestModuleIf )
    {
    // Initial values
    }

// -----------------------------------------------------------------------------
// CCbs::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCbs::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif
    iCbsLog = CStifLogger::NewL( KCbsLogPath,
                          KCbsLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    iCbsLog->Log(_L8("CCbs::ConstructL"));

    InitializeIscControlL(KIscControlTestCaseFileCbs);

    // Initialise CB GSM data buffer from parameter checking
    TUint8 cbsDataGsm[RMobileBroadcastMessaging::KBroadcastPageSize]= {
        0x40,0x00,0x00,0x65,0x01,0x2a,0x68,0x3a,0x1d,0xae,0x7b,0xbd,0xee,0xf7,
        0xbb,0xcb,0xfd,0x5e,0xa7,0xc3,0xae,0xf1,0xbb,0xdd,0x50,0x80,0x40,0x20,
        0x10,0x08,0x04,0x02,0x81,0x40,0x20,0x10,0x08,0x04,0x02,0x81,0x40,0x20,
        0x10,0x08,0x04,0x02,0x81,0x40,0x20,0x10,0x08,0x04,0x02,0x81,0x40,0x20,
        0x10,0x08,0x04,0x02,0x81,0x40,0x20,0x10,0x08,0x04,0x02,0x81,0x40,0x20,
        0x10,0x08,0x04,0x02,0x81,0x40,0x20,0x10,0x08,0x04,0x02,0x81,0x40,0x20,
        0x10,0x08,0x04,0x02 };
    iCbsMsgGSM.Append( &cbsDataGsm[0],sizeof( cbsDataGsm ) );

    // Initialise CB WCDMA data buffer from parameter checking
    TUint8 cbsDataWcdma[RMobileBroadcastMessaging::KBroadcastPageSize]= {
        0xC3,0x32,0x9B,0x0D,0x12,0xCA,0xDF,0x61,0xF2,0x38,0x3C,0xA7,0x83,0x40,
        0xF4,0xF2,0x9C,0x0E,0x6A,0x97,0xE7,0xF3,0xF0,0xB9,0x0C,0xA2,0xBE,0xE1,
        0xE9,0x31,0x68,0x46,0x6B };

    iCbsMsgWCDMA.Append( &cbsDataWcdma[0],sizeof( cbsDataWcdma ) );
    iCbsMsgWCDMA.SetLength( 33 );
    }

// -----------------------------------------------------------------------------
// CCbs::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCbs* CCbs::NewL( CTestModuleIf& aTestModuleIf )
    {
    CCbs* self = new (ELeave) CCbs( aTestModuleIf );

    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CCbs::~CCbs()
    {
    // Delete resources allocated from test methods
    Delete();

    delete iCbsLog;
    }

// -----------------------------------------------------------------------------
// CCbs::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CCbs::RouteCompletion
    (
    TInt aIpc,
    CMmDataPackage* aDataPackage,
    TInt aResult
    )
    {

    // Process the ISI msg according to IPC requested.
    if ( !iHandleSpecifiedRequests )
        {
        switch(aIpc)
            {
            case EMobileBroadcastMessagingReceiveMessage:
                {
                CompleteActivateCbsReception( aResult );
                break;
                }
            case EMobileBroadcastMessagingReceiveMessageCancel:
                {
                CompleteDeactivateCbsReception( aResult );
                break;
                }
            case EMmTsyGsmBroadcastNotifyMessageReceived:
                {
                CompleteReceiveGsmCbs( aResult, *aDataPackage );
                break;
                }
            case EMmTsyWcdmaBroadcastNotifyMessageReceived:
                {
                CompleteReceiveWcdmaCbs( aResult, *aDataPackage );
                break;
                }
            case ECustomStartSimCbTopicBrowsingIPC:
                {
                CompleteReadCBSSimTopics( aResult, *aDataPackage );
                break;
                }
            case ECustomDeleteSimCbTopicIPC:
                {
                CompleteDeleteCBSSimTopic( aResult );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    else
        {
        switch(aIpc)
            {
            case EMobileBroadcastMessagingReceiveMessage:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMobileBroadcastMessagingReceiveMessage" ) ) )
                    {
                    CompleteActivateCbsReception( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMobileBroadcastMessagingReceiveMessageCancel:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                   ( _L( "EMobileBroadcastMessagingReceiveMessageCancel" ) ) )
                    {
                    CompleteDeactivateCbsReception( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMmTsyGsmBroadcastNotifyMessageReceived:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyGsmBroadcastNotifyMessageReceived" ) ) )
                    {
                    CompleteReceiveGsmCbs( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case EMmTsyWcdmaBroadcastNotifyMessageReceived:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "EMmTsyWcdmaBroadcastNotifyMessageReceived" ) ) )
                    {
                    CompleteReceiveWcdmaCbs( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomStartSimCbTopicBrowsingIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomStartSimCbTopicBrowsingIPC" ) ) )
                    {
                    CompleteReadCBSSimTopics( aResult, *aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            case ECustomDeleteSimCbTopicIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                    ( _L( "ECustomDeleteSimCbTopicIPC" ) ) )
                    {
                    CompleteDeleteCBSSimTopic( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CCbs::NewL( aTestModuleIf );

    }

//  End of File
