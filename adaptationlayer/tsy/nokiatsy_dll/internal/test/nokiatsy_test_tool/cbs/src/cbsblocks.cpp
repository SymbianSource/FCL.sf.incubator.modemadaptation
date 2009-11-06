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
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include "cbs.h"
#include <ctsy/serviceapi/cmmsmsutility.h>
#include <ctsy/rmmcustomapi.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCbs::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CCbs::Delete()
    {
    }

// -----------------------------------------------------------------------------
// CCbs::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCbs::RunMethodL(
    CStifItemParser& aItem )
    {
    iCbsLog->Log(_L8("CCbs::RunMethodL"));

    static TStifFunctionInfo const KFunctions[] =
        {
        ENTRY( "RunIscTestCase",  CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase", CCore::SyncIscTestCase),
        ENTRY( "ActivateCbsReception", CCbs::ActivateCbsReceptionL ),
        ENTRY( "DeactivateCbsReception", CCbs::DeactivateCbsReceptionL ),
        ENTRY( "ReceiveCbs", CCbs::ReceiveCbsL ),
        ENTRY( "ReadCBSSimTopics", CCbs::ReadCBSSimTopicsL ),
        ENTRY( "DeleteCBSSimTopic", CCbs::DeleteCBSSimTopicL ),
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC", CCore::SetExpectedCompleteIPC),
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    
    
// -----------------------------------------------------------------------------
// CCbs::ActivateCbsReceptionL
// -----------------------------------------------------------------------------
//
TInt CCbs::ActivateCbsReceptionL(CStifItemParser& /*aItem*/)
    {
    iCbsLog->Log(_L8("CBS::ActivateCbsReception"));

    TInt ret = KErrGeneral;

    TCbsCbmiAndLangAndFilter filter;
    filter.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptAll;
    CMmDataPackage package;
    package.PackData(&filter);
    ret = HandleRequestL( EMobileBroadcastMessagingReceiveMessage, &package );
    if ( KErrNone != ret )
        {
        iCbsLog->Log(_L8("Error in activating CBS reception: %d"), ret );
        }
    else
        {
        iActivateCbsReceptionPending = ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CCbs::CompleteActivateCbsReception
// -----------------------------------------------------------------------------
//
void CCbs::CompleteActivateCbsReception(TInt aErr)
    {
    iCbsLog->Log(_L8("CBS::CompleteActivateCbsReception, err=%d"),aErr);
    if (iActivateCbsReceptionPending)
        {
        iActivateCbsReceptionPending = EFalse;
        Signal( aErr );
        }
    }

// -----------------------------------------------------------------------------
// CCbs::DeactivateCbsReceptionL
// -----------------------------------------------------------------------------
//
TInt CCbs::DeactivateCbsReceptionL(CStifItemParser& /*aItem*/)
    {
    iCbsLog->Log(_L8("CBS::DeactivateCbsReception"));

    TInt ret = KErrGeneral;

    TCbsCbmiAndLangAndFilter filter;
    filter.iSetting = RMobileBroadcastMessaging::EBroadcastAcceptNone;
    CMmDataPackage package;
    package.PackData(&filter);
    ret = HandleRequestL( EMobileBroadcastMessagingReceiveMessageCancel, &package );
    if ( KErrNone != ret )
        {
        iCbsLog->Log(_L8("Error in deactivating CBS reception: %d"), ret );
        }
    else
        {
        iDeactivateCbsReceptionPending = ETrue;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CCbs::CompleteDeactivateCbsReception
// -----------------------------------------------------------------------------
//
void CCbs::CompleteDeactivateCbsReception(TInt aErr)
    {
    iCbsLog->Log(_L8("CBS::CompleteDeactivateCbsReception, err=%d"),aErr);
    if (iDeactivateCbsReceptionPending)
        {
        iDeactivateCbsReceptionPending = EFalse;
        Signal( aErr );
        }
    }

// -----------------------------------------------------------------------------
// CCbs::ReceiveCbsL
// -----------------------------------------------------------------------------
//
TInt CCbs::ReceiveCbsL(CStifItemParser& aItem)
    {
    iCbsLog->Log(_L8("CBS::ReceiveCbsL. Waiting for CBS message (be patient, please)."));

    // Set reference parameters for parameter checking
    TInt temp( 0 );
    aItem.GetNextInt( temp );
    iSbNumber = temp;

    aItem.GetNextInt( temp );
    iNumberOfPages = temp;

    aItem.GetNextInt( temp );
    iMessageType = temp;

    aItem.GetNextInt( temp );
    iMessageId = temp;

    aItem.GetNextInt( temp );
    iSerialNum = temp;

    aItem.GetNextInt( temp );
    iDCS = temp;

    aItem.GetNextInt( temp );
    iInfoLength = temp;

    // do nothing but waiting ...
    iReceiveCbsPending = ETrue;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCbs::CompleteReceiveCbs, GSM only
// -----------------------------------------------------------------------------
//
void CCbs::CompleteReceiveGsmCbs(TInt aErr, CMmDataPackage& aData)
    {
    iCbsLog->Log(_L8("CBS::CompleteReceiveGsmCbs, err=%d"),aErr);
    if (iReceiveCbsPending)
        {
        if ( KErrNone == aErr )
            {
            _LIT8( KReceiveCbsOk, "CBS receiving ok" );
            iCbsLog->Log((TDesC8)KReceiveCbsOk );

            TGsmCbsMsg cbsMsg;
            aData.UnPackData( cbsMsg );
            // CBS message starts with:
            //  - serial nr MSB
            //  - serial nr LSB
            //  - message id MSB
            //  - message id LSB
            //  - data coding scheme (DCS)
            iCbsLog->Log(_L8("Received msg has serial nr=%d, cbmi=%d, dcs=%d"),
                ((cbsMsg.iCbsMsg[0]<<8)+cbsMsg.iCbsMsg[1]),
                ((cbsMsg.iCbsMsg[2]<<8)+cbsMsg.iCbsMsg[3]),
                cbsMsg.iCbsMsg[4]);

            // Check received data
            if ( iHandleSpecifiedRequests )
                {
                if ( cbsMsg.iCbsMsg != iCbsMsgGSM )
                    {
                    aErr = KErrGeneral;
                    }
                }
            }
        else
            {
            _LIT8( KReceiveCbsFailed, "CBS receiving failed: %d" );
            iCbsLog->Log((TDesC8)KReceiveCbsFailed, aErr );
            }
        iReceiveCbsPending = EFalse;
        Signal( aErr );
        }
    }

// -----------------------------------------------------------------------------
// CCbs::CompleteReceiveWcdmaCbs, WCDMA only
// -----------------------------------------------------------------------------
//
void CCbs::CompleteReceiveWcdmaCbs( TInt aErr, CMmDataPackage& aData )
    {
    iCbsLog->Log(_L8("CBS::CompleteReceiveWcdmaCbs."));
    if ( iReceiveCbsPending )
        {
        if ( KErrNone == aErr )
            {
            _LIT8( KReceiveCbsOk, "CBS WCDMA receiving ok" );
            iCbsLog->Log((TDesC8)KReceiveCbsOk );
            CArrayPtrFlat<TWcdmaCbsMsg>* cbsMsg;
            TInt sbNumber;
            aData.UnPackData( cbsMsg, sbNumber );
            TWcdmaCbsMsg* wcdmaCbsMsg( cbsMsg->At( 0 ) );
            iCbsLog->Log(_L8("Received msg has serial nr=%d, cbmessage id=%d, dcs=%d"),
                wcdmaCbsMsg->iSerialNum, wcdmaCbsMsg->iMessageId,wcdmaCbsMsg->iDCS );

            // Check received data
            if ( iHandleSpecifiedRequests )
                {
                TBuf8<RMobileBroadcastMessaging::KBroadcastPageSize>
                    iWcdmaCbsDataTemp;
                iWcdmaCbsDataTemp.Append(
                    wcdmaCbsMsg->iWcdmaCbsData.Mid ( 0 , wcdmaCbsMsg->iInfoLength ) );

                if ( iWcdmaCbsDataTemp != iCbsMsgWCDMA )
                    {
                    aErr = KErrGeneral;
                    }
                if ( wcdmaCbsMsg->iSbNumber != iSbNumber )
                    {
                    aErr = KErrGeneral;
                    }
                if ( wcdmaCbsMsg->iNumberOfPages != iNumberOfPages )
                    {
                    aErr = KErrGeneral;
                    }
                if ( wcdmaCbsMsg->iMessageType != iMessageType )
                    {
                    aErr = KErrGeneral;
                    }
                if ( wcdmaCbsMsg->iMessageId != iMessageId )
                    {
                    aErr = KErrGeneral;
                    }
                if ( wcdmaCbsMsg->iSerialNum != iSerialNum )
                    {
                    aErr = KErrGeneral;
                    }
                if ( wcdmaCbsMsg->iDCS != iDCS )
                    {
                    aErr = KErrGeneral;
                    }
                if ( wcdmaCbsMsg->iInfoLength != iInfoLength )
                    {
                    aErr = KErrGeneral;
                    }
                }
            }
         else
            {
            _LIT8( KReceiveCbsFailed, "CBS WCDMA receiving failed: %d" );
            iCbsLog->Log((TDesC8)KReceiveCbsFailed, aErr );
            }
        iReceiveCbsPending = EFalse;
        Signal( aErr );
        }
    }


// -----------------------------------------------------------------------------
// CCbs::ReadCBSSimTopicsL
// -----------------------------------------------------------------------------
//
TInt CCbs::ReadCBSSimTopicsL(CStifItemParser& /*aItem*/)
    {
    iCbsLog->Log(_L8("CBS::ReadCBSSimTopicsL"));
    TInt ret = KErrGeneral;
    ret = HandleRequestL( ECustomStartSimCbTopicBrowsingIPC );
    if ( KErrNone != ret )
        {
        iCbsLog->Log(_L8("Error in reading CBS SIM topics: %d"), ret );
        }
    else
        {
        iReadCBSSimTopicsPending = ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CCbs::CompleteReadCBSSimTopics
// -----------------------------------------------------------------------------
//
void CCbs::CompleteReadCBSSimTopics(TInt aErr, CMmDataPackage& aData)
    {
    iCbsLog->Log(_L8("CBS::CompleteReadCBSSimTopics, err=%d"),aErr);
    if (iReadCBSSimTopicsPending)
        {
        if ( KErrNone == aErr )
            {
            CArrayFixFlat<RMmCustomAPI::TSimCbTopic>* topics;
            aData.UnPackData( topics );
            TInt count = topics->Count();
            if ( 0 < count )
                {
                iCbsLog->Log(_L8("%d topics read."), count);
                for ( TInt i = 0; i < count; i++ )
                    {
                    iCbsLog->Log(_L8("  topic[%d]=%d."),i,topics->At( i ).iNumber);
                    }
                }
            }
        iReadCBSSimTopicsPending = EFalse;
        Signal(aErr);
        }
    }

// -----------------------------------------------------------------------------
// CCbs::DeleteCBSSimTopicL
// -----------------------------------------------------------------------------
//
TInt CCbs::DeleteCBSSimTopicL(CStifItemParser& aItem)
    {
    TInt nbr;
    aItem.GetNextInt(nbr);
    iCbsLog->Log(_L8("CBS::DeleteCBSSimTopicL, deleting topic %d."),nbr);

    TInt ret = KErrGeneral;
    TBool deleteTopic = ETrue;
    CMmDataPackage package;
    package.PackData(&nbr, &deleteTopic);
    ret = HandleRequestL( ECustomDeleteSimCbTopicIPC, &package );
    if ( KErrNone != ret )
        {
        iCbsLog->Log(_L8("Error in deleting CBS SIM topic: %d"), ret );
        }
    else
        {
        iDeleteCBSSimTopicPending = ETrue;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CCbs::CompleteDeleteCBSSimTopic
// -----------------------------------------------------------------------------
//
void CCbs::CompleteDeleteCBSSimTopic(TInt aErr)
    {
    iCbsLog->Log(_L8("CBS::CompleteDeleteCBSSimTopic, err=%d"),aErr);
    if (iDeleteCBSSimTopicPending)
        {
        iDeleteCBSSimTopicPending = EFalse;
        Signal(aErr);
        }
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
