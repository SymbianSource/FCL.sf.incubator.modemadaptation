/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES

#include <ctsy/pluginapi/cmmdatapackage.h>
#include <etelmm.h>
#include <etelsat.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/mmgsmwcdmautils.h> // utility functions
#include <mmlist.h>   // list types
#include <ctsy/serviceapi/mmtsy_defaults.h> // for kmaxlengthofussdmessage
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ss_wmisi.h>
#include <tisi.h>
#include <uiccisi.h>

#include "cmmcallmesshandler.h" // utility function MapTypeOfNumberMmToIsi
#include "cmmmessagerouter.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h" // utility functions
#include "cmmsupplservmesshandler.h"
#include "tssparser.h" // for parsing service string
#include "tsylogger.h"
#include "OstTraceDefinitions.h"
#include "cmmuiccmesshandler.h"
#include "cmmphonemesshandler.h"  // for CallForwFlagsCachingCompleted
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsupplservmesshandlerTraces.h"
#endif
 // logging
#include <satcs.h>

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
const TUint8 KSsPadding = 0x00; // Filler byte for ISI messages
const TInt KIpcNoValue = 0;
const TUint16 KSsCodeNoValue = 0xFFFF;
const TUint8 KDataAppendToServiceReqHeaderInBytes = 7;

_LIT(KGsmNewPasswordFiller, "0000");
_LIT(KGsmVerifiedPasswordFiller, "1111");

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
    //None

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::CMmSupplServMessHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmSupplServMessHandler::CMmSupplServMessHandler
    (
    //none
    )
    {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::CMmSupplServMessHandler - Start" );
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_CMMSUPPLSERVMESSHANDLER, "CMmSupplServMessHandler::CMmSupplServMessHandler" );
    //none
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmSupplServMessHandler* CMmSupplServMessHandler::NewL
    (
    CMmPhoNetSender* aPhoNetSender, //pointer to the phonet sender
    CMmPhoNetReceiver* aPhoNetReceiver, // pointer to the phoner receiver
    CMmMessageRouter* aMessageRouter, // pointer to the message router
    CMmUiccMessHandler* aUiccMessHandler
    )
    {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::NewL" );
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_NEWL, "CMmSupplServMessHandler::NewL" );

    CMmSupplServMessHandler* supplServMessHandler =
        new ( ELeave ) CMmSupplServMessHandler();

    CleanupStack::PushL( supplServMessHandler );
    supplServMessHandler->iPhoNetSender = aPhoNetSender;
    supplServMessHandler->iMessageRouter = aMessageRouter;
    supplServMessHandler->iMmUiccMessHandler = aUiccMessHandler;
    supplServMessHandler->ConstructL();

    aPhoNetReceiver->RegisterL(
        supplServMessHandler,
        PN_SS,
        SS_SERVICE_COMPLETED_RESP );

    aPhoNetReceiver->RegisterL(
        supplServMessHandler,
        PN_SS,
        SS_SERVICE_FAILED_RESP );

    aPhoNetReceiver->RegisterL(
        supplServMessHandler,
        PN_SS,
        SS_SERVICE_NOT_SUPPORTED_RESP );

    aPhoNetReceiver->RegisterL(
        supplServMessHandler,
        PN_SS,
        SS_GSM_USSD_SEND_RESP );

    aPhoNetReceiver->RegisterL(supplServMessHandler, PN_SS, SS_STATUS_IND );

    aPhoNetReceiver->RegisterL(
        supplServMessHandler,
        PN_SS,
        SS_SERVICE_COMPLETED_IND );

    CleanupStack::Pop( supplServMessHandler );

    return supplServMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::ConstructL
    (
    //none
    )
    {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::ConstructL" );
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_CONSTRUCTL, "CMmSupplServMessHandler::ConstructL" );

    iVoiceServiceMappedToTelephony = EFalse;
    iCircuitServiceMappedToAllBearer = EFalse;
    iGetCallForwardingNumber = EFalse;
    iAlsLine = RMobilePhone::EAlternateLineUnknown;
    //initialize IPC and SsCode
    iIpc = KIpcNoValue;
    iSsCode = KSsCodeNoValue;
    iMsgFlagType = EMsgFlagTypeRel4;
    iGetIccCallForwardingStatus = EFalse;
    iCheckInfoSubblock = EFalse;
    iFdnCheck = EFalse;
    iResourceControlSuppress = EFalse;
    }


// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::~CMmSupplServMessHandler
// Destructor
// -----------------------------------------------------------------------------
//
CMmSupplServMessHandler::~CMmSupplServMessHandler
    (
    //none
    )
    {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::~CMmSupplServMessHandler" );
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_CMMSUPPLSERVMESSHANDLER, "CMmSupplServMessHandler::~CMmSupplServMessHandler" );

    delete iCFResults;
    delete iCBResults;
    delete iCWResults;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::ExtFuncL
// Forwards requests coming from the Symbian OS layer to the
// specific method.
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::ExtFuncL
    (
    TInt aIpc,
    const CMmDataPackage* aDataPackage
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::ExtFuncL: IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_EXTFUNCL, "CMmSupplServMessHandler::ExtFuncL;aIpc=%d", aIpc );

    //*************************************************************//
    // NOTE
    //
    // LICENSEE SPECIFIC MESSAGE HANDLER IMPLEMENTATION STARTS HERE
    //
    //*************************************************************//

    TInt ret( KErrNone );

    // Note: all SS requests use one of the values in
    // TSSOperationType as their transaction ID. This is needed in order
    // to route failed responses to the correct completion method (the
    // response messages for failed and not supported cases don't contain
    // the SS code).

    //save IPC,
    //in case call control changes SS request we can complete right request
    iIpc = aIpc;

    switch ( aIpc )
        {
        case EMobilePhoneGetCallForwardingStatusPhase1:
            {
            //unpack parameters: forwarding condition and service group
            RMobilePhone::TMobilePhoneCFCondition conditionETel =
                RMobilePhone::ECallForwardingUnspecified;
            RMobilePhone::TMobileService serviceGroup =
                RMobilePhone::EServiceUnspecified;

            aDataPackage->UnPackData ( conditionETel, serviceGroup );

            TUint8 operation( SS_INTERROGATION );
            TUint8 basicServiceCode( SS_ALL_TELE_AND_BEARER );

            // Map MM API condition to SS server condition
            ret = MapCFConditionMmToIsi( conditionETel, &iSsCode );

            User::LeaveIfError( ret );

            // If user wishes to get some specific group, then send
            // a different SS request.
            if ( RMobilePhone::EAllServices != serviceGroup &&
                 RMobilePhone::EServiceUnspecified != serviceGroup )
                {
                // Map MM API enum to SS server constant
                ret = MapMobileServiceToBasicServiceCodeIsi(
                    serviceGroup, &basicServiceCode );
                User::LeaveIfError( ret );
                }
            ret = SsCallForwServiceReq(
                ESSOperationTypeGetCallForwardingStatus,
                operation,
                iSsCode,
                0,
                0,
                &KNullDesC,
                basicServiceCode );

            break;
            }

        case EMobilePhoneSetCallForwardingStatus:
            {
            //unpack parameters: TMobilePhoneCFCondition and TMobilePhoneCFChangeV1
            RMobilePhone::TMobilePhoneCFCondition conditionETel =
                RMobilePhone::ECallForwardingUnspecified;
            RMobilePhone::TMobilePhoneCFChangeV1* changeInfo;

            aDataPackage->UnPackData ( conditionETel, changeInfo );

            TUint8 operation( 0 );
            TUint8 typeOfNumber( 0 );
            TInt noReplyTime( 0 );
            TUint8 mmiBasicServiceCode( SS_UNKNOWN_SERVICE );

            iMmCFCondition = conditionETel;
            iMobileService = changeInfo->iServiceGroup;
            iCFAddress = changeInfo->iNumber;

TFLOGSTRING2("TSY: CMmSupplServMessHandler::ExtFuncL - EMobilePhoneSetCallForwardingStatus - Number: %S", &iCFAddress.iTelNumber);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_EXTFUNCL, "CMmSupplServMessHandler::ExtFuncL;EMobilePhoneSetCallForwardingStatus iCFTelNumber=%S", iCFAddress.iTelNumber );
TFLOGSTRING2("TSY: CMmSupplServMessHandler::ExtFuncL - EMobilePhoneSetCallForwardingStatus - MobileService: %d", iMobileService);
OstTrace1( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_EXTFUNCL, "CMmSupplServMessHandler::ExtFuncL;EMobilePhoneSetCallForwardingStatus iMobileService=%d", iMobileService );
TFLOGSTRING2("TSY: CMmSupplServMessHandler::ExtFuncL - EMobilePhoneSetCallForwardingStatus - Condition: %d", iMmCFCondition);
OstTrace1( TRACE_NORMAL, DUP3_CMMSUPPLSERVMESSHANDLER_EXTFUNCL, "CMmSupplServMessHandler::ExtFuncL;EMobilePhoneSetCallForwardingStatus iMmCFCondition=%d", iMmCFCondition );

            //map operation
            ret = MapOperationMmToIsi( changeInfo->iAction, &operation );

            if ( KErrNone == ret )
                {
                //map condition
                ret = MapCFConditionMmToIsi( conditionETel, &iSsCode );
                if ( KErrNone == ret )
                    {
                    //map type of number
                    MapTypeOfNumberMmToIsi(
                        iCFAddress.iTypeOfNumber,
                        iCFAddress.iNumberPlan,
                        typeOfNumber );

                    //map basic service code
                    ret = MapMobileServiceToBasicServiceCodeIsi(
                        changeInfo->iServiceGroup,
                        &mmiBasicServiceCode );

                    if ( KErrNone == ret )
                        {
                        //map the reply time
                        // -1 is defined in Etel MM API as undefined timeout
                        if ( -1 == changeInfo->iTimeout )
                            {
                            noReplyTime = SS_UNDEFINED_TIME;
                            }
                        else
                            {
                            noReplyTime = changeInfo->iTimeout;
                            }
                        //make a pointer to the phone number
                        //call the method
                        ret = SsCallForwServiceReq(
                            ESSOperationTypeSetCallForwardingStatus,
                            operation,
                            iSsCode,
                            typeOfNumber,
                            noReplyTime,
                            &( iCFAddress.iTelNumber ),
                            mmiBasicServiceCode );
                        }
                    }
                }
            break;
            }

        case EMobilePhoneGetBarringStatusPhase1:
            {
            //unpack parameter: barring condition
            RMobilePhone::TMobilePhoneCBCondition conditionETel =
                RMobilePhone::EBarUnspecified;
            aDataPackage->UnPackData ( conditionETel );

            //map condition
            ret = MapCBConditionMmToIsi( conditionETel, &iSsCode );
            if ( KErrNone == ret )
                {
                // password subblock is not added to the ISI
                // message, because all passwords are empty
                ret = SsGsmBarringReq(
                    ESSOperationTypeGetCallBarringStatus,
                    SS_INTERROGATION,
                    iSsCode,
                    &KNullDesC,
                    KNullDesC,
                    KNullDesC ,
                    SS_ALL_TELE_AND_BEARER );
                }
            break;
            }

        case EMobilePhoneSetCallBarringStatus:
            {
            //unpack parameters: TMobilePhoneCBCondition and TMobilePhoneCBChangeV1
            RMobilePhone::TMobilePhoneCBCondition conditionETel =
                RMobilePhone::EBarUnspecified;
            RMobilePhone::TMobilePhoneCBChangeV1* changeInfo;

            aDataPackage->UnPackData ( conditionETel, changeInfo );

            TUint8 operation( 0 );
            TUint8 mmiBasicServiceCode( SS_UNKNOWN_SERVICE );

            //map operation
            ret = MapOperationMmToIsi( changeInfo->iAction, &operation );

            if ( KErrNone == ret )
                {
                //map call barring condition
                ret = MapCBConditionMmToIsi( conditionETel, &iSsCode );
                if ( KErrNone == ret )
                    {
                    //map basic service code
                    ret = MapMobileServiceToBasicServiceCodeIsi(
                        changeInfo->iServiceGroup,
                        &mmiBasicServiceCode );
                    if ( KErrNone == ret )
                        {
                        //send ss call barring service request
                        // fillers are used to send "bad" SS_GSM_PASSWORD
                        // subblock to SS server in case password is omitted
                        ret = SsGsmBarringReq(
                            ESSOperationTypeSetCallBarringStatus,
                            operation,
                            iSsCode,
                            &( changeInfo->iPassword ),
                            KGsmNewPasswordFiller,
                            KGsmVerifiedPasswordFiller,
                            mmiBasicServiceCode );
                        }
                    }
                }
            break;
            }

        case EMobilePhoneSetSSPassword:
            {
            //unpack parameter: TMobilePhonePasswordChangeV2 and TUint16
            RMobilePhone::TMobilePhonePasswordChangeV2 passwd;
            aDataPackage->UnPackData ( passwd, iSsCode );

            // password change affects all services
            TUint8 mmiBasicServiceCode = SS_ALL_TELE_AND_BEARER;

            // client is fully responsible for making
            // "bad" password subblock
            ret = SsGsmBarringReq(
                ESSOperationTypeSetSSPassword,
                SS_GSM_PASSWORD_REGISTRATION,
                iSsCode,
                &passwd.iOldPassword,
                passwd.iNewPassword,
                passwd.iVerifiedPassword,
                mmiBasicServiceCode );
            break;
            }

        case EMobilePhoneGetWaitingStatusPhase1:
            {
            // no packed parameters
            // send ss call waiting service request
            ret = SsGsmWaitingReq(
                ESSOperationTypeGetCallWaitingStatus,
                SS_INTERROGATION,
                SS_GSM_CALL_WAITING,
                SS_ALL_TELE_AND_BEARER );

            iSsCode = SS_GSM_CALL_WAITING;
            break;
            }

        case EMobilePhoneSetCallWaitingStatus:
            {
            //unpack parameters: TMobileService and TMobilePhoneServiceAction
            RMobilePhone::TMobileService serviceGroup =
                RMobilePhone::EServiceUnspecified;
            RMobilePhone::TMobilePhoneServiceAction action;

            aDataPackage->UnPackData ( serviceGroup, action );

            TUint8 operation( 0 );
            TUint8 mmiBasicServiceCode( SS_UNKNOWN_SERVICE );

            //map operation
            ret = MapOperationMmToIsi( action, &operation );
            if ( KErrNone == ret )
                {
                //map basic service code
                ret = MapMobileServiceToBasicServiceCodeIsi(
                    serviceGroup,
                    &mmiBasicServiceCode );

                if ( KErrNone == ret )
                    {
                    // send call waiting service request
                    ret = SsGsmWaitingReq(
                        ESSOperationTypeSetCallWaitingStatus,
                        operation,
                        SS_GSM_CALL_WAITING,
                        mmiBasicServiceCode );

                    iSsCode = SS_GSM_CALL_WAITING;
                    }
                }
            break;
            }

        case EMobilePhoneGetIdentityServiceStatus:
            {
            // packed parameter: TMobilePhoneIdService (service code)
            RMobilePhone::TMobilePhoneIdService serviceETel;
            aDataPackage->UnPackData ( serviceETel );

            MapIdentityServiceMmToIsi( serviceETel, &iSsCode );
            // send ss identity service request
            ret = SsIdentityServiceReq(
                ESSOperationTypeGetIdentityServiceStatus,
                iSsCode );
            break;
            }
        case EMobilePhoneSendNetworkServiceRequest:
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
            {
            // These IPCs are received in case of SAT originated SS request.
            // In that case SS_GSM_SIM_ATK_INFO subblock must be inserted to
            // SS_SERVICE_NO_SIMATK_REQ to indicate if FDN check should be
            // done or not
            TUint8 trId( ESSOperationTypeSendNetworkServiceRequestNoFdnCheck );
            TUint8 operation( SS_OPERATION_UNDEFINED );
            TUint16 serviceCode( SS_CODE_UNDEFINED );
            TPtrC ssInfoA;
            TPtrC ssInfoB;
            TPtrC ssInfoC;
            TPtrC* serviceString;
            TSsParser ssParser;

            // indicates that SS_GSM_SIMATK_INFO subblock must be added
            iCheckInfoSubblock = ETrue;

            // depending on ipc, FDN check must be done or not
            if ( EMobilePhoneSendNetworkServiceRequest == aIpc )
                {
                trId = ESSOperationTypeSendNetworkServiceRequest;
                iFdnCheck = ETrue;
                }

            // packed parameter: TPtrC* (service string)
            aDataPackage->UnPackData( serviceString );

            // parse the service string: operation type, SS code and
            // supplementary info
            ret = ssParser.Parse( *serviceString );

            // if parsing was successful, continue with creating a request
            if ( KErrNone == ret )
                {
                ssParser.GetSsOperation( operation );
                ssParser.GetServiceCode( serviceCode);
                ssParser.GetServiceInfo( ssInfoA, ssInfoB, ssInfoC );

                // set ss code
                iSsCode = serviceCode;

                ret = SsServiceReqSatOriginated(
                    trId,
                    operation,
                    serviceCode,
                    ssInfoA,
                    ssInfoB,
                    ssInfoC );
                }
            else
                {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::ExtFuncL: parsing of \
              service string failed!" );
OstTrace0( TRACE_NORMAL, DUP4_CMMSUPPLSERVMESSHANDLER_EXTFUNCL, "CMmSupplServMessHandler::ExtFuncL, parsing of service string failed!" );
                ret = KErrArgument;
                }

            // initialize flags back to "normal" state
            iFdnCheck = EFalse;
            iCheckInfoSubblock = EFalse;
            break;
            }
        case EMmTsyGetCallForwardingNumberIPC:
            {
            iGetCallForwardingNumber = ETrue;
            ret = UiccReadCallFwdFlagsRel4Req();
            break;
            }
        case EMobilePhoneGetIccMessageWaitingIndicators:
            {
            UiccReadVoiceMsgFlagsRel4Req();
            break;
            }
        case EMobilePhoneSetIccMessageWaitingIndicators:
            {
            //unpack parameter: TMobilePhoneMessageWaitingV1
            RMobilePhone::TMobilePhoneMessageWaitingV1* msgWaiting = NULL;
            aDataPackage->UnPackData ( &msgWaiting );

            if( EMsgFlagTypeRel4 == iMsgFlagType )
                {
                UiccWriteVoiceMsgFlagsRel4Req( *msgWaiting );
                }
                else
                {
                UiccWriteVoiceMsgFlagsCPHSReq( *msgWaiting );
                }
            break;
            }
        case ECustomGetIccCallForwardingStatusIPC:
            {
            iGetIccCallForwardingStatus = ETrue;
            UiccReadCallFwdFlagsRel4Req();
            break;
            }
        case ECustomNotifyIccCallForwardingStatusChangeIPC:
            {
            iNotifyIccCallForwardingStatus = ETrue;
            UiccReadCallFwdFlagsRel4Req();
            break;
            }
        case ESatNotifySendSsPCmd:
            {
            // SIM ATK TSY has informed that next SS request coming from
            // Etel is SAT originated and FDN check should not be done.
            iCheckInfoSubblock = ETrue;

            // FDN check should not be done for SAT originated requests
            iFdnCheck = EFalse;

            // Let's check is resource control suppress needed. This info is set
            // by simatktsy when extFuncL is called with IPC ESatNotifySendSsPCmd
            aDataPackage->UnPackData ( iResourceControlSuppress );

            break;
            }
        default:
            {
            // this method should only be called for SS cases
TFLOGSTRING2("TSY: CMmSupplServMessHandler::ExtFuncL - Unknown IPC: %d",
              aIpc);
OstTrace1( TRACE_NORMAL, DUP5_CMMSUPPLSERVMESSHANDLER_EXTFUNCL, "CMmSupplServMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
            ret = KErrArgument;
            break;
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::ProcessUiccMsg
// Handles UICC messages
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8& aFileData )
    {
TFLOGSTRING3("TSY: CMmSupplServMessHandler::ProcessUiccMsg, transaction ID: %d, status: %d", aTraId, aStatus );
OstTraceExt2( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSUICCMSG, "CMmSupplServMessHandler::ProcessUiccMsg;aTraId=%d;aStatus=%d", aTraId, aStatus );

switch( aTraId )
        {
        case ETrIdReadCallFwdFlagsRel4:
            {
            if ( UICC_STATUS_OK == aStatus )
                {
                // Flags were successfully read
                UiccHandleCallFwdFlagsResp( aStatus, aTraId, aFileData );
                }
            else
                {
                // Reading from rel4 didn't succeed, try to read CPHS
                UiccReadCallFwdFlagsCphsReq();
                }
            break;
            }
        case ETrIdReadCallFwdFlagsCphs:
            {
            UiccHandleCallFwdFlagsResp( aStatus, aTraId, aFileData );
            break;
            }
        case ETrIdWriteCallFwdFlagsRel4:
        case ETrIdWriteCallFwdFlagsCphs:
            {
            // Complete the SetCallForwardingStatus to SOS layer
            // always with KErrNone regardless of writing to SIM failed
            // or succeed
            iMessageRouter->Complete(
                EMobilePhoneSetCallForwardingStatus,
                KErrNone );

            // This package is used at the completion of IPCs:
            // ECustomGetIccCallForwardingStatusIPC and
            // ECustomNotifyIccCallForwardingStatusChangeIPC
            CMmDataPackage customDataPackage;
            customDataPackage.PackData( &iTCFIndicators );

            iMessageRouter->Complete(
                ECustomNotifyIccCallForwardingStatusChangeIPC,
                &customDataPackage,
                KErrNone );
            break;
            }
        case ETrIdReadVoiceMsgFlagsRel4:
            {
            if ( UICC_STATUS_OK == aStatus )
                {
                // Flags were successfully read
                UiccReadVoiceMsgFlagsResp( aTraId, aFileData );
                }
            else
                {
                // Reading from rel4 didn't succeed, try to read CPHS
                UiccReadVoiceMsgFlagsCphsReq();
                }
            break;
            }
        case ETrIdReadVoiceMsgFlagsCphs:
            {
            if ( UICC_STATUS_OK == aStatus )
                {
                UiccReadVoiceMsgFlagsResp( aTraId, aFileData );
                }
            else // Complete error value
                {
                iMessageRouter->Complete(
                    EMobilePhoneGetIccMessageWaitingIndicators,
                    KErrNotFound );
                }
            break;
            }
        case ETrIdWriteVoiceMsgFlagsRel4:
        case ETrIdWriteVoiceMsgFlagsCphs:
             {
             if ( UICC_STATUS_OK == aStatus )
                 {
                 aStatus = KErrNone;
                 }
             else
                 {
                 aStatus = KErrNotFound;
                 }
             iMessageRouter->Complete(
                 EMobilePhoneSetIccMessageWaitingIndicators,
                 aStatus );

             CMmDataPackage dataPackage;
             dataPackage.PackData( &iVMIndicators );
             iMessageRouter->Complete(
                 EMobilePhoneNotifyIccMessageWaitingIndicatorsChange,
                 &dataPackage,
                 aStatus );
             break;
             }
        default:
            {
TFLOGSTRING("TSY: CMmSupplServMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_PROCESSUICCMSG, "CMmSupplServMessHandler::ProcessUiccMsg - unknown transaction ID" );
            break;
            }
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::ReceiveMessageL
// Called when an ISI message has been received.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::ReceiveMessageL
    (
    const TIsiReceiveC &aIsiMessage //received ISI message
    )
    {
    TInt resource (aIsiMessage.Get8bit(ISI_HEADER_OFFSET_RESOURCEID) );
    TInt messageId (aIsiMessage.Get8bit(ISI_HEADER_OFFSET_MESSAGEID) );

TFLOGSTRING3("TSY: CMmSupplServMessHandler::ReceiveMessageL - resource: %d, \
              messageId: %d", resource, messageId);
OstTraceExt2( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_RECEIVEMESSAGEL, "CMmSupplServMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch ( resource )
        {
        case PN_SS:
            {
            switch( messageId )
                {
                case SS_SERVICE_COMPLETED_RESP:
                    {
                    SsServiceCompletedRespL( aIsiMessage );
                    break;
                    }
                case SS_SERVICE_FAILED_RESP:
                    {
                    SsServiceFailedResp( aIsiMessage );
                    break;
                    }
                case SS_SERVICE_NOT_SUPPORTED_RESP:
                    {
                    SsServiceNotSupportedResp( aIsiMessage );
                    break;
                    }
                case SS_GSM_USSD_SEND_RESP:
                    {
                    SsGsmUssdSendResp( aIsiMessage );
                    break;
                    }
                case SS_STATUS_IND:
                    {
                    SsStatusInd( aIsiMessage );
                    break;
                    }
                case SS_SERVICE_COMPLETED_IND:
                    {
                    SsCompleteInd( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING2( "TSY: CMmSupplServMessHandler::ReceiveMessageL - \
               PN_SS - unknown msgId: %d", messageId );
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_RECEIVEMESSAGEL, "CMmSupplServMessHandler::ReceiveMessageL;PN_SS - unknown msgId=%d", messageId );
                    break;
                    }
                } // switch( messageId )
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::ReceiveMessageL - unknown \
              resource: %d", resource);
OstTrace1( TRACE_NORMAL, DUP3_CMMSUPPLSERVMESSHANDLER_RECEIVEMESSAGEL, "CMmSupplServMessHandler::ReceiveMessageL;unknown resource=%d", resource );
            break;
            }
        } // switch( resource )
    }


// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsServiceReqSatOriginated
// Calls appropiate function for creating SS_SERVICE_NO_SIMATK_REQ message
// Returns KErrNone or error code
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::SsServiceReqSatOriginated(
    const TUint8 aTrId,              // transaction ID
    const TUint8 aOperation,         // operation type
    const TUint16 aServiceCode,      // SS code
    const TDesC& ssInfoA,            // SS info A
    const TDesC& ssInfoB,            // SS info B
    const TDesC& ssInfoC )           // SS info C
    {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::SsServiceReqSatOriginated" );
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSSERVICEREQSATORIGINATED, "CMmSupplServMessHandler::SsServiceReqSatOriginated" );

    TInt ret( KErrNone );
    TUint8 basicServiceCode( SS_ALL_TELE_AND_BEARER );
    TSsParser parser;

    // find out the basic service code and other information
    switch ( aServiceCode )
        {
        case SS_GSM_ALL_FORWARDINGS:
        case SS_GSM_ALL_COND_FORWARDINGS:
        case SS_GSM_FORW_UNCONDITIONAL:
        case SS_GSM_FORW_BUSY:
        case SS_GSM_FORW_NO_REPLY:
        case SS_GSM_FORW_NO_REACH:
            {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::SsServiceReqSatOriginated, \
              call forwarding" );
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_SSSERVICEREQSATORIGINATED, "CMmSupplServMessHandler::SsServiceReqSatOriginated, call forwarding" );

            TUint8 numberType ( SS_NBR_TYPE_UNKNOWN );
            TUint8 numberPlan ( SS_NBR_PLAN_ISDN_TELEPHONY );
            TInt noReplyTime ( SS_UNDEFINED_TIME );

            // forwarding number is in ssInfoA
            if ( 0 < ssInfoA.Length( ) )
                {
                TInt index( 0 );

                // check number type
                if ( '+' == ssInfoA.operator[]( 0 ) )
                    {
                    numberType = SS_NBR_TYPE_INTERNATIONAL;
                    index = 1; // ignore '+' in next for loop
                    }

                // check number plan, loop all numbers
                for ( TInt i( index ); i < ssInfoA.Length( ); i++ )
                    {
                    if ( ( ssInfoA.operator[](i) < '0' ) ||
                         ( ssInfoA.operator[](i) > '9' ) )
                        {
                        numberPlan = SS_NBR_PLAN_UNKNOWN;
                        break;
                        }
                    }
                // number type: 4 MSB, number plan: 4 LSB
                numberType = ( ( numberType << 4 ) | numberPlan );
                }

            // basic service code is in ssInfoB, must be converted to TUint8
            basicServiceCode = ( TUint8 )parser.GetInt( ssInfoB );

            // no reply condition timer (if exists) is in ssInfoC,
            // must be converted to TInt
            if ( 0 < ssInfoC.Length( ) )
                {
                noReplyTime = parser.GetInt( ssInfoC );
                }

            ret = SsCallForwServiceReq(
                aTrId,
                aOperation,
                aServiceCode,
                numberType,
                noReplyTime,
                &ssInfoA,
                basicServiceCode );

            break;
            }

        case SS_GSM_ALL_BARRINGS:
        case SS_GSM_BARR_ALL_OUT:
        case SS_GSM_BARR_OUT_INTER:
        case SS_GSM_BARR_OUT_INTER_EXC_HOME:
        case SS_GSM_BARR_ALL_IN:
        case SS_GSM_BARR_ALL_IN_ROAM:
        case SS_GSM_OUTGOING_BARR_SERV:
        case SS_GSM_INCOMING_BARR_SERV:
            {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::SsServiceReqSatOriginated, \
              call barring" );
OstTrace0( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_SSSERVICEREQSATORIGINATED, "CMmSupplServMessHandler::SsServiceReqSatOriginated, call barring" );

            // basic service code is in ssInfoB, must be converted to TUint8
            basicServiceCode = ( TUint8 )parser.GetInt( ssInfoB );

            // if ssInfoA is empty - no password subblock is added
            // if ssInfoA has some data - subblock added with fillers
            ret = SsGsmBarringReq(
                aTrId,
                aOperation,
                aServiceCode,
                &ssInfoA,
                ssInfoA.Length() ? KGsmNewPasswordFiller() : KNullDesC(),
                ssInfoA.Length() ? KGsmVerifiedPasswordFiller() : KNullDesC(),
                basicServiceCode );

            break;
            }

        case SS_GSM_CALL_WAITING:
            {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::SsServiceReqSatOriginated, \
              call waiting" );
OstTrace0( TRACE_NORMAL, DUP3_CMMSUPPLSERVMESSHANDLER_SSSERVICEREQSATORIGINATED, "CMmSupplServMessHandler::SsServiceReqSatOriginated, call waiting" );
            // basic service code is in ssInfoA, must be converted to TUint8
            basicServiceCode = ( TUint8 )parser.GetInt( ssInfoA );
            ret = CreateServiceReqWithCheckInfo(
                aTrId,
                aOperation,
                aServiceCode,
                basicServiceCode );

            break;
            }

        case SS_GSM_CLIP:
        case SS_GSM_CLIR:
        case SS_GSM_COLP:
        case SS_GSM_COLR:
        case SS_GSM_ECT:
        case SS_GSM_CNAP:
            {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::SsServiceReqSatOriginated, \
              other ss codes" );
OstTrace0( TRACE_NORMAL, DUP6_CMMSUPPLSERVMESSHANDLER_SSSERVICEREQSATORIGINATED, "CMmSupplServMessHandler::SsServiceReqSatOriginated, other ss codes" );
            ret = CreateServiceReqWithCheckInfo(
                aTrId,
                aOperation,
                aServiceCode,
                basicServiceCode );

            break;
            }

        default:
            {
TFLOGSTRING2( "TSY: CMmSupplServMessHandler::SsServiceReqSatOriginated, \
               error: unknown ss service: %d", aServiceCode );
OstTraceExt1( TRACE_NORMAL, DUP7_CMMSUPPLSERVMESSHANDLER_SSSERVICEREQSATORIGINATED, "CMmSupplServMessHandler::SsServiceReqSatOriginated;ERROR: unknown ss service=%hu", aServiceCode );
            ret = KErrArgument;
            break;
            }
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::CreateServiceReqWithSimAtkInfo
// Constructs a SS_SERVICE_NO_SIMATK_REQ ISI message with SIM ATK info
// sub block.
// Returns KErrNone or error code
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::CreateServiceReqWithCheckInfo(
    const TUint8 aTrId,
    const TUint8 aOperation,
    const TUint16 aServiceCode,
    const TUint8 aBasicServiceCode )
    {
TFLOGSTRING( "TSY: CMmSupplServMessHandler::CreateServiceReqWithCheckInfo" );
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_CREATESERVICEREQWITHCHECKINFO, "CMmSupplServMessHandler::CreateServiceReqWithCheckInfo" );
    TInt ret( KErrNone );
    TUint8 numOfSubblocks( 1 );

    // create ss service request message and set it to point send buffer
    TIsiSend isimsg( iPhoNetSender->SendBufferDes( ) );

    TBuf8<KDataAppendToServiceReqHeaderInBytes> data;
    // transaction ID ( 1 byte)
    data.Append( aTrId );
    // message ID ( 1 byte)
    data.Append( SS_SERVICE_REQ );
    // operation type
    data.Append( aOperation );
    // basic service code
    data.Append( aBasicServiceCode );
    // SS code
    data.Append( ( aServiceCode & 0xFF00 ) >> 8 );// first byte
    data.Append( ( aServiceCode & 0xFF ) );       // second byte
    // response data to send
    data.Append( SS_SEND_ADDITIONAL_INFO );

    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SS );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    // create subblock SS_GSM_SIMATK_INFO
    TBuf8<SIZE_SS_SB_CHECK_INFO>sbData( 0 );
    TIsiSubBlock ssCheckInfoSb(
        sbData,
        SS_SB_CHECK_INFO,
        EIsiSubBlockTypeId8Len8 );

    if ( iFdnCheck )
        {
        sbData.Append( SS_NO_FDN_CHECK_SUPPRESS );
        }
    else
        {
        sbData.Append( SS_FDN_CHECK_SUPPRESS );
        }

    if ( iResourceControlSuppress )
        {
        sbData.Append( SS_RESOURCE_CONTROL_SUPPRESS );
        }
    else
        {
        sbData.Append( SS_NO_RESOURCE_CONTROL_SUPPRESS );
        }
    iResourceControlSuppress = EFalse;

    // add the SS_SB_CHECK_INFO subblock to service req
    isimsg.CopyData(
        ISI_HEADER_SIZE +
        SIZE_SS_SERVICE_REQ,
        ssCheckInfoSb.CompleteSubBlock( ) );

    isimsg.Set8bit(
        ISI_HEADER_SIZE + SS_SERVICE_REQ_OFFSET_SUBBLOCKCOUNT,
        numOfSubblocks );

    ret = iPhoNetSender->Send( isimsg.Complete( ) );

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsCallForwServiceReq
// Constructs a SS_SERVICE_REQ ISI message with ss forwarding
// sub block.
// Returns KErrNone or error code
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::SsCallForwServiceReq
    (
    TUint8 aTransactionId,         //transaction id
    TUint8 aOperation,             //operation
    TUint16 aMmiSsCode,            //mmi ss code
    TUint8 aTypeOfNumber,          //type of number
    TInt aNoReplyTime,             //no reply time
    TDesC16 const* aForwToNumber, //forwarded to number
    TUint8 aMmiBasicServiceCode    //MMI basic service code
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsCallForwServiceReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSCALLFORWSERVICEREQ, "CMmSupplServMessHandler::SsCallForwServiceReq" );

    TUint8 numOfSubblocks( 0 );
    TBuf<RMobilePhone::KMaxMobileTelNumberSize> number( 0 );
    //number can contain character '+'
    if ( 0 < aForwToNumber->Length() )
        {
        if ( '+' == aForwToNumber->operator[]( 0 ) )
            {
            number.Copy( aForwToNumber->Mid( 1, aForwToNumber->Length() - 1 ) );
            }
        else
            {
            number.Copy( aForwToNumber->Left( aForwToNumber->Length() ) );
            }
        }

    //create ss service request message and set it to point send buffer
    TIsiSend ssServiceReq( iPhoNetSender->SendBufferDes( ) );

    //create ss service request
    TBuf8<KDataAppendToServiceReqHeaderInBytes> data;
    data.Append( aTransactionId );        // transaction id
    // Message id needs to be  SS_SERVICE_REQ until
    // SAT: ETEL Multimode API usage for SAT has been
    // implemented by S60
    data.Append( SS_SERVICE_REQ );

    data.Append( aOperation );            // operation
    data.Append( aMmiBasicServiceCode );  // mmi basic service code

    //split aMmiSsCode to two TBuf8 ( data )
    data.Append( ( aMmiSsCode & 0xFF00 ) >> 8 );// first byte
    data.Append( ( aMmiSsCode & 0xFF ) );       // second byte


    // Custom Tsy's SsAdditionalInfoNotification needs this
    data.Append( SS_SEND_ADDITIONAL_INFO );
    ssServiceReq.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SS );
    ssServiceReq.CopyData( ISI_HEADER_SIZE, data );

    // SS_GSM_SIMATK_INFO must be added to indicate SS server that
    // FDN check must not be done
    if ( iCheckInfoSubblock )
        {
        numOfSubblocks++;

        // create subblock SS_SB_CHECK_INFO
        TBuf8<SIZE_SS_SB_CHECK_INFO>sbData( 0 );
        TIsiSubBlock ssCheckInfoSb(
            sbData,
            SS_SB_CHECK_INFO,
            EIsiSubBlockTypeId8Len8 );

        if ( iFdnCheck )
            {
            sbData.Append( SS_NO_FDN_CHECK_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_FDN_CHECK_SUPPRESS );
            }

        if ( iResourceControlSuppress )
            {
            sbData.Append( SS_RESOURCE_CONTROL_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_NO_RESOURCE_CONTROL_SUPPRESS );
            }
        iResourceControlSuppress = EFalse;

        // add the subblock
        ssServiceReq.CopyData(
            ISI_HEADER_SIZE + SIZE_SS_SERVICE_REQ,
            ssCheckInfoSb.CompleteSubBlock() );
        }
    // SS Server expects that the phone number is present if the SS_FORWARDING
    // sub block is sent.
    if ( number.Length() )
        {
        numOfSubblocks++;
        // create Ss Forwarding sub block
        // allocate enough memory for forwading data
        TBuf8< SIZE_SS_FORWARDING + RMobilePhone::KMaxMobileTelNumberSize * 2 >
            dataForwarding( 0 );
        TIsiSubBlock forwardingSb(
            dataForwarding,
            SS_FORWARDING,
            EIsiSubBlockTypeId8Len8 ) ;
        dataForwarding.Append( aTypeOfNumber );

        //if reply time is not defined set to SS_UNDEFINED_TIME
        if ( 0 > aNoReplyTime )
            {
            aNoReplyTime = SS_UNDEFINED_TIME;
            }

        dataForwarding.Append( aNoReplyTime );
        dataForwarding.Append( number.Length() );
        dataForwarding.Append( 0x00 ); // Subaddress length
        // Copy 16 to 8
        TIsiUtility::CopyToBigEndian( number, dataForwarding );

        if ( ! iCheckInfoSubblock )
            {
            ssServiceReq.CopyData(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_REQ,
                forwardingSb.CompleteSubBlock( ) );
            }
        else
            {
            ssServiceReq.CopyData(
                ISI_HEADER_SIZE + SIZE_SS_SERVICE_REQ + SIZE_SS_SB_CHECK_INFO,
                forwardingSb.CompleteSubBlock( ) );
            }
        }

    // Set number of subblocks
    ssServiceReq.Set8bit(
        ISI_HEADER_SIZE + SS_SERVICE_REQ_OFFSET_SUBBLOCKCOUNT,
        numOfSubblocks );
    iCheckInfoSubblock = EFalse;

    //send message via phonet
    return iPhoNetSender->Send( ssServiceReq.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsServiceCompletedRespL
// Called when an SS_SERVICE_COMPLETED_RESP message has been received.
// This function is called in case of successful service response, which is
// completed with KErrNone ( unless original SS request was changed
// or initiated by Call Control )
//
// First check if this is a response to a password registration request.
// If not, the SS code defines which service (call forwarding, call barring,
// call waiting, identity service) is affected. The operation code defines if
// this is a response to a status interrogation or an action that sets the
// status.
//
// Some details on the response message structure for interrogations:
//
// The response to a Call Forwarding status interrogation (SS codes
// 2, 4, 21, 61, 62, 67) may contain a SS_GSM_FORWARDING_INFO sub block. If
// the sub block exists, it always contains SS_GSM_FORWARDING_FEATURE sub
// blocks. If the sub block does not exist, the result of a SS_STATUS_RESULT
// sub block is read. If also this does not exist, it means "Service Not Active"
// for the requested condition.
//
// The response to a Call Barring status interrogation (SS codes 33, 35, 330,
// 331, 332, 333, 353) may contain a SS_GSM_BSC_INFO sub block. If the sub block
// does not exist, the result of a SS_STATUS_RESULT sub block is read. If also this
// does not exist, it means "Service Not Active" for the requested condition.
//
// The response to a Call Waiting status interrogation (SS code 43) may contain
// a SS_GSM_BSC_INFO sub block. If the sub block does not exist, the result
// of a SS_STATUS_RESULT sub block is read. If also this does not exist, it means
// "Service Not Active".
//
// The response to a status interrogation of one of the Identity Services CLIP,
// COLP, COLR, CNAP (SS codes 30, 76, 77, 300) always contains a
// SS_STATUS_RESULT sub block.
//
// The response to a status interrogation of the Identity Service CLIR (SS
// code 31) may contain a SS_GSM_GENERIC_SERVICE_INFO sub block, which
// in turn always contains a SS_GSM_CLIR_INFO sub block. If the sub block
// does not exist, the result of a SS_STATUS_RESULT sub block is read.
//
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::SsServiceCompletedRespL
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {

    // read some commonly used values from response message:
    TPreprocessedSsServiceCompleteResponse response( aIsiMessage );
    // transaction id
    response.iTraId = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_SERVICE_COMPLETED_RESP_OFFSET_TRANSID );

    // number of included sub blocks
    response.iNumSubBlocks = aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + SS_SERVICE_COMPLETED_RESP_OFFSET_SUBBLOCKCOUNT );
    // operation
    response.iOperation = aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + SS_SERVICE_COMPLETED_RESP_OFFSET_OPERATION );
    // ss code
    response.iSsCode =  aIsiMessage.Get16bit(
            ISI_HEADER_SIZE + SS_SERVICE_COMPLETED_RESP_OFFSET_SSCODE );
    // basic service
    response.iBasicService =  aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + SS_SERVICE_COMPLETED_RESP_OFFSET_BASICSERVICECODE );

TFLOGSTRING3("TSY: CMmSupplServMessHandler::SsServiceCompletedRespL number \
              of sub blocks: %d, operation: %d", response.iNumSubBlocks, response.iOperation );
OstTraceExt2( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSSERVICECOMPLETEDRESPL, "CMmSupplServMessHandler::SsServiceCompletedRespL;numSubBlocks=%hhu;operation=%hhu", response.iNumSubBlocks, response.iOperation );
TFLOGSTRING3("TSY: CMmSupplServMessHandler::SsServiceCompletedRespL ssCode: \
              %d, basicService: %d", response.iSsCode, response.iBasicService );
OstTraceExt2( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_SSSERVICECOMPLETEDRESPL, "CMmSupplServMessHandler::SsServiceCompletedRespL;ssCode=%hu;basicService=%hhu", response.iSsCode, response.iBasicService );

    if (
       ProcessIfSsRequestChanged( response ) ||         // original SS request is changed, or initiated by Call Control
       ProcessSsGsmPasswordRegistration( response ) ||  // password changed successfully
       ProcessIfSimpleCompletionPossible( response ) ||
       ProcessIfCallForwardingL( response ) ||           // call forwarding cases
       ProcessIfCallBarringL( response ) ||              // call barring cases
       ProcessIfCallWaitingL( response ) ||              // call waiting cases
       ProcessIfIdentityServices( response )            // identity services
       )
        {
        // response to SS request was expected and completed, reset iSsCode
        iSsCode = KSsCodeNoValue;
        }
    else
        {
        TFLOGSTRING2("TSY: CMmSupplServMessHandler::SsServiceCompletedRespL \
                      - unknown ssCode: %d", response.iSsCode );
OstTraceExt1( TRACE_NORMAL, DUP5_CMMSUPPLSERVMESSHANDLER_SSSERVICECOMPLETEDRESPL, "CMmSupplServMessHandler::SsServiceCompletedRespL;ssCode=%hu", response.iSsCode );
        }
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::HandleSsGsmForwardingSubBlockL
// Reads a SS_GSM_FORWARDING_INFO sub block and saves results in iCFList
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::HandleSsGsmForwardingSubBlockL
    (
    const TIsiReceiveC& aIsiMessage,  //received isi message
    TUint aSbStartOffset,           //Ss Gsm Forwarding info sub block offset
    TUint16 aSsCode                 //SS condition
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::HandleSsGsmForwardingSubBlockL - \
              SS condition: %d", aSsCode);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_HANDLESSGSMFORWARDINGSUBBLOCKL, "CMmSupplServMessHandler::HandleSsGsmForwardingSubBlockL;aSsCode=%hu", aSsCode );

    TUint sbStartOffset( aSbStartOffset );

    // get number of SS_GSM_FORWARDING_FEATURE sub blocks in this
    // SS_GSM_FORWARDING_INFO sub block
    TUint8 numSubBlocks( aIsiMessage.Get8bit(
            aSbStartOffset + SS_GSM_FORWARDING_INFO_OFFSET_SUBBLOCKCOUNT ) );

    // As there can be several sub blocks, we cannot use
    // FindSubBlockOffsetById. Using FindSubBlockOffsetByIndex instead.
    TInt subIndex = 1;

    // check all sublocks
    while ( subIndex <= numSubBlocks &&
            KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
                aSbStartOffset + SIZE_SS_GSM_FORWARDING_INFO,
                subIndex,
                EIsiSubBlockTypeId8Len8,
                sbStartOffset ) )
        {
        // get subblock ID
        TUint8 sbId = aIsiMessage.Get8bit( sbStartOffset );

        // if SS_GSM_FORWARDING_FEATURE
        if ( SS_GSM_FORWARDING_FEATURE == sbId )
            {

            // Extract ISI values from subblock
            TUint8 mmiBsc = aIsiMessage.Get8bit(
                sbStartOffset + SS_GSM_FORWARDING_FEATURE_OFFSET_BASICSERVICECODE );

            TUint8 ssStatus = aIsiMessage.Get8bit(
                sbStartOffset + SS_GSM_FORWARDING_FEATURE_OFFSET_SSSTATUS );

            TUint8 typeOfNumber = aIsiMessage.Get8bit(
                sbStartOffset + SS_GSM_FORWARDING_FEATURE_OFFSET_TYPEOFNUMBER );

            TUint8 noReplyTime = aIsiMessage.Get8bit(
                sbStartOffset + SS_GSM_FORWARDING_FEATURE_OFFSET_NOREPLYTIME );

            TUint8 offsetNumberLength( aIsiMessage.Get8bit(
                sbStartOffset + SS_GSM_FORWARDING_FEATURE_OFFSET_NUMBERLENGTH ) );

            //numberlength is unicode -> multiplicand by 2
            TPtrC8 number ( aIsiMessage.GetData(
                sbStartOffset + SS_GSM_FORWARDING_FEATURE_OFFSET_NUMBER,
                offsetNumberLength * 2 ) );

            // Convert ISI values to ETel values
            RMobilePhone::TMobilePhoneCFInfoEntryV1 infoEntry;

            //Condition is got from the main message structure
            MapCFConditionIsiToMm( aSsCode, &infoEntry.iCondition );
            MapBasicServiceCodeIsiToMm( mmiBsc, &infoEntry.iServiceGroup );
            MapCFStatusIsiToMm( ssStatus, &infoEntry.iStatus );
            MapTypeOfNumberIsiToMm(
                typeOfNumber,
                &infoEntry.iNumber.iTypeOfNumber,
                &infoEntry.iNumber.iNumberPlan );

            TBuf8<RMobilePhone::KMaxMobileTelNumberSize> number2( 0 );
            // Check if number is international number and numberLength is
            // bigger than zero. The length of the string does not have to be checked
            // since the original max length was 100. When the string was sent to ISA
            // side the '+' character was cut off and now we just put it back.
            if ( ( RMobilePhone::EInternationalNumber == infoEntry.iNumber.iTypeOfNumber )
                && ( 0x00 != number.Length() ) )
                {
                // add '+' character back to the string
                number2.Append( 0x00 );
                number2.Append( '+' );
                }
            //append tel number to number2
            number2.Append( number );

            TIsiUtility::CopyFromBigEndian( number2, infoEntry.iNumber.iTelNumber );
            MapTimeoutIsiToMm( noReplyTime, &infoEntry.iTimeout );

            // Save this entry
            iCFResults->AddEntryL( infoEntry );

            }//if SS_GSM_FORWARDING_FEATURE

        // uppdate indexcount
        subIndex++;

        } // while

    return;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::HandleBarringSsGsmBscInfoSubBlockL
// Reads a SS_GSM_BSC_INFO sub block (coming with response for a
// call barring status request) and saves results in iCBList
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::HandleBarringSsGsmBscInfoSubBlockL
        (
        const TIsiReceiveC &aIsiMessage,  //received isi message
        TUint aSbStartOffset,           //Ss Gsm Bsc info sub block offset
        TUint16 aSsCode         // service code (barring condition)
        )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::HandleBarringSsGsmBscInfoSubBlockL \
              - service code: %d", aSsCode);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_HANDLEBARRINGSSGSMBSCINFOSUBBLOCKL, "CMmSupplServMessHandler::HandleBarringSsGsmBscInfoSubBlockL;aSsCode=%hu", aSsCode );

    //get number of basic service codes in this SS_GSM_BSC_INFO sub block
    TUint8 numBsc( aIsiMessage.Get8bit(
        aSbStartOffset + SS_GSM_BSC_INFO_OFFSET_NUMBEROFBSC ));

    if( 0 < numBsc )
        {
        // there's no other way to get the BSCs than to extract
        // them directly from this array of 8-Bit unsigned integers

        // get the address and reserve the memory
        HBufC8* allBscs = aIsiMessage.GetData(
            aSbStartOffset + SS_GSM_BSC_INFO_OFFSET_BASICSERVICECODE,
            numBsc ).AllocL();

        for ( TInt i = 0; i < numBsc; i++ )
            {
            // Extract ISI values from subblock
            TUint8 bsc = ( *allBscs )[i];

            // Convert ISI values to ETel values
            RMobilePhone::TMobilePhoneCBInfoEntryV1 infoEntry;

            // this BSC info comes only for those service groups
            // for which barring is active
            infoEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
            MapCBConditionIsiToMm( aSsCode, &infoEntry.iCondition );
            TInt ret = MapBasicServiceCodeIsiToMm( bsc, &infoEntry.iServiceGroup );
            if ( KErrNone != ret )
                {
TFLOGSTRING2( "TSY: CMmSupplServMessHandler::HandleSsGsmBscInfoSubBlockL: \
               Unknown Basic Service Group = %d received. Skipping this entry.", bsc );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_HANDLEBARRINGSSGSMBSCINFOSUBBLOCKL, "CMmSupplServMessHandler::HandleBarringSsGsmBscInfoSubBlockL;Unknown Basic Service Group=%hhu", bsc );
                continue; // ignore this entry and go to the next one
                }

            // Save this entry
            iCBResults->AddEntryL( infoEntry );
            }

        // release memory
        delete allBscs;
        allBscs = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::HandleWaitingSsGsmBscInfoSubBlockL
// Reads a SS_GSM_BSC_INFO sub block (coming with response for a
// call waiting status request) and saves results in iCWList
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::HandleWaitingSsGsmBscInfoSubBlockL
    (
    const TIsiReceiveC &aIsiMessage,  //received isi message
    TUint aSbStartOffset            //Ss Gsm Bsc info sub block offset
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::HandleWaitingSsGsmBscInfoSubBlockL");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_HANDLEWAITINGSSGSMBSCINFOSUBBLOCKL, "CMmSupplServMessHandler::HandleWaitingSsGsmBscInfoSubBlockL" );

    //get number of basic service codes in this SS_GSM_BSC_INFO sub block
    TUint8 numBsc( aIsiMessage.Get8bit(
        aSbStartOffset + SS_GSM_BSC_INFO_OFFSET_NUMBEROFBSC ));

    if( 0 < numBsc )
        {
        // there's no other way to get the BSCs than to extract
        // them directly from this array of 8-Bit values

        // get the address and reserve the memory
        HBufC8* allBscs = aIsiMessage.GetData(
            aSbStartOffset + SS_GSM_BSC_INFO_OFFSET_BASICSERVICECODE,
            numBsc ).AllocL();

        for ( TInt i = 0; i < numBsc; i++ )
            {
            // Extract ISI values from subblock
            TUint8 bsc = ( *allBscs )[i];

            // Convert ISI values to ETel values
            RMobilePhone::TMobilePhoneCWInfoEntryV1 infoEntry;

            // this BSC info comes only for those service groups
            // for which waiting is active
            infoEntry.iStatus = RMobilePhone::ECallWaitingStatusActive;
            TInt ret = MapBasicServiceCodeIsiToMm( bsc, &infoEntry.iServiceGroup );
            if ( KErrNone != ret )
                {
TFLOGSTRING2( "TSY: CMmSupplServMessHandler::HandleSsGsmDataSubBlockL: \
               Unknown Basic Service Group = %d received. Skipping this entry.", bsc );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_HANDLEWAITINGSSGSMBSCINFOSUBBLOCKL, "CMmSupplServMessHandler::HandleWaitingSsGsmBscInfoSubBlockL;Unknown Basic Service Group=%hhu", bsc );
                continue; // ignore this entry and go to the next one
                }
            // Save this entry
            iCWResults->AddEntryL( infoEntry );
            }

        // release memory
        delete allBscs;
        allBscs = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsServiceFailedResp
// Reads a SS_SERVICE_FAILED_RESP and completes to client
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::SsServiceFailedResp
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsServiceFailedResp");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSSERVICEFAILEDRESP, "CMmSupplServMessHandler::SsServiceFailedResp" );

    TInt errorToClient( KErrGeneral );

    //sub block start offset
    TUint sbStartOffset( 0 );

    //if sub block exists
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetByIndex(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
        1,
        EIsiSubBlockTypeId8Len8,
        sbStartOffset ) )
        {
        TUint8 subBlockId = aIsiMessage.Get8bit( sbStartOffset );
TFLOGSTRING2("TSY: CMmSupplServMessHandler::SsServiceFailedResp - \
              sub block Id: %d", subBlockId);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_SSSERVICEFAILEDRESP, "CMmSupplServMessHandler::SsServiceFailedResp;subBlockId=%hhu", subBlockId );
        switch ( subBlockId )
            {
            case SS_GSM_INDICATE_PASSWORD_ERROR:
                {
                errorToClient = CMmStaticUtility::CSCauseToEpocError(
                    PN_SS,
                    subBlockId,
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_PASSWORD_ERROR_OFFSET_GUIDANCEINFO ) );
                break;
                }
            case SS_GSM_INDICATE_ERROR:
                {
                errorToClient = CMmStaticUtility::CSCauseToEpocError(
                    PN_SS,
                    subBlockId,
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_ERROR_OFFSET_ERRORCODE ) );
                break;
                }
            case SS_GSM_INDICATE_PROBLEM:
                {
                errorToClient = CMmStaticUtility::CSCauseToEpocError(
                    PN_SS,
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMTYPE ),
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMCODE ) );
                break;
                }
            case SS_GSM_INDICATE_MSG_ERROR:
                {
                errorToClient = CMmStaticUtility::CSCauseToEpocError(
                    PN_SS,
                    subBlockId,
                    aIsiMessage.Get8bit( sbStartOffset +
                        SS_GSM_INDICATE_MSG_ERROR_OFFSET_MESSAGEERRORCODE ) );
                break;
                }
            case SS_OTHER_ERROR:
                {
                //If cause value is KSsGsmSsNotAvailable (comes in SS_OTHER_ERROR sub block)
                //this indicates that there is not network coverage.
                TUint8 errorCode( aIsiMessage.Get8bit(
                    sbStartOffset + SS_OTHER_ERROR_OFFSET_ERRORCODE ) );

                if( SS_GSM_SS_NOT_AVAILABLE == errorCode )
                    {
                    errorToClient = CMmStaticUtility::EpocErrorCode(
                        KErrCouldNotConnect,
                        KErrGsmSMSNoNetworkService );
TFLOGSTRING("CMmSupplServMessHandler::SsServiceFailedRespL. \
             Cause: KSsGsmSsNotAvailable => No network coverage." );
OstTrace0( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_SSSERVICEFAILEDRESP, "CMmSupplServMessHandler::SsServiceFailedResp, Cause: KSsGsmSsNotAvailable => No network coverage" );
                    }
                else if( SS_RESOURCE_CONTROL_DENIED == errorCode )
                    {
                     if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
                         ISI_HEADER_SIZE + SIZE_SS_SERVICE_FAILED_RESP,
                         SS_SB_RESOURCE_CONTROL_INFO,
                         EIsiSubBlockTypeId8Len8,
                         sbStartOffset ) )
                         {
                         TUint8 dataLen( aIsiMessage.Get8bit(
                             sbStartOffset + SS_SB_RESOURCE_CONTROL_INFO_OFFSET_DATALENGTH ) );

                         TPtrC8 data( aIsiMessage.GetData(
                             sbStartOffset + SS_SB_RESOURCE_CONTROL_INFO_OFFSET_DATA,
                             dataLen ) );
                         // sw1, sw2 and result is inserted to SS_SB_RESOURCE_CONTROL_INFO
                         // by simatktsy and ther order from first byte is: sw1, sw2 and result
                         TUint8 sw1 = data[KSw1Index];
                         TUint8 sw2 = data[KSw2Index];
                         TUint8 result = data[KResultIndex];
                         errorToClient = CMmStaticUtility::MapSw1Sw2ToEpocError( 
                             sw1, 
                             sw2, 
                             result );
                         }
                     else
                         {
                         errorToClient = CMmStaticUtility::CSCauseToEpocError(
                            PN_SS,
                            subBlockId,
                            errorCode );
                         }
                    }
                else
                    {
                    // Translate error to epoc world
                    errorToClient = CMmStaticUtility::CSCauseToEpocError(
                        PN_SS,
                        subBlockId,
                        aIsiMessage.Get8bit(
                            sbStartOffset +SS_OTHER_ERROR_OFFSET_ERRORCODE ) );
                    }
                break;
                }
            default:
                {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::SsServiceFailedResp \
              - unknown sub block Id: %d", subBlockId);
OstTraceExt1( TRACE_NORMAL, DUP3_CMMSUPPLSERVMESSHANDLER_SSSERVICEFAILEDRESP, "CMmSupplServMessHandler::SsServiceFailedResp;unknown subBlockId=%hhu", subBlockId );
                // unknown error sub block
                errorToClient = KErrGeneral;
                break;
                }
            }
        }

    // complete to SOS layer
    CompleteFailedSSRequest( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_SERVICE_FAILED_RESP_OFFSET_TRANSID ),
        errorToClient );
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsServiceNotSupportedResp
// Reads a SS_SERVICE_NOT_SUPPORTED_RESP and completes to client
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::SsServiceNotSupportedResp
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsServiceNotSupportedResp");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSSERVICENOTSUPPORTEDRESP, "CMmSupplServMessHandler::SsServiceNotSupportedResp" );

    //create ss service not supported response message
    // complete to SOS layer
    CompleteFailedSSRequest( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_SERVICE_NOT_SUPPORTED_RESP_OFFSET_TRANSID ),
        KErrNotSupported );

    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::CompleteFailedSSRequest
// Completes a failed request to SOS layer. The decision which request(s)
// to complete is based on the response message's transaction ID.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::CompleteFailedSSRequest
    (
    TUint8 aTransactionId, // the response's transaction ID
    TInt aError            // Symbian error code
    )
    {
TFLOGSTRING3("TSY: CMmSupplServMessHandler::CompleteFailedSSRequest - \
              traId: %d, error code: %d", aTransactionId, aError);
OstTraceExt2( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_COMPLETEFAILEDSSREQUEST, "CMmSupplServMessHandler::CompleteFailedSSRequest;aTransactionId=%hhu;aError=%d", aTransactionId, aError );

    TBool isExpectedMessage = ETrue;
    switch ( aTransactionId )
        {
        case ESSOperationTypeGetCallForwardingStatus:
            {
            iMessageRouter->Complete(
                EMobilePhoneGetCallForwardingStatusPhase1,
                aError );
            break;
            }
        case ESSOperationTypeSetCallForwardingStatus:
            {
            iMessageRouter->Complete(
                EMobilePhoneSetCallForwardingStatus,
                aError );
            break;
            }
        case ESSOperationTypeGetCallBarringStatus:
            {
            iMessageRouter->Complete(
                EMobilePhoneGetBarringStatusPhase1,
                aError );
            break;
            }
        case ESSOperationTypeSetCallBarringStatus:
            {
            iMessageRouter->Complete(
                EMobilePhoneSetCallBarringStatus,
                aError );
            break;
            }
        case ESSOperationTypeGetCallWaitingStatus:
            {
            iMessageRouter->Complete(
                EMobilePhoneGetWaitingStatusPhase1,
                aError );
            break;
            }
        case ESSOperationTypeSetCallWaitingStatus:
            {
            iMessageRouter->Complete(
                EMobilePhoneSetCallWaitingStatus,
                aError );
            break;
            }
        case ESSOperationTypeSetSSPassword:
            {
            iMessageRouter->Complete(
                EMobilePhoneSetSSPassword,
                aError );
            break;
            }
        case ESSOperationTypeGetIdentityServiceStatus:
            {
            iMessageRouter->Complete(
                EMobilePhoneGetIdentityServiceStatus,
                aError );
            break;
            }
        case ESSOperationTypeSendNetworkServiceRequest:
            {
            iMessageRouter->Complete(
                EMobilePhoneSendNetworkServiceRequest,
                aError );
            break;
            }
        case ESSOperationTypeSendNetworkServiceRequestNoFdnCheck:
            {
            iMessageRouter->Complete(
                EMobilePhoneSendNetworkServiceRequestNoFdnCheck,
                aError );
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::CompleteFailedSSRequest \
              - unknown traId: %d", aTransactionId );
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_COMPLETEFAILEDSSREQUEST, "CMmSupplServMessHandler::CompleteFailedSSRequest;aTransactionId=%hhu", aTransactionId );
            // ignore unknown response; do nothing
            isExpectedMessage = EFalse;
            break;
            }
        }
        if ( isExpectedMessage )
            {
            // response was expected and completed, reset iSsCode
            iSsCode = KSsCodeNoValue;
            }
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsGsmBarringReq
// Constructs a SS_SERVICE_REQ ISI message for a call barring request. Always
// includes gsm call barring password.
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::SsGsmBarringReq
    (
    TUint8 aTransactionId,            //transaction id
    TUint8 aOperation,                //operation
    TUint16 aMmiSsCode,               //MMI ss code
    TDesC16 const* aOldPassword,      //old password
    TDesC16 const& aNewPassword,      //new password
    TDesC16 const& aVerifyPassword,   //new password for verification
    TUint8 aMmiBasicServiceCode       //MMI basic service code
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsGsmBarringReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSGSMBARRINGREQ, "CMmSupplServMessHandler::SsGsmBarringReq" );

    TUint8 numOfSubblocks( 0 );
    TInt subblockStart = ISI_HEADER_SIZE + SIZE_SS_SERVICE_REQ;

    //create ss service request message and set it to point send buffer
    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );

    if ( aOldPassword->Length()
        || aNewPassword.Length()
        || aVerifyPassword.Length() )
        {
        numOfSubblocks++;

        /*
        password string handling:

            create buffer for password data
            if there is old password
                append old password to password data buffer
            else
                append eight zeroes to password data buffer

            if there is new password
                append new password to password buffer
            else
                append eight zeroes to password data buffer

            if there is new password2
                append new password2  to password buffer
            else
                append eight zeroes to password data buffer

            add Ss Gsm Password sub block to the send buffer
        */

        TBuf8<SIZE_SS_GSM_PASSWORD> dataPasswords( 0 );

        TIsiSubBlock passwordsSb(
            dataPasswords,
            SS_GSM_PASSWORD,
            EIsiSubBlockTypeId8Len8 );

        if ( 0 < aOldPassword->Length() )
            {
            // Copy 16 to 8
            TIsiUtility::CopyToBigEndian( *aOldPassword, dataPasswords );
            }
        else
            {
            dataPasswords.AppendFill( 0x00, 8 );
            }

        if ( 0 < aNewPassword.Length() )
            {
            // Copy 16 to 8
            TIsiUtility::CopyToBigEndian( aNewPassword, dataPasswords );
            }
        else
            {
            dataPasswords.AppendFill( 0x00, 8 );
            }

        if ( 0 < aVerifyPassword.Length() )
            {
            // Copy 16 to 8
            TIsiUtility::CopyToBigEndian( aVerifyPassword, dataPasswords );
            }
        else
            {
            dataPasswords.AppendFill( 0x00, 8 );
            }

        //add password subblock to isi message
        isimsg.CopyData( subblockStart, passwordsSb.CompleteSubBlock() );
        subblockStart += SIZE_SS_GSM_PASSWORD;
        }

    //create ss service request message
    TBuf8<KDataAppendToServiceReqHeaderInBytes> data( 0 );
    data.Append( aTransactionId ); //transaction id

    // SAT: ETEL Multimode API usage for SAT has been
    // implemented by S60
    data.Append( SS_SERVICE_REQ );

    data.Append( aOperation );            //operation
    data.Append( aMmiBasicServiceCode );  //mmi basic service code

    //split aMmiSsCode to two TBuf8 ( data )
    data.Append( ( aMmiSsCode & 0xFF00 ) >> 8 );// first byte
    data.Append( ( aMmiSsCode & 0xFF ) );       // second byte

    // Custom Tsy's SsAdditionalInfoNotification needs this
    data.Append( SS_SEND_ADDITIONAL_INFO );

    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SS );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    // SS_GSM_SIMATK_INFO must be added to indicate SS server
    // if FDN check must not be done
    if ( iCheckInfoSubblock )
        {
        numOfSubblocks++;

        // create subblock SS_SB_CHECK_INFO
        TBuf8<SIZE_SS_SB_CHECK_INFO>sbData( 0 );
        TIsiSubBlock ssCheckInfoSb(
            sbData,
            SS_SB_CHECK_INFO,
            EIsiSubBlockTypeId8Len8 );

        if ( iFdnCheck )
            {
            sbData.Append( SS_NO_FDN_CHECK_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_FDN_CHECK_SUPPRESS );
            }

        if ( iResourceControlSuppress )
            {
            sbData.Append( SS_RESOURCE_CONTROL_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_NO_RESOURCE_CONTROL_SUPPRESS );
            }
        iResourceControlSuppress = EFalse;

        // add the subblock
        isimsg.CopyData( subblockStart, ssCheckInfoSb.CompleteSubBlock( ) );

        iCheckInfoSubblock = EFalse;
        }

    isimsg.Set8bit(
        ISI_HEADER_SIZE + SS_SERVICE_REQ_OFFSET_SUBBLOCKCOUNT,
        numOfSubblocks );
    //send message via phonet
    return iPhoNetSender->Send( isimsg.Complete( ) );
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsGsmWaitingReq
// Constructs a SS_SERVICE_REQ ISI message for Call Waiting request.
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::SsGsmWaitingReq
    (
    TUint8 aTransactionId,      //transaction id
    TUint8 aOperation,          //operation
    TUint16 aMmiSsCode,         //MMI ss code
    TUint8 aMmiBasicServiceCode //MMI basic service code
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsGsmWaitingReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSGSMWAITINGREQ, "CMmSupplServMessHandler::SsGsmWaitingReq" );

    TUint8 numOfSubblocks( 0 );
    TBuf8<8> data( 0 );
    data.Append( aTransactionId );
    data.Append( SS_SERVICE_REQ );
    data.Append( aOperation );
    data.Append( aMmiBasicServiceCode );

    //split aMmiSsCode to two TBuf8 ( data )
    data.Append( ( aMmiSsCode & 0xFF00 ) >> 8 );// first byte of aMmiSsCode
    data.Append( ( aMmiSsCode & 0xFF ) );       // second byte of aMmiSsCode

    // Custom Tsy's SsAdditionalInfoNotification needs this
    data.Append( SS_SEND_ADDITIONAL_INFO );

    // Number of sub blocks
    data.Append( KSsPadding );

    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SS );
    isimsg.CopyData( ISI_HEADER_SIZE, data );
    // SS_GSM_SIMATK_INFO must be added to indicate SS server
    // if FDN check must not be done
    if ( iCheckInfoSubblock )
        {
        numOfSubblocks++;

        // create subblock SS_SB_CHECK_INFO
        TBuf8<SIZE_SS_SB_CHECK_INFO>sbData( 0 );
        TIsiSubBlock ssCheckInfoSb(
            sbData,
            SS_SB_CHECK_INFO,
            EIsiSubBlockTypeId8Len8 );

        if ( iFdnCheck )
            {
            sbData.Append( SS_NO_FDN_CHECK_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_FDN_CHECK_SUPPRESS );
            }

        if ( iResourceControlSuppress )
            {
            sbData.Append( SS_RESOURCE_CONTROL_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_NO_RESOURCE_CONTROL_SUPPRESS );
            }
        iResourceControlSuppress = EFalse;

        // add the subblock
        isimsg.CopyData(
            ISI_HEADER_SIZE + SIZE_SS_SERVICE_REQ,
            ssCheckInfoSb.CompleteSubBlock( ) );

        iCheckInfoSubblock = EFalse;
        }

    isimsg.Set8bit(
        ISI_HEADER_SIZE + SS_SERVICE_REQ_OFFSET_SUBBLOCKCOUNT,
        numOfSubblocks );
    return iPhoNetSender->Send( isimsg.Complete() );

    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsIdentityServiceReq
// Constructs a SS_SERVICE_REQ ISI message for Caller Identify (CLIP and CLIR),
// Connected Line Identify (COLP and COLR) and Caller Name Presentation (CNAP).
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::SsIdentityServiceReq
    (
    TUint8 aTransactionId,    //transaction id
    TUint16 aMmiSsCode        //MMI ss code
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsIdentityServiceReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSIDENTITYSERVICEREQ, "CMmSupplServMessHandler::SsIdentityServiceReq" );

    //create ss service request message and set it to point send buffer
    TIsiSend isimsg( iPhoNetSender->SendBufferDes() );

    TUint8 numOfSubblocks( 0 );
    //create ss service request message. Need only 5 bytes for data.
    TBuf8<8> data( 0 );
    //operation
    data.Append( aTransactionId);
    data.Append( SS_SERVICE_REQ );
    data.Append( SS_INTERROGATION );
    data.Append( SS_ALL_TELE_AND_BEARER ); //mmi basic service code

    //split aMmiSsCode to two TBuf8 ( data )
    data.Append( ( aMmiSsCode & 0xFF00 ) >> 8 );// first byte
    data.Append( ( aMmiSsCode & 0xFF ) );       // second byte

    // Custom Tsy's SsAdditionalInfoNotification needs this
    data.Append( SS_SEND_ADDITIONAL_INFO );

    //number of sub blocks
    data.Append( KSsPadding );

    //create the ISI message
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SS );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    // SS_GSM_SIMATK_INFO must be added to indicate SS server
    // if FDN check must not be done
    if ( iCheckInfoSubblock )
        {
        numOfSubblocks++;

        // create subblock SS_SB_CHECK_INFO
        TBuf8<SIZE_SS_SB_CHECK_INFO>sbData( 0 );
        TIsiSubBlock ssCheckInfoSb(
            sbData,
            SS_SB_CHECK_INFO,
            EIsiSubBlockTypeId8Len8 );

        if ( iFdnCheck )
            {
            sbData.Append( SS_NO_FDN_CHECK_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_FDN_CHECK_SUPPRESS );
            }

        if ( iResourceControlSuppress )
            {
            sbData.Append( SS_RESOURCE_CONTROL_SUPPRESS );
            }
        else
            {
            sbData.Append( SS_NO_RESOURCE_CONTROL_SUPPRESS );
            }
        iResourceControlSuppress = EFalse;

        // add the subblock
        isimsg.CopyData(
            ISI_HEADER_SIZE + SIZE_SS_SERVICE_REQ,
            ssCheckInfoSb.CompleteSubBlock( ) );

        iCheckInfoSubblock = EFalse;
        }

    isimsg.Set8bit(
        ISI_HEADER_SIZE + SS_SERVICE_REQ_OFFSET_SUBBLOCKCOUNT,
        numOfSubblocks );
    //send the SS_REQUEST ISI message via phonet
    return iPhoNetSender->Send( isimsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsGsmUssdSendReq
// Construct a SS_GSM_USSD_SEND_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::SsGsmUssdSendReq
    (
    TUint8 aTransactionId,  //transaction id
    TDesC8* aString         //USSD string
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsGsmUssdSendReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSGSMUSSDSENDREQ, "CMmSupplServMessHandler::SsGsmUssdSendReq" );

    //create ss service request message
    TBuf8<4> data( 0 ); //allocate memory for data
    data.Append( aTransactionId );

    data.Append( SS_GSM_USSD_SEND_REQ );

    data.Append( SS_GSM_USSD_COMMAND );
    // Number of Subblocks
    data.Append( 1 );

    //create the buffer for SB_PP subblock
    TBuf8<SIZE_SS_GSM_USSD_STRING + KMaxLengthOfUssdMessage>
        ssGsmStringBuf( 0 );
    TIsiSubBlock ssGsmStringSb(
        ssGsmStringBuf,
        SS_GSM_USSD_STRING,
        EIsiSubBlockTypeId8Len8 );

    // Coding info is always "Language unspecified = 0x0F"
    ssGsmStringBuf.Append( 0x0F );
    ssGsmStringBuf.Append( ( TUint8 )aString->Length() );
    ssGsmStringBuf.Append( *aString );

    TIsiSend isimsg ( iPhoNetSender->SendBufferDes() );
    isimsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SS );
    isimsg.CopyData( ISI_HEADER_SIZE, data );

    //add subblock to the message
    isimsg.CopyData(
        ISI_HEADER_SIZE + SIZE_SS_GSM_USSD_SEND_REQ,
        ssGsmStringSb.CompleteSubBlock() );

    //send message via phonet
    return iPhoNetSender->Send( isimsg.Complete() );
    }


// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsGsmUssdSendResp
// Breaks a SS_GSM_USSD_SEND_RESP ISI message. Completes Send Network Service
// Request.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::SsGsmUssdSendResp
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsGsmUssdSendResp");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSGSMUSSDSENDRESP, "CMmSupplServMessHandler::SsGsmUssdSendResp" );

    if ( ESSOperationTypeSendNetworkServiceRequest == aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_GSM_USSD_SEND_RESP_OFFSET_TRANSID ) )
        {
        // complete (no packed parameter)
        iMessageRouter->Complete(
            EMobilePhoneSendNetworkServiceRequest,
            KErrNone );
        }

    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsStatusInd
// Breaks a SS_STATUS_IND ISI message. Completes Send Network Service
// Request.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::SsStatusInd
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsStatusInd");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSSTATUSIND, "CMmSupplServMessHandler::SsStatusInd" );

    // Get number of subblocks
    TInt numOfSubBlocks( 0 );
    numOfSubBlocks = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_STATUS_IND_OFFSET_SUBBLOCKCOUNT );

    if( numOfSubBlocks > 0 )
        {
        TUint sbStartOffSet( 0 );
        RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;

        returnResult.iOpCode = KErrNone;
        returnResult.iAdditionalInfo.Zero();
        RMobilePhone::TMobilePhoneNotifySendSSOperation operation;

        TBool error( EFalse );

        // SS Return Error
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SS_STATUS_IND,
            SS_GSM_INDICATE_ERROR,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // get data
            TUint8 opCode( aIsiMessage.Get8bit(
                sbStartOffSet + SS_GSM_INDICATE_ERROR_OFFSET_ERRORCODE ) );

            returnResult.iOpCode = opCode;

            TUint8 additionalInfo( aIsiMessage.Get8bit(
                sbStartOffSet +
                SS_GSM_INDICATE_ERROR_OFFSET_ADDITIONALINDICATION ) );

            returnResult.iAdditionalInfo.Append( additionalInfo );

            operation = RMobilePhone::ESendSSReturnError;
            error = ETrue;
            }

        // SS Return Error
        else if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SS_STATUS_IND,
            SS_GSM_INDICATE_PROBLEM,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // get data
            TUint8 problemType( aIsiMessage.Get8bit(
                sbStartOffSet + SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMTYPE ) );
            returnResult.iOpCode = problemType;
            operation = RMobilePhone::ESendSSReturnError;

            TUint8 problemCode( aIsiMessage.Get8bit(
                sbStartOffSet + SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMCODE ) );

            returnResult.iAdditionalInfo.Append( problemCode );
            operation = RMobilePhone::ESendSSReturnError;
            error = ETrue;
            }

          // SS reject
        else if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SS_STATUS_IND,
            SS_OTHER_ERROR,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // get data
            TUint8 misc( aIsiMessage.Get8bit(
                sbStartOffSet + SS_OTHER_ERROR_OFFSET_ERRORCODE ) );

            returnResult.iOpCode = misc;

            operation = RMobilePhone::ESendSSReject;
            error = ETrue;
            }

           // SS Return Error
         if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_SS_STATUS_IND,
            SS_GSM_MM_RELEASED,
            EIsiSubBlockTypeId8Len8,
            sbStartOffSet ) )
            {
            // get data
            TUint8 cause( aIsiMessage.Get8bit(
                sbStartOffSet + SS_GSM_MM_RELEASED_OFFSET_CAUSE ) );
            returnResult.iAdditionalInfo.Append( cause );

            operation = RMobilePhone::ESendSSReturnError;
            error = ETrue;
            }

        // No need to complete notification without subblock
        if ( error )
            {
            //create package.
            CMmDataPackage package;

            //pack the data
            package.PackData( &returnResult, &operation );
            //complete the request
            #if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
            iMessageRouter->Complete(
                EMobilePhoneNotifySendNetworkServiceRequest,
                &package,
                KErrNone );
            #else
            iMessageRouter->Complete(
                EMobilePhoneNotifyAllSendNetworkServiceRequest,
                &package,
                KErrNone );
            #endif // NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32

            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::SsCompleteInd
// Breaks a SS_SERVICE_COMPLETED_IND ISI message. Completes Send Network
// Service Request.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::SsCompleteInd
    (
    const TIsiReceiveC &aIsiMessage //received isi message
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::SsCompleteInd");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SSCOMPLETEIND, "CMmSupplServMessHandler::SsCompleteInd" );

    TUint sbStartOffSet( 0 );
    RMobilePhone::TMobilePhoneSendSSRequestV3 returnResult;

    returnResult.iOpCode = KErrNone;
    returnResult.iAdditionalInfo.Zero();

    RMobilePhone::TMobilePhoneNotifySendSSOperation operation;

    // Get ss operation
    TInt ssOperation( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + SS_SERVICE_COMPLETED_IND_OFFSET_OPERATION ) );
    returnResult.iOpCode = ssOperation;

    // Send SS Return result
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_IND,
        SS_GSM_ADDITIONAL_INFO,
        EIsiSubBlockTypeId8Len8,
        sbStartOffSet ) )
        {
        // Get length of the data
        TUint8 len( aIsiMessage.Get8bit(
            sbStartOffSet + SS_GSM_ADDITIONAL_INFO_OFFSET_RETURNRESULTLEN ) );

        // Get data
        returnResult.iAdditionalInfo.Copy(aIsiMessage.GetData(
            sbStartOffSet + SS_GSM_ADDITIONAL_INFO_OFFSET_RETURNRESULT,
            static_cast<TInt> ( len ) ) );

        operation = RMobilePhone::ESendSSReturnResult;
        }

    //create package.
    CMmDataPackage package;

    //pack the data
    package.PackData( &returnResult, &operation );

    //complete the request
    #if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    iMessageRouter->Complete(
        EMobilePhoneNotifySendNetworkServiceRequest,
        &package,
        KErrNone );
    #else
        iMessageRouter->Complete(
        EMobilePhoneNotifyAllSendNetworkServiceRequest,
        &package,
        KErrNone );
    #endif // NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32
    }


// ############ static utility functions ###############

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapMobileServiceToBasicServiceCodeIsi
// Map multimode api mobile service to isi basic service code.
//
// Most ETel <=> ISI mappings are 1:1, but this method contains two exceptions:
// (EVoiceService,ETelephony) <=> KSsGsmTelephony
// (ECircuitDataService,EAllBearer) <=> KSsGsmAllBearer
//
// Returns KErrNone, KErrNotSupported or KErrArgument
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::MapMobileServiceToBasicServiceCodeIsi
    (
    RMobilePhone::TMobileService aMobileService,
    // multimode API mobile service
    TUint8* aIsiBasicServiceCode // isi basic service code
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapMobileServiceToBasicServiceCodeIsi \
              - basic service code: %d", aIsiBasicServiceCode);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPMOBILESERVICETOBASICSERVICECODEISI, "CMmSupplServMessHandler::MapMobileServiceToBasicServiceCodeIsi;aIsiBasicServiceCode=%hhu", *aIsiBasicServiceCode );

    TInt ret ( KErrNone );
    //get the right mobile service
    switch ( aMobileService )
        {
        case RMobilePhone::EAllServices:
            *aIsiBasicServiceCode = SS_ALL_TELE_AND_BEARER;
            break;
        case RMobilePhone::EAllTele:
            *aIsiBasicServiceCode = SS_GSM_ALL_TELE;
            break;
        case RMobilePhone::ETelephony:
            iVoiceServiceMappedToTelephony = EFalse;
            *aIsiBasicServiceCode = SS_GSM_TELEPHONY;
            break;
        case RMobilePhone::EVoiceService:
            iVoiceServiceMappedToTelephony = ETrue;
            *aIsiBasicServiceCode = SS_GSM_TELEPHONY;
            break;
        case RMobilePhone::EAllDataTele:
            *aIsiBasicServiceCode = SS_GSM_ALL_DATA_TELE;
            break;
        case RMobilePhone::EFaxService:
            *aIsiBasicServiceCode = SS_GSM_FACSIMILE;
            break;
        case RMobilePhone::EShortMessageService:
            *aIsiBasicServiceCode = SS_GSM_SMS;
            break;
        case RMobilePhone::EVoiceGroupCall:
            *aIsiBasicServiceCode = SS_GSM_VOICE_GROUP;
            break;
        case RMobilePhone::EAllTeleExcSms:
            *aIsiBasicServiceCode = SS_GSM_ALL_TELE_EXC_SMS;
            break;
        case RMobilePhone::EAllPlmnTele:
            *aIsiBasicServiceCode = SS_GSM_ALL_PLMN_SPEC_TELE_SERV;
            break;
        case RMobilePhone::EPlmnTele1:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_1;
            break;
        case RMobilePhone::EPlmnTele2:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_2;
            break;
        case RMobilePhone::EPlmnTele3:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_3;
            break;
        case RMobilePhone::EPlmnTele4:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_4;
            break;
        case RMobilePhone::EPlmnTele5:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_5;
            break;
        case RMobilePhone::EPlmnTele6:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_6;
            break;
        case RMobilePhone::EPlmnTele7:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_7;
            break;
        case RMobilePhone::EPlmnTele8:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_8;
            break;
        case RMobilePhone::EPlmnTele9:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_9;
            break;
        case RMobilePhone::EPlmnTeleA:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_A;
            break;
        case RMobilePhone::EPlmnTeleB:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_B;
            break;
        case RMobilePhone::EPlmnTeleC:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_C;
            break;
        case RMobilePhone::EPlmnTeleD:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_D;
            break;
        case RMobilePhone::EPlmnTeleE:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_E;
            break;
        case RMobilePhone::EPlmnTeleF:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_TELE_SERV_F;
            break;
        case RMobilePhone::EAllBearer:
            iCircuitServiceMappedToAllBearer = EFalse;
            *aIsiBasicServiceCode = SS_GSM_ALL_BEARER;
            break;
        case RMobilePhone::ECircuitDataService:
            iCircuitServiceMappedToAllBearer = ETrue;
            *aIsiBasicServiceCode = SS_GSM_ALL_BEARER;
            break;
        case RMobilePhone::EAllAsync:
            *aIsiBasicServiceCode = SS_GSM_ALL_ASYNC;
            break;
        case RMobilePhone::EAllSync:
            *aIsiBasicServiceCode = SS_GSM_ALL_SYNC;
            break;
        case RMobilePhone::ESyncData:
            *aIsiBasicServiceCode = SS_GSM_ALL_DATA_CIRCUIT_SYNC;
            break;
        case RMobilePhone::EAsyncData:
            *aIsiBasicServiceCode = SS_GSM_ALL_DATA_CIRCUIT_ASYNC;
            break;
        case RMobilePhone::EPadAccess:
            *aIsiBasicServiceCode = SS_GSM_ALL_PAD_ACCESS;
            break;
        case RMobilePhone::EAllPlmnBearer:
            *aIsiBasicServiceCode = SS_GSM_ALL_PLMN_SPEC_BEAR_SERV;
            break;
        case RMobilePhone::EPlmnBearerServ1:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_1;
            break;
        case RMobilePhone::EPlmnBearerServ2:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_2;
            break;
        case RMobilePhone::EPlmnBearerServ3:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_3;
            break;
        case RMobilePhone::EPlmnBearerServ4:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_4;
            break;
        case RMobilePhone::EPlmnBearerServ5:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_5;
            break;
        case RMobilePhone::EPlmnBearerServ6:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_6;
            break;
        case RMobilePhone::EPlmnBearerServ7:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_7;
            break;
        case RMobilePhone::EPlmnBearerServ8:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_8;
            break;
        case RMobilePhone::EPlmnBearerServ9:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_9;
            break;
        case RMobilePhone::EPlmnBearerServA:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_A;
            break;
        case RMobilePhone::EPlmnBearerServB:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_B;
            break;
        case RMobilePhone::EPlmnBearerServC:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_C;
            break;
        case RMobilePhone::EPlmnBearerServD:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_D;
            break;
        case RMobilePhone::EPlmnBearerServE:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_E;
            break;
        case RMobilePhone::EPlmnBearerServF:
            *aIsiBasicServiceCode = SS_GSM_PLMN_SPEC_BEAR_SERV_F;
            break;
        case RMobilePhone::EServiceUnspecified:
            *aIsiBasicServiceCode = SS_UNKNOWN_SERVICE;
            break;
        case RMobilePhone::EAuxVoiceService:
            *aIsiBasicServiceCode = SS_GSM_AUX_TELEPHONY;
            break;
        case RMobilePhone::EPacketDataService:
            *aIsiBasicServiceCode = SS_GSM_ALL_DATA_PACKET_SYNC;
            break;
        //following values are not supported in tsy as they are
        //duplicated.
        case RMobilePhone::EAltTele: //use EAuxVoiceService
        case RMobilePhone::EPacketData: //use EPacketDataService
        case RMobilePhone::EVoiceBroadcast:
        case RMobilePhone::EAllGprsBearer:
        case RMobilePhone::EAllDataExSms:
            ret = KErrNotSupported;
            break;
        default:
            // not expected.
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapMobileServiceToBasicServiceCodeIsi \
              - unknown mobile service: %d", aMobileService);
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPMOBILESERVICETOBASICSERVICECODEISI, "CMmSupplServMessHandler::MapMobileServiceToBasicServiceCodeIsi;aMobileService=%d", aMobileService );
            ret = KErrArgument;
            break;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapCFConditionMmToIsi
// Map multimode api call forwarding condition to isi call forwarding condition.
// Returns KErrNone or KErrNotSupported
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::MapCFConditionMmToIsi
    (
    RMobilePhone::TMobilePhoneCFCondition aMmCFCondition,
    // multimode api call forw. condition
    TUint16* aCFCondition // isi call forw. condition
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::MapCFConditionMmToIsi");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPCFCONDITIONMMTOISI, "CMmSupplServMessHandler::MapCFConditionMmToIsi" );
    TInt retVal( KErrNone );

    switch ( aMmCFCondition )
        {
        case RMobilePhone::ECallForwardingUnconditional:
            *aCFCondition = SS_GSM_FORW_UNCONDITIONAL;
            break;
        case RMobilePhone::ECallForwardingAllCases:
            *aCFCondition = SS_GSM_ALL_FORWARDINGS;
            break;
        case RMobilePhone::ECallForwardingAllConditionalCases:
            *aCFCondition = SS_GSM_ALL_COND_FORWARDINGS;
            break;
        case RMobilePhone::ECallForwardingBusy:
            *aCFCondition = SS_GSM_FORW_BUSY;
            break;
        case RMobilePhone::ECallForwardingNoReply:
            *aCFCondition = SS_GSM_FORW_NO_REPLY;
            break;
        case RMobilePhone::ECallForwardingNotReachable:
            *aCFCondition = SS_GSM_FORW_NO_REACH;
            break;
        case RMobilePhone::ECallForwardingUnspecified:
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCFConditionMmToIsi - \
              unknown call forward condition: %d", aCFCondition);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPCFCONDITIONMMTOISI, "CMmSupplServMessHandler::MapCFConditionMmToIsi;aCFCondition=%hu", *aCFCondition );
            retVal = KErrNotSupported;
            break;
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapTypeOfNumberIsiToMm
// Map isi type of number to multimode api type of number.
// Returns KErrNone or KErrArgument
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::MapTypeOfNumberIsiToMm
    (
    TUint8 aIsiTypeOfNumber, // isi number type
    RMobilePhone::TMobileTON* aMmTypeOfNumber, // number type
    RMobilePhone::TMobileNPI* aMmNumberingPlan // numbering plan
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::MapTypeOfNumberIsiToMm");
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPTYPEOFNUMBERISITOMM, "CMmSupplServMessHandler::MapTypeOfNumberIsiToMm" );

    TInt retVal( KErrNone );
    //get the right numbering plan
    switch ( aIsiTypeOfNumber & 0xF )
        // 0xF = 1111, above switch case checks four least significant bits
        {
        case SS_NBR_PLAN_UNKNOWN:
            *aMmNumberingPlan = RMobilePhone::EUnknownNumberingPlan;
            break;
        case SS_NBR_PLAN_ISDN_TELEPHONY:
            *aMmNumberingPlan = RMobilePhone::EIsdnNumberPlan;
            break;
        case SS_NBR_PLAN_DATA:
            *aMmNumberingPlan = RMobilePhone::EDataNumberPlan;
            break;
        case SS_NBR_PLAN_TELEX:
            *aMmNumberingPlan = RMobilePhone::ETelexNumberPlan;
            break;
        case SS_NBR_PLAN_NATIONAL:
            *aMmNumberingPlan = RMobilePhone::ENationalNumberPlan;
            break;
        case SS_NBR_PLAN_PRIVATE:
            *aMmNumberingPlan = RMobilePhone::EPrivateNumberPlan;
            break;
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapTypeOfNumberIsiToMm - \
              unknown number plan: %d", ( aIsiTypeOfNumber & 0xF ) );
#ifdef OST_TRACE_COMPILER_IN_USE // following lines flagged out just get rid of
                                 // compiler warning when trace compiler is not
                                 // in use.
         TUint8 tOn = aIsiTypeOfNumber & 0x0F; // parameter made just for tracing.
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPTYPEOFNUMBERISITOMM, "CMmSupplServMessHandler::MapTypeOfNumberIsiToMm;aIsiTypeOfNumber=%hhu", tOn );
#endif
            retVal = KErrArgument;
            break;
        }

    //Get the right type of number.
    //Masked with 0x70 as different highest bit (8th bit) is always 1.
    switch ( ( aIsiTypeOfNumber & 0x70 ) >> 4 )
        {
        case SS_NBR_TYPE_UNKNOWN:
            *aMmTypeOfNumber = RMobilePhone::EUnknownNumber;
            break;
        case SS_NBR_TYPE_INTERNATIONAL:
            *aMmTypeOfNumber = RMobilePhone::EInternationalNumber;
            break;
        case SS_NBR_TYPE_NATIONAL:
            *aMmTypeOfNumber = RMobilePhone::ENationalNumber;
            break;
        case SS_NBR_TYPE_NETWORK_SPECIFIC:
            *aMmTypeOfNumber = RMobilePhone::ENetworkSpecificNumber;
            break;
        case SS_NBR_TYPE_SUBSCRIBER:
            *aMmTypeOfNumber = RMobilePhone::ESubscriberNumber;
            break;
        case SS_NBR_TYPE_ALPHANUMERIC:
            *aMmTypeOfNumber = RMobilePhone::EAlphanumericNumber;
            break;
        case SS_NBR_TYPE_ABBREVIATED:
            *aMmTypeOfNumber = RMobilePhone::EAbbreviatedNumber;
            break;
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapTypeOfNumberIsiToMm - \
              unknown number type: %d", ( ( aIsiTypeOfNumber & 0x70  ) >> 4 ) );
#ifdef OST_TRACE_COMPILER_IN_USE // following lines flagged out just get rid of
                                 // compiler warning when trace compiler is not
                                 // in use.
            TUint8 tOn = ( ( aIsiTypeOfNumber & 0x70  ) >> 4 ); // Parameter just for tracing.
OstTraceExt1( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_MAPTYPEOFNUMBERISITOMM, "CMmSupplServMessHandler::MapTypeOfNumberIsiToMm;tOn=%hhu", tOn );
#endif
            retVal =  KErrArgument;
            break;
        }

    return retVal;
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapCFStatusIsiToMm
// Map call forwarding isi status to multimode api status.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapCFStatusIsiToMm
    (
    TUint8 aIsiSsStatus, // SS Status
    RMobilePhone::TMobilePhoneCFStatus* aMmCFStatus // call forw. status
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCFStatusIsiToMm - SS status: %d",
              aIsiSsStatus);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPCFSTATUSISITOMM, "CMmSupplServMessHandler::MapCFStatusIsiToMm;aIsiSsStatus=%hhu", aIsiSsStatus );

    if ( ( 0 == ( aIsiSsStatus & ( SS_GSM_ACTIVE + SS_GSM_REGISTERED +
        SS_GSM_PROVISIONED + SS_GSM_QUIESCENT ) ) ) &&
        ( 0 < aIsiSsStatus ) )
        // no valid status code found
        {
        *aMmCFStatus = RMobilePhone::ECallForwardingStatusUnknown;
        }
    else if ( 0 == ( aIsiSsStatus & SS_GSM_PROVISIONED ) )
        //not SS_GSM_PROVISIONED
        {
        *aMmCFStatus = RMobilePhone::ECallForwardingStatusNotProvisioned;
        }
    else if ( 0 == ( aIsiSsStatus & SS_GSM_REGISTERED ) )
        //not SS_GSM_REGISTERED
        {
        *aMmCFStatus = RMobilePhone::ECallForwardingStatusNotRegistered;
        }
    else if ( (0 == (aIsiSsStatus & SS_GSM_ACTIVE )) ||
        (aIsiSsStatus & SS_GSM_QUIESCENT ) )
        // not SS_GSM_ACTIVE or SS_GSM_QUIESCENT
        {
        *aMmCFStatus = RMobilePhone::ECallForwardingStatusNotActive;
        }
    else if ( aIsiSsStatus & SS_GSM_ACTIVE )
        // SS_GSM_ACTIVE
        {
        *aMmCFStatus = RMobilePhone::ECallForwardingStatusActive;
        }
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapCWStatusIsiToMm
// Map call waiting isi status to multimode api status.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapCWStatusIsiToMm
    (
    TUint8 aIsiSsStatus, // SS Status
    RMobilePhone::TMobilePhoneCWStatus* aMmCWStatus // call waiting status
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::MapCWStatusIsiToMm");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPCWSTATUSISITOMM, "CMmSupplServMessHandler::MapCWStatusIsiToMm" );

    if ( ( 0 == ( aIsiSsStatus & ( SS_GSM_ACTIVE + SS_GSM_REGISTERED +
        SS_GSM_PROVISIONED + SS_GSM_QUIESCENT ) ) ) &&
        ( 0 < aIsiSsStatus ) )
        // no valid status code found
        {
        *aMmCWStatus = RMobilePhone::ECallWaitingStatusUnknown;
        }
    else if ( 0 == ( aIsiSsStatus & SS_GSM_PROVISIONED ) )
        //not SS_GSM_PROVISIONED
        {
        *aMmCWStatus = RMobilePhone::ECallWaitingStatusNotProvisioned;
        }
    else if ( (0 == (aIsiSsStatus & SS_GSM_ACTIVE )) ||
        (aIsiSsStatus & SS_GSM_QUIESCENT ) )
        // not SS_GSM_ACTIVE or SS_GSM_QUIESCENT
        {
        *aMmCWStatus = RMobilePhone::ECallWaitingStatusNotActive;
        }
    else if ( aIsiSsStatus & SS_GSM_ACTIVE )
        // SS_GSM_ACTIVE
        {
        *aMmCWStatus = RMobilePhone::ECallWaitingStatusActive;
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapBasicServiceCodeIsiToMm
// Map isi basic service code to multimode api basic service code.
//
// Most ETel <=> ISI mappings are 1:1, but this method contains two exceptions:
// (EVoiceService,ETelephony) <=> KSsGsmTelephony
// (ECircuitDataService,EAllBearer) <=> KSsGsmAllBearer
//
// Returns KErrNone or KErrGeneral
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::MapBasicServiceCodeIsiToMm
    (
    TUint8 aIsiBasicServiceCode, // isi basic service code
    RMobilePhone::TMobileService* aMmBasicServiceCode
    // multimode api basic service code
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapBasicServiceCodeIsiToMm - \
              isi basic service code: %d", aIsiBasicServiceCode);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPBASICSERVICECODEISITOMM, "CMmSupplServMessHandler::MapBasicServiceCodeIsiToMm;aIsiBasicServiceCode=%hhu", aIsiBasicServiceCode );

    TInt ret( KErrNone );

    switch ( aIsiBasicServiceCode )
        {
        case SS_ALL_TELE_AND_BEARER:
            *aMmBasicServiceCode = RMobilePhone::EAllServices;
            break;
        case SS_GSM_ALL_TELE:
            *aMmBasicServiceCode = RMobilePhone::EAllTele;
            break;
        case SS_GSM_TELEPHONY:
            if ( iVoiceServiceMappedToTelephony )
                {
                *aMmBasicServiceCode = RMobilePhone::EVoiceService;
                iVoiceServiceMappedToTelephony = EFalse;
                }
            else
                {
                *aMmBasicServiceCode = RMobilePhone::ETelephony;
                }
            break;
        case SS_GSM_ALL_DATA_TELE:
            *aMmBasicServiceCode = RMobilePhone::EAllDataTele;
            break;
        case SS_GSM_FACSIMILE:
            *aMmBasicServiceCode = RMobilePhone::EFaxService;
            break;
        case SS_GSM_SMS:
            *aMmBasicServiceCode = RMobilePhone::EShortMessageService;
            break;
        case SS_GSM_VOICE_GROUP:
            *aMmBasicServiceCode = RMobilePhone::EVoiceGroupCall;
            break;
        case SS_GSM_ALL_TELE_EXC_SMS:
            *aMmBasicServiceCode = RMobilePhone::EAllTeleExcSms;
            break;
        case SS_GSM_ALL_PLMN_SPEC_TELE_SERV:
            *aMmBasicServiceCode = RMobilePhone::EAllPlmnTele;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_1:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele1;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_2:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele2;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_3:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele3;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_4:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele4;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_5:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele5;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_6:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele6;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_7:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele7;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_8:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele8;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_9:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTele9;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_A:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTeleA;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_B:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTeleB;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_C:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTeleC;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_D:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTeleD;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_E:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTeleE;
            break;
        case SS_GSM_PLMN_SPEC_TELE_SERV_F:
            *aMmBasicServiceCode = RMobilePhone::EPlmnTeleF;
            break;
        case SS_GSM_AUX_TELEPHONY:
            *aMmBasicServiceCode = RMobilePhone::EAuxVoiceService;
            break;
        case SS_GSM_ALL_BEARER:
            if ( iCircuitServiceMappedToAllBearer )
                {
                *aMmBasicServiceCode = RMobilePhone::ECircuitDataService;
                iCircuitServiceMappedToAllBearer = EFalse;
                }
            else
                {
                *aMmBasicServiceCode = RMobilePhone::EAllBearer;
                }
            break;
        case SS_GSM_ALL_ASYNC:
            *aMmBasicServiceCode = RMobilePhone::EAllAsync;
            break;
        case SS_GSM_ALL_SYNC:
            *aMmBasicServiceCode = RMobilePhone::EAllSync;
            break;
        case SS_GSM_ALL_DATA_CIRCUIT_SYNC:
            *aMmBasicServiceCode = RMobilePhone::ESyncData;
            break;
        case SS_GSM_ALL_DATA_CIRCUIT_ASYNC:
            *aMmBasicServiceCode = RMobilePhone::EAsyncData;
            break;
        case SS_GSM_ALL_DATA_PACKET_SYNC:
            *aMmBasicServiceCode = RMobilePhone::EPacketDataService;
            break;
        case SS_GSM_ALL_PAD_ACCESS:
            *aMmBasicServiceCode = RMobilePhone::EPadAccess;
            break;
        case SS_GSM_ALL_PLMN_SPEC_BEAR_SERV:
            *aMmBasicServiceCode = RMobilePhone::EAllPlmnBearer;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_1:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ1;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_2:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ2;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_3:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ3;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_4:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ4;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_5:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ5;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_6:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ6;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_7:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ7;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_8:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ8;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_9:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServ9;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_A:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServA;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_B:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServB;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_C:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServC;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_D:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServD;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_E:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServE;
            break;
        case SS_GSM_PLMN_SPEC_BEAR_SERV_F:
            *aMmBasicServiceCode = RMobilePhone::EPlmnBearerServF;
            break;
        case SS_UNKNOWN_SERVICE:
            *aMmBasicServiceCode = RMobilePhone::EServiceUnspecified;
            break;
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapBasicServiceCodeIsiToMm \
              - unknown isi basic service code: %d", aIsiBasicServiceCode);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPBASICSERVICECODEISITOMM, "CMmSupplServMessHandler::MapBasicServiceCodeIsiToMm;unknown isi basic service code=%hhu", aIsiBasicServiceCode );
            ret = KErrGeneral; // this can't be KErrArgument because basic
                            // service code isn't usually given as argument
            break;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapCFConditionIsiToMm
// Map isi call forwarding condition to multimode api call forwarding condition
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapCFConditionIsiToMm
    (
    TUint16 aCFCondition, // isi call forw. condition
    RMobilePhone::TMobilePhoneCFCondition* aMmCFCondition
    // multimode api call forw. condition
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCFConditionIsiToMm - isi call \
              forward condition: %d", aCFCondition);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPCFCONDITIONISITOMM, "CMmSupplServMessHandler::MapCFConditionIsiToMm;aCFCondition=%hu", aCFCondition );

    //get the right condition code
    switch ( aCFCondition )
        {
        case SS_GSM_FORW_UNCONDITIONAL:
            *aMmCFCondition = RMobilePhone::ECallForwardingUnconditional;
            break;
        case SS_GSM_FORW_BUSY:
            *aMmCFCondition = RMobilePhone::ECallForwardingBusy;
            break;
        case SS_GSM_FORW_NO_REPLY:
            *aMmCFCondition = RMobilePhone::ECallForwardingNoReply;
            break;
        case SS_GSM_FORW_NO_REACH:
            *aMmCFCondition = RMobilePhone::ECallForwardingNotReachable;
            break;
        case SS_GSM_ALL_COND_FORWARDINGS:
            *aMmCFCondition = RMobilePhone::ECallForwardingAllConditionalCases;
            break;
        case SS_GSM_ALL_FORWARDINGS:
            *aMmCFCondition = RMobilePhone::ECallForwardingAllCases;
            break;
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCFConditionIsiToMm - unknown isi call forward condition: %d", aCFCondition);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPCFCONDITIONISITOMM, "CMmSupplServMessHandler::MapCFConditionIsiToMm;unknown isi call forward condition=%hu", aCFCondition );
            *aMmCFCondition = RMobilePhone::ECallForwardingUnspecified;
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapTypeOfNumberMmToIsi
// Map isi timeout value to ETel timeout value
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapTimeoutIsiToMm
    (
    TUint8 aISITimeout, // isi timeout
    TInt* aMmTimeout // multimode api timeout
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapTimeoutIsiToMm - isi timeout: %d",
              aISITimeout);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPTIMEOUTISITOMM, "CMmSupplServMessHandler::MapTimeoutIsiToMm;aISITimeout=%hhu", aISITimeout );

    if ( SS_UNDEFINED_TIME == aISITimeout )
        {
        *aMmTimeout = -1;
        }
    else
        {
        *aMmTimeout = aISITimeout;
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapOperationMmToIsi
// Map multimode api operation to isi operation.
// Returns KErrNone or KErrArgument
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::MapOperationMmToIsi
    (
    RMobilePhone::TMobilePhoneServiceAction aMmOperation,
    // multimode api service action
    TUint8* aIsiOperation // isi operation
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapOperationMmToIsi - multimode \
              api service action: %d", aMmOperation);
OstTrace1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPOPERATIONMMTOISI, "CMmSupplServMessHandler::MapOperationMmToIsi;aMmOperation=%d", aMmOperation );

    TInt ret( KErrNone );

    //get the right operation code
    switch ( aMmOperation )
        {
        case RMobilePhone::EServiceActionActivate:
            *aIsiOperation = SS_ACTIVATION;
            break;
        case RMobilePhone::EServiceActionDeactivate:
            *aIsiOperation = SS_DEACTIVATION;
            break;
        case RMobilePhone::EServiceActionRegister:
            *aIsiOperation = SS_REGISTRATION;
            break;
        case RMobilePhone::EServiceActionErase:
            *aIsiOperation = SS_ERASURE;
            break;
        case RMobilePhone::EServiceActionUnspecified:
        case RMobilePhone::EServiceActionInvoke:
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapOperationMmToIsi - \
              unknown multimode api service action: %d", aMmOperation);
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPOPERATIONMMTOISI, "CMmSupplServMessHandler::MapOperationMmToIsi;unknown multimode api service action=%d", aMmOperation );
            ret = KErrArgument;
            break;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapCBConditionIsiToMm
// Map isi call barring condition to multimode api call barring condition.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapCBConditionIsiToMm
    (
    TUint16 aCBCondition, // isi call barring condition
    RMobilePhone::TMobilePhoneCBCondition* aMmCBCondition
    // multimode api call barring condition
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCBConditionIsiToMm - isi call \
              barring condition: %d", aCBCondition);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPCBCONDITIONISITOMM, "CMmSupplServMessHandler::MapCBConditionIsiToMm;aCBCondition=%hu", aCBCondition );

    //get the right condition code
    switch ( aCBCondition )
        {
        case SS_GSM_BARR_ALL_IN:
            *aMmCBCondition = RMobilePhone::EBarAllIncoming;
            break;
        case SS_GSM_BARR_ALL_IN_ROAM:
            *aMmCBCondition = RMobilePhone::EBarIncomingRoaming;
            break;
        case SS_GSM_BARR_ALL_OUT:
            *aMmCBCondition = RMobilePhone::EBarAllOutgoing;
            break;
        case SS_GSM_BARR_OUT_INTER:
            *aMmCBCondition = RMobilePhone::EBarOutgoingInternational;
            break;
        case SS_GSM_BARR_OUT_INTER_EXC_HOME:
            *aMmCBCondition = RMobilePhone::EBarOutgoingInternationalExHC;
            break;
        case SS_GSM_ALL_BARRINGS:
            *aMmCBCondition = RMobilePhone::EBarAllCases;
            break;
        case SS_GSM_OUTGOING_BARR_SERV:
            *aMmCBCondition = RMobilePhone::EBarAllOutgoingServices;
            break;
        case SS_GSM_INCOMING_BARR_SERV:
            *aMmCBCondition = RMobilePhone::EBarAllIncomingServices;
            break;
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCBConditionIsiToMm - unknown isi call barring condition: %d", aCBCondition);
OstTraceExt1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPCBCONDITIONISITOMM, "CMmSupplServMessHandler::MapCBConditionIsiToMm;unknown isi call barring condition=%hu", aCBCondition );
            *aMmCBCondition = RMobilePhone::EBarUnspecified;
            break;
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapCBConditionMmToIsi
// Map multimode api call barring condition to isi call barring condition.
// Returns KErrNone or KErrNotSupported
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::MapCBConditionMmToIsi
    (
    RMobilePhone::TMobilePhoneCBCondition aMmCBCondition,
    // multimode api call barring condition
    TUint16* aCBCondition // isi call barring condition
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCBConditionMmToIsi - multimode \
              api call barring condition: %d", aMmCBCondition);
OstTrace1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPCBCONDITIONMMTOISI, "CMmSupplServMessHandler::MapCBConditionMmToIsi;aMmCBCondition=%d", aMmCBCondition );

    TInt ret( KErrNone );

    switch ( aMmCBCondition )
        {
        case RMobilePhone::EBarAllIncoming:
            *aCBCondition = SS_GSM_BARR_ALL_IN;
            break;
        case RMobilePhone::EBarIncomingRoaming:
            *aCBCondition = SS_GSM_BARR_ALL_IN_ROAM;
            break;
        case RMobilePhone::EBarAllOutgoing:
            *aCBCondition = SS_GSM_BARR_ALL_OUT;
            break;
        case RMobilePhone::EBarOutgoingInternational:
            *aCBCondition = SS_GSM_BARR_OUT_INTER;
            break;
        case RMobilePhone::EBarOutgoingInternationalExHC:
            *aCBCondition = SS_GSM_BARR_OUT_INTER_EXC_HOME;
            break;
        case RMobilePhone::EBarAllCases:
            *aCBCondition = SS_GSM_ALL_BARRINGS;
            break;
        case RMobilePhone::EBarAllOutgoingServices:
            *aCBCondition = SS_GSM_OUTGOING_BARR_SERV;
            break;
        case RMobilePhone::EBarAllIncomingServices:
            *aCBCondition = SS_GSM_INCOMING_BARR_SERV;
            break;
        case RMobilePhone::EBarUnspecified:
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCBConditionMmToIsi - \
              unknown multimode api call barring condition: %d", aMmCBCondition);
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPCBCONDITIONMMTOISI, "CMmSupplServMessHandler::MapCBConditionMmToIsi;unknown multimode api call barring condition=%d", aMmCBCondition );
            ret = KErrNotSupported;
            break;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapCBStatusIsiToMm
// Map isi call barring status to multimode api call barring status.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapCBStatusIsiToMm
    (
    TUint8 aIsiSsStatus, // isi status
    RMobilePhone::TMobilePhoneCBStatus* aMmCBStatus // multimode api status
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapCBStatusIsiToMm - isi status: %d",
              aIsiSsStatus);
OstTraceExt1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPCBSTATUSISITOMM, "CMmSupplServMessHandler::MapCBStatusIsiToMm;aIsiSsStatus=%hhu", aIsiSsStatus );

    //get the right status code
   if ( ( 0 == ( aIsiSsStatus & ( SS_GSM_ACTIVE + SS_GSM_REGISTERED +
        SS_GSM_PROVISIONED + SS_GSM_QUIESCENT ) ) ) &&
        ( 0 < aIsiSsStatus ) )
        // no valid status code found
        {
        *aMmCBStatus = RMobilePhone::ECallBarringStatusUnknown;
        }
    else if ( 0 == ( aIsiSsStatus & SS_GSM_PROVISIONED ) )
        //not SS_GSM_PROVISIONED
        {
        *aMmCBStatus = RMobilePhone::ECallBarringStatusNotProvisioned;
        }
    else if ( ( 0 == ( aIsiSsStatus & SS_GSM_ACTIVE ) ) ||
        (aIsiSsStatus & SS_GSM_QUIESCENT ) )
        // not SS_GSM_ACTIVE or SS_GSM_QUIESCENT
        {
        *aMmCBStatus = RMobilePhone::ECallBarringStatusNotActive;
        }
    else if ( aIsiSsStatus & SS_GSM_ACTIVE )
        // SS_GSM_ACTIVE
        {
        *aMmCBStatus = RMobilePhone::ECallBarringStatusActive;
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapIdentityServiceMmToIsi
// Map multimode api identity service to isi identity service.
// Returns KErrNone or KErrNotSupported
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::MapIdentityServiceMmToIsi
    (
    RMobilePhone::TMobilePhoneIdService aService,
    // multimode api identity service
    TUint16* aIdentityService // isi identity service
    )
    {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapIdentityServiceMmToIsi - \
              multimode api identity service: %d", aService);
OstTrace1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPIDENTITYSERVICEMMTOISI, "CMmSupplServMessHandler::MapIdentityServiceMmToIsi;aService=%d", aService );

    TInt ret( KErrNone );

    //get the right operation code
    switch ( aService )
        {
        case RMobilePhone::EIdServiceCallerPresentation:
            *aIdentityService = SS_GSM_CLIP;
            break;
        case RMobilePhone::EIdServiceCallerRestriction:
            *aIdentityService = SS_GSM_CLIR;
            break;
        case RMobilePhone::EIdServiceConnectedPresentation:
            *aIdentityService = SS_GSM_COLP;
            break;
        case RMobilePhone::EIdServiceConnectedRestriction:
            *aIdentityService = SS_GSM_COLR;
            break;
        case RMobilePhone::EIdServiceCallerName:
            *aIdentityService = SS_GSM_CNAP;
            break;
        case RMobilePhone::EIdServiceUnspecified:
        case RMobilePhone::EIdServiceCalledPresentation:
        default:
TFLOGSTRING2("TSY: CMmSupplServMessHandler::MapIdentityServiceMmToIsi \
              - unknown multimode api identity service: %d", aService);
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPIDENTITYSERVICEMMTOISI, "CMmSupplServMessHandler::MapIdentityServiceMmToIsi;unknown multimode api identity service=%d", aService );
            ret = KErrNotSupported;
            break;
        }
    return ret;
    }


// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapIdentityServiceStatusIsiToMm
// Map identity service isi status to multimode status.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapIdentityServiceStatusIsiToMm
    (
    TUint8 aIsiIdentityServiceStatus,
    // isi identity service status
    TUint8 aIsiClirOption,
    // isi clir option
    RMobilePhone::TMobilePhoneIdServiceStatus* aMmIdentityServiceStatus
    // multimode api identity service status
    )
    {
TFLOGSTRING3("TSY: CMmSupplServMessHandler::MapIdentityServiceStatusIsiToMm \
              - isi id service status: %d, isi clir operation: %d",
              aIsiIdentityServiceStatus, aIsiClirOption);
OstTraceExt2( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPIDENTITYSERVICESTATUSISITOMM, "CMmSupplServMessHandler::MapIdentityServiceStatusIsiToMm;aIsiIdentityServiceStatus=%hhu;aIsiClirOption=%hhu", aIsiIdentityServiceStatus, aIsiClirOption );

    if ( 0 == ( aIsiIdentityServiceStatus & SS_GSM_PROVISIONED ) )
        {
        // service not provisioned
        *aMmIdentityServiceStatus = RMobilePhone::EIdServiceNotProvisioned;
        }
    else if ( ( 0 == ( aIsiIdentityServiceStatus &
        ( SS_GSM_ACTIVE + SS_GSM_REGISTERED + SS_GSM_PROVISIONED +
        SS_GSM_QUIESCENT ) ) ) && ( 0 < aIsiIdentityServiceStatus ) )
        {
        // id service is unknown if all above status bit operations equal zero
        // and statusvalue is above zero
        *aMmIdentityServiceStatus = RMobilePhone::EIdServiceUnknown;
        }
    else
        {
        // identity service is an exception to other ss services:
        // if it is provisioned then it's also active
        switch ( aIsiClirOption )
            {
            case SS_GSM_CLI_PERMANENT:
                *aMmIdentityServiceStatus = RMobilePhone::
                    EIdServiceActivePermanent;
                break;
            case SS_GSM_DEFAULT_RESTRICTED:
                *aMmIdentityServiceStatus = RMobilePhone::
                    EIdServiceActiveDefaultRestricted;
                break;
            case SS_GSM_CLI_DEFAULT_ALLOWED:
                *aMmIdentityServiceStatus = RMobilePhone::
                    EIdServiceActiveDefaultAllowed;
                break;
            default:
                *aMmIdentityServiceStatus = RMobilePhone::EIdServiceUnknown;
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapIdentityServiceStatusIsiToMm
// This method packs a given text string consisting of 7-bit characters into
// 8 bit bytes in such a way that a 160-character message takes only 140 bytes
// of memory after the compression.
// Returns length of packed message in bytes.
// -----------------------------------------------------------------------------
//
TUint16 CMmSupplServMessHandler::GsmLibSmsPackMessage
    (
    TDes8& dest,     // packed message
    TDes8& src,      // unpacked message
    TUint byte_count // the number of bytes in source
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::GsmLibSmsPackMessage");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_GSMLIBSMSPACKMESSAGE, "CMmSupplServMessHandler::GsmLibSmsPackMessage" );

    TUint16 si( 0 );
    TUint16 di( 0 ); // Indexes
    TInt tmp_modulo; // Temporary; to improve efficiency

    tmp_modulo = di % 7;

    while( si < byte_count )
        {
        // Is this the last character of the message
        // If it is, just shift it to the right. Otherwise,
        // fill the rest of the byte with the bits of the next source character.
        if( ( byte_count - 1 ) == si )
            {
            TUint num2 = ( src[si] >> tmp_modulo );
            dest.Append( num2 );
            }
        else
            {
            TUint num2 = ( src[si] >> tmp_modulo ) |
                ( src[si + 1] << (7 - tmp_modulo ) );
            dest.Append( num2 );
            }
        di++;
        tmp_modulo = di % 7;

        // Check if the destination byte could take the entire source character.
        // In that case, the source character does not have to be divided and
        // the next source character can be taken.
        if( 0 == tmp_modulo )
            {
            si += 2;
            }
        else
            {
            si++;
            }
        }

    // Return the length of the coded message.
    return di;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::HandleError
// Handles CMmSupplServMessHandler's errors that comes via PhoNetReceiver
// RunError method.
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::HandleError
    (
    const TIsiReceiveC& /*aIsiMsg*/,    // Isi message
    TInt /*aError*/    // Error code
    )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::HandleError");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_HANDLEERROR, "CMmSupplServMessHandler::HandleError" );
    //none
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req
// Write call forwarding flags REL4 to UICC
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req()
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCWRITECALLFWDFLAGSREL4REQ, "CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req" );

    TBuf<RMobilePhone::KMaxMobileTelNumberSize> telNumber( 0 );

    TUint8 voiceStatus( iVoiceStatus );
    TUint8 faxStatus( iFaxStatus );
    TUint8 dataStatus( iDataStatus );
    TUint8 numberLength( iCFAddress.iTelNumber.Length() );
    TUint8 status( 0 );

    if ( 0 < numberLength )
        {
        status = 1; // Set divert
        }
    else
        {
        status = 0; // Cancel divert
        }

    switch ( iMobileService )
        {
        case RMobilePhone::EAllTele:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                telNumber.Copy( iCFAddress.iTelNumber );
                voiceStatus = status;
                faxStatus = status;
                }
            break;
            }
        case RMobilePhone::ETelephony:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                telNumber.Copy( iCFAddress.iTelNumber );
                voiceStatus = status;
                }
            else
                {
                telNumber.Copy( iCFTelNumberFromSim );
                numberLength = iCFTelNumberFromSim.Length();
                }
            break;
            }
        case RMobilePhone::EAllBearer:
            {
            telNumber.Copy( iCFTelNumberFromSim );
            numberLength = iCFTelNumberFromSim.Length();

            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                dataStatus = status;
                }

            break;
            }
        case RMobilePhone::EFaxService:
            {
            telNumber.Copy( iCFTelNumberFromSim );
            numberLength = iCFTelNumberFromSim.Length();

            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                faxStatus = status;
                }
            break;
            }
        case RMobilePhone::EAllServices: //cancel all diverts
            {
            if ( RMobilePhone::ECallForwardingAllCases == iMmCFCondition ||
                RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                telNumber.Copy( iCFAddress.iTelNumber );
                voiceStatus = status;
                dataStatus = status;
                faxStatus = status;
                }
            break;
            }
        case RMobilePhone::EAllSync:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                telNumber.Copy( iCFAddress.iTelNumber );
                dataStatus = status;
                }
            break;
            }
        case RMobilePhone::ESyncData:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                telNumber.Copy( iCFAddress.iTelNumber );
                dataStatus = status;
                }
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req - unknown call service: %d", iMobileService);
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_UICCWRITECALLFWDFLAGSREL4REQ, "CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req;iMobileService=%d", iMobileService );
            break;
            }
        }

TFLOGSTRING3("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req -  Number: %S, length: %d", &telNumber, numberLength);
TFLOGSTRING4("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req - VoiceStatus: %d, FaxStatus: %d, DataStatus: %d", voiceStatus, faxStatus, dataStatus);
OstTraceExt5( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_UICCWRITECALLFWDFLAGSREL4REQ, "CMmSupplServMessHandler::UiccWriteCallFwdFlagsRel4Req;telNumber=%S;numberLength=%hhu;voiceStatus=%hhu;faxStatus=%hhu;dataStatus=%hhu", telNumber, numberLength, voiceStatus, faxStatus, dataStatus );

    // Status of the call forward unconditional indicator
    TUint8 cfuIndicatorStatus( 0 );
    cfuIndicatorStatus = voiceStatus; // 1st bit is for voice
    // 2nd bit is for fax
    cfuIndicatorStatus = cfuIndicatorStatus | ( faxStatus << 1 );
    // 3rd bit is for data
    cfuIndicatorStatus = cfuIndicatorStatus | ( dataStatus << 2 );

    // Convert number to BCD format
    TBuf8<RMobilePhone::KMaxMobileTelNumberSize> bcdNumberToSim( 0 );
    TUint8 length( telNumber.Length() );
    // Two byte are combined to on bytes, divide by 2
    TUint numOfBytes( length >> 1 );
    TUint8 i( 0 );
    TUint8 j( 0 );
    // Loop all the numbers and convert to semi-octets
    for ( i = 0; i < numOfBytes; i++, j+= 2 )
        {
        TUint8 lsb( telNumber[j] & 0x0F );
        TUint8 msb( telNumber[j+1] & 0x0F );
        bcdNumberToSim.Append( ( ( msb << 4 ) | lsb ) );
        }

    // If length is odd, add last byte
    if ( length % 2 )
        {
        bcdNumberToSim.Append( ( 0x0F << 4 )| ( telNumber[j] & 0x0F ) );
        }
    // TON & NPI
    TUint8 tonAndNpi( 0x80 ); // 1st bit is 1
    // Bits 5-7 are for TON
    tonAndNpi = tonAndNpi | ( ( iCFAddress.iTypeOfNumber & 0x07 ) << 4 );
    // Bits 1-4 are for NPI
    tonAndNpi = tonAndNpi | ( iCFAddress.iNumberPlan & 0x0F );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteCallFwdFlagsRel4;
    // Update CFU indicator status, length of BCD number, TON and NPI,
    // dialling number
    params.dataOffset = 1;
    params.record = 1;
    params.fileId = KElemFileCallFwdIndicationStatus;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    // File data to be updated.
    TBuf8<KFileDataLength> fileDataBuf;
    // CFU indicator status
    fileDataBuf.Append( cfuIndicatorStatus );
    // Length of bytes of the following two data items
    length = bcdNumberToSim.Length() + 1;
    fileDataBuf.Append( length );
    // TON & NPI
    fileDataBuf.Append( tonAndNpi );
    // CFU indicator status
    fileDataBuf.Append( bcdNumberToSim );
    params.fileData.Append( fileDataBuf );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq
// Write call forwarding flags CPHS to UICC
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq()
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCWRITECALLFWDFLAGSCPHSREQ, "CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq" );

    TUint8 voiceLine1( iVoiceLine1 );
    TUint8 voiceLine2( iVoiceLine2 );
    TUint8 faxFlag( iFax );
    TUint8 dataFlag( iData );

    TUint8 numberLength = iCFAddress.iTelNumber.Length();
    TUint8 status( 0 );

    if ( 0 < numberLength )
        {
        status = 0x0A; // set divert
        }
    else
        {
        status = 0x05; // cancel divert
        }

    switch ( iMobileService )
        {
        case RMobilePhone::ETelephony:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                voiceLine1 = status;
                }
            break;
            }
        case RMobilePhone::EAllBearer:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                dataFlag = status;
                }
            break;
            }
        case RMobilePhone::EFaxService:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                faxFlag = status;
                }
            break;
            }
        case RMobilePhone::EAllServices:
            {
            if ( RMobilePhone::ECallForwardingAllCases == iMmCFCondition ||
                RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                voiceLine1 = status;
                dataFlag = status;
                faxFlag = status;
                }
            break;
            }
        case RMobilePhone::EAuxVoiceService:
            {
            if ( RMobilePhone::ECallForwardingUnconditional == iMmCFCondition )
                {
                voiceLine2 = status;
                }
            else if ( RMobilePhone::ECallForwardingAllCases == iMmCFCondition )
                {
                voiceLine2 = status;
                dataFlag = status;
                faxFlag = status;
                }
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq - unknown call service: %d", iMobileService);
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_UICCWRITECALLFWDFLAGSCPHSREQ, "CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq;iMobileService=%d", iMobileService );
            break;
            }
        }

TFLOGSTRING2("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq - VoiceLine1: %d", voiceLine1);
TFLOGSTRING2("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq - VoiceLine2: %d", voiceLine2);
TFLOGSTRING2("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq - Fax: %d", faxFlag);
TFLOGSTRING2("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq - Data: %d", dataFlag);
OstTraceExt4( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_UICCWRITECALLFWDFLAGSCPHSREQ, "CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq;voiceLine1=%hhu;voiceLine2=%hhu;faxFlag=%hhu;dataFlag=%hhu", voiceLine1, voiceLine2, faxFlag, dataFlag );

    // Stores CPHS indicator values to be used in notify
    // ECustomNotifyIccCallForwardingStatusChangeIPC
    // Stores CAPI CF indicator values
    iTCFIndicators.iMultipleSubscriberProfileID = RMmCustomAPI::KProfileIdentityOne;
    iTCFIndicators.iIndicator =
        ( voiceLine1 == 0x0A ? RMobilePhone::KCFUIndicatorVoice:
        RMobilePhone::KCFUIndicatorUnknown );

    iTCFIndicators.iIndicator |=
        ( voiceLine2 == 0x0A ? RMobilePhone::KCFUIndicatorAuxVoice:
        RMobilePhone::KCFUIndicatorUnknown );

    iTCFIndicators.iIndicator |=
        ( faxFlag == 0x0A ? RMobilePhone::KCFUIndicatorFax:
        RMobilePhone::KCFUIndicatorUnknown  );

    iTCFIndicators.iIndicator |=
        ( dataFlag == 0x0A ? RMobilePhone::KCFUIndicatorData:
        RMobilePhone::KCFUIndicatorUnknown );

TFLOGSTRING2("TSY: CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq - Indicator: 0x%x", iTCFIndicators.iIndicator );
OstTrace1( TRACE_NORMAL, DUP3_CMMSUPPLSERVMESSHANDLER_UICCWRITECALLFWDFLAGSCPHSREQ, "CMmSupplServMessHandler::UiccWriteCallFwdFlagsCPHSReq;iTCFIndicators.iIndicator=%x", iTCFIndicators.iIndicator );

    iTCFIndicators.iCFNumber.iTelNumber.Zero();

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteCallFwdFlagsCphs;
    // We are updating voice Call forward unconditional flags ( byte = 1 )
    // and data/fax call forward unconditional flags ( byte = 2 )
    params.dataOffset = 0;
    params.fileId = KElemFileCallFwdFlagsCphs;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    // File data tu be updated.
    TBuf8<2> fileDataBuf;
    // Byte 1: Voice line 2 (bits 4-7) and Voice line 1 (bits 0-3)
    fileDataBuf.Append( ( voiceLine2 << 4 ) | ( voiceLine1 ) );
    // Byte 2: Data calls (bits 4-7) and Fax calls (bits 0-3)
    fileDataBuf.Append( ( dataFlag << 4 ) | ( faxFlag ) );

    params.fileData.Append( fileDataBuf );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccReadCallFwdFlagsRel4Req
// Read call forwarding flags REL 4
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccReadCallFwdFlagsRel4Req()
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccReadCallFwdFlagsRel4Req");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCREADCALLFWDFLAGSREL4REQ, "CMmSupplServMessHandler::UiccReadCallFwdFlagsRel4Req" );

    // At first try to read rel4 EF ( 6FCB )
    // If reading is not successful, then we try CPHS file ( 6F13 )
    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadCallFwdFlagsRel4;
    params.dataOffset = 0;
    params.fileId = KElemFileCallFwdIndicationStatus;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    params.record = 1;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccReadCallFwdFlagsCphsReq
// Read call forwarding flags CPHS
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccReadCallFwdFlagsCphsReq()
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccReadCallFwdFlagsCphsReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCREADCALLFWDFLAGSCPHSREQ, "CMmSupplServMessHandler::UiccReadCallFwdFlagsCphsReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadCallFwdFlagsCphs;
    params.dataOffset = 0;
    params.dataAmount = 2;
    params.fileId = KElemFileCallFwdFlagsCphs;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccHandleCallFwdFlagsResp
// Complete call forwarding flags
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::UiccHandleCallFwdFlagsResp(
    TInt aStatus,
    TInt aTrId,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccHandleCallFwdFlagsResp");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCHANDLECALLFWDFLAGSRESP, "CMmSupplServMessHandler::UiccHandleCallFwdFlagsResp" );

    CMmDataPackage dataPackage;
    CMmDataPackage customDataPackage;
    TBool rel4Exists( EFalse );
    TBool cphsExists( EFalse );

    // Read file data only if UICC status is OK
    if ( UICC_STATUS_OK == aStatus )
        {
        if ( ETrIdReadCallFwdFlagsRel4 == aTrId )
            {
            rel4Exists = ETrue;
            iVoiceStatus = aFileData[1] & 0x01;
            iFaxStatus = ( ( aFileData[1] & 0x02 ) >> 1 );
            iDataStatus = ( ( aFileData[1] & 0x04 ) >> 2 );

            // In case of inactive flags, remaining bytes may be set to 0xFF in SIM
            // Check the byte 3 for avoiding crash in that case.
            if ( iVoiceStatus || iFaxStatus || iDataStatus &&
                0xFF != aFileData[2] )
                {
                TUint8 numberLen( aFileData[2] );
                numberLen--; // decrease by one because of TON&NPI
                TBuf<20> tempNumber;
                tempNumber.Copy( aFileData.Mid( 4, numberLen ) );

                // Convert and copy to 8-bit format
                for ( TUint8 i( 0 ); i < numberLen; i++ )
                    {
                    iCFTelNumberFromSim.Append( tempNumber[i] & 0x0F );
                    iCFTelNumberFromSim.Append( ( tempNumber[i] & 0xF0 ) >> 4 );
                    }
                // Check if last byte is 0xF, in that case ignore it
                TUint8 index( iCFTelNumberFromSim.Length() );
                if ( 0x0F == iCFTelNumberFromSim[--index] )
                    {
                    iCFTelNumberFromSim.SetLength( index );
                    }
                }
            }
        else // CPHS case
            {
            iVoiceLine1 = aFileData[0] & 0x0F;
            iVoiceLine2 = ( aFileData[0] & 0xF0 ) >> 4;
            iFax = aFileData[1] & 0x0F;
            iData = ( aFileData[1] & 0xF0 ) >> 4;
            cphsExists = ETrue;
            }
        } // End of if ( UICC_STATUS_OK == aStatus )

    iTCFIndicators.iIndicator = RMobilePhone::KCFUIndicatorUnknown;
    iTCFIndicators.iCFNumber.iTelNumber.Zero();
    // Stores CAPI CF indicator values
    iTCFIndicators.iMultipleSubscriberProfileID =
        RMmCustomAPI::KProfileIdentityOne;

    if ( rel4Exists )
        {
        iTCFIndicators.iIndicator =
            ( iVoiceStatus ? RMobilePhone::KCFUIndicatorVoice:
            RMobilePhone::KCFUIndicatorUnknown );
        iTCFIndicators.iIndicator |=
            ( iFaxStatus ? RMobilePhone::KCFUIndicatorFax:
            RMobilePhone::KCFUIndicatorUnknown );
        iTCFIndicators.iIndicator |=
            ( iDataStatus ? RMobilePhone::KCFUIndicatorData:
            RMobilePhone::KCFUIndicatorUnknown );

        iTCFIndicators.iCFNumber.iTelNumber.Copy( iCFTelNumberFromSim );
        }
    else if ( cphsExists )  // CPHS
        {
        iTCFIndicators.iIndicator =
            ( iVoiceLine1 == 0x0A ? RMobilePhone::KCFUIndicatorVoice:
            RMobilePhone::KCFUIndicatorUnknown );

        iTCFIndicators.iIndicator |=
            ( iVoiceLine2 == 0x0A ? RMobilePhone::KCFUIndicatorAuxVoice:
            RMobilePhone::KCFUIndicatorUnknown );

        iTCFIndicators.iIndicator |=
            ( iFax == 0x0A ? RMobilePhone::KCFUIndicatorFax:
            RMobilePhone::KCFUIndicatorUnknown  );
        iTCFIndicators.iIndicator |=
            ( iData == 0x0A ? RMobilePhone::KCFUIndicatorData:
            RMobilePhone::KCFUIndicatorUnknown );
        }

    if ( iGetIccCallForwardingStatus ) // ECustomGetIccCallForwardingStatusIPC
        {
        iGetIccCallForwardingStatus = EFalse;

        // This package is used at the completion of IPCs:
        // ECustomGetIccCallForwardingStatusIPC and
        // ECustomNotifyIccCallForwardingStatusChangeIPC
        customDataPackage.PackData( &iTCFIndicators );

        // ICC Call forwarding status successfully read from the SIM
        // Complete the ECustomGetIccCallForwardingStatusIPC to SOS layer
        iMessageRouter->Complete(
            ECustomGetIccCallForwardingStatusIPC,
            &customDataPackage,
            KErrNone );
        }
    else if ( iNotifyIccCallForwardingStatus )
        {
        iNotifyIccCallForwardingStatus = EFalse;
        customDataPackage.PackData( &iTCFIndicators );

        iMessageRouter->Complete(
            ECustomNotifyIccCallForwardingStatusChangeIPC,
            &customDataPackage,
            KErrNone );

TFLOGSTRING("TSY: CMmSupplServMessHandler::SimCallFwdRespL - Check possible refresh status");
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_UICCHANDLECALLFWDFLAGSRESP, "CMmSupplServMessHandler::UiccHandleCallFwdFlagsResp - Check possible refresh status" );
        iMessageRouter->GetPhoneMessHandler()->CallForwFlagsCachingCompleted( aStatus );
        }
    else if ( !iGetCallForwardingNumber ) // EMobilePhoneSetCallForwardingStatus
        {
        if ( rel4Exists )
            {
            // Call forwarding status successfully read from the SIM
            // Write call forwarding number and indicator status
            // to the SIM
            UiccWriteCallFwdFlagsRel4Req();
            }
        else if ( cphsExists )
            {
            // Call forwarding status successfully read from the SIM
            // Write call forwarding number and indicator status
            // to the SIM
            UiccWriteCallFwdFlagsCPHSReq();
            }
        else
            {
            // Call forwarding status can't be read from the SIM
            // probably due to missing EF-CFIS file on the SIM.
            // Complete the SetCallForwardingStatus to SOS layer
            // always with KErrNone regardless of writing to SIM
            // failed or succeed
            iMessageRouter->Complete(
                EMobilePhoneSetCallForwardingStatus,
                KErrNone );
            }
        }
    else    // EMmTsyGetCallForwardingNumberIPC
        {
        iGetCallForwardingNumber = EFalse;
        dataPackage.PackData( &iCFTelNumberFromSim );

        if ( rel4Exists )
            {
            // Call forwarding number successfully read from the SIM
            // Complete the GetCallForwardingNumber to SOS layer
            iMessageRouter->Complete(
                EMmTsyGetCallForwardingNumberIPC,
                &dataPackage,
                KErrNone );
            }
        else
            {
            // Error reading call forwarding number from the SIM
            // probably because the SIM doesn't include EF-CFIS file.
            // Complete the GetCallForwardingNumber to SOS layer with
            // an error KErrGeneral
            iMessageRouter->Complete(
                EMmTsyGetCallForwardingNumberIPC,
                &dataPackage,
                KErrGeneral );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccReadVoiceMsgFlagsRel4Req
// Read voice message flags REL4
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccReadVoiceMsgFlagsRel4Req()
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccReadVoiceMsgFlagsRel4Req");
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_UICCREADVOICEMSGFLAGSREL4REQ, "CMmSupplServMessHandler::UiccReadVoiceMsgFlagsRel4Req" );

    // At first try to read rel4 EF ( 6FCA )
    // If reading is not successful, then we try CPHS file ( 6F11 )
    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadVoiceMsgFlagsRel4;
    params.dataOffset = 0;
    params.fileId = KElemFileMessageWaitingIndStatus; // EF MWIS
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_LINEAR_FIXED;
    params.record = 1;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccReadVoiceMsgFlagsResp
// Complete voice message flags
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::UiccReadVoiceMsgFlagsResp(
    TInt aTrId,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccReadVoiceMsgFlagsResp");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCREADVOICEMSGFLAGSRESP, "CMmSupplServMessHandler::UiccReadVoiceMsgFlagsResp" );

    TInt ret( KErrNone );
    CMmDataPackage dataPackage;

    if ( ETrIdReadVoiceMsgFlagsRel4 == aTrId )
        {
        iVMIndicators.iDisplayStatus = aFileData[0];
        iVMIndicators.iVoiceMsgs = aFileData[1];
        iVMIndicators.iFaxMsgs = aFileData[2];
        iVMIndicators.iEmailMsgs = aFileData[3];
        iVMIndicators.iOtherMsgs = aFileData[4];

        iMsgFlagType = EMsgFlagTypeRel4;
        }
    else // CPHS
        {
        // "0x0A" means active and "0x05" means inactive.
        iVMIndicators.iDisplayStatus =
            ( ( ( aFileData[0] & 0x0F ) == 0x0A ) ?
            RMobilePhone::KDisplayVoicemailActive : 0 );
        iVMIndicators.iDisplayStatus |=
            ( ( ( aFileData[0] >> 4 ) == 0x0A ) ?
            RMobilePhone::KDisplayAuxVoicemailActive : 0 );

        // Fax and data are optional, if exist file data is 2 bytes
        if ( 1 < aFileData.Length() )
            {
            iVMIndicators.iDisplayStatus  |=
                ( ( ( aFileData[1] & 0x0F ) == 0x0A ) ?
                RMobilePhone::KDisplayFaxActive : 0 );

            iVMIndicators.iDisplayStatus |=
                ( ( ( aFileData[1] >> 4 ) == 0x0A ) ?
                RMobilePhone::KDisplayDataActive : 0 );
            }
        // unable to determine message number in CPHS
        iVMIndicators.iVoiceMsgs = 0;
        iVMIndicators.iFaxMsgs = 0;
        iVMIndicators.iEmailMsgs = 0;
        iVMIndicators.iOtherMsgs = 0;

        iMsgFlagType = EMsgFlagTypeCphs;

        // Because not possible to determine amount of messages,
        // complete with error.
        ret = KErrCPHSInUseBySIM;
        }

        dataPackage.PackData( &iVMIndicators );
            iMessageRouter->Complete(
            EMobilePhoneGetIccMessageWaitingIndicators,
            &dataPackage,
            ret );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccReadVoiceMsgFlagsCphsReq
// Read voice message flags CPHS
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccReadVoiceMsgFlagsCphsReq()
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccHandleCallFwdFlagsResp");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCREADVOICEMSGFLAGSCPHSREQ, "CMmSupplServMessHandler::UiccReadVoiceMsgFlagsCphsReq" );

    // Reading of rel4 was not successful, we try CPHS file ( 6F11 )
    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdReadVoiceMsgFlagsCphs;
    params.dataOffset = 0;
    params.fileId = KElemFileVoiceMsgWaitingFlagsCphs;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccWriteVoiceMsgFlagsCPHSReq
// Write voice message flags CPHS
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccWriteVoiceMsgFlagsCPHSReq(
    const RMobilePhone::TMobilePhoneMessageWaitingV1& aMsgWaiting )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccWriteVoiceMsgFlagsCPHSReq");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCWRITEVOICEMSGFLAGSCPHSREQ, "CMmSupplServMessHandler::UiccWriteVoiceMsgFlagsCPHSReq" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteVoiceMsgFlagsCphs;
    params.dataOffset = 0;
    params.fileId = KElemFileVoiceMsgWaitingFlagsCphs;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    // In CPHS flag fields are coded as "0x0A" means active and "0x05" means
    // inactive.
    // Voice line 1 message waiting flag
    TUint8 voiceLine1( ( aMsgWaiting.iDisplayStatus &
        RMobilePhone::KDisplayVoicemailActive ) ? 0x0A : 0x05 );

    // Voice line 2 message waiting flag
    TUint8 voiceLine2( ( aMsgWaiting.iDisplayStatus &
         RMobilePhone::KDisplayAuxVoicemailActive ) ? 0x0A : 0x05 );

    // Fax message waiting flag
    TUint8 faxFlag( ( aMsgWaiting.iDisplayStatus &
        RMobilePhone::KDisplayFaxActive ) ? 0x0A : 0x05 );

    // Data message waiting flag
    TUint8 dataFlag( ( aMsgWaiting.iDisplayStatus &
        RMobilePhone::KDisplayDataActive ) ? 0x0A : 0x05 );

    // File data to be updated.
    TBuf8<2> fileDataBuf;
    // Byte 1: Voice line 2 (bits 4-7) and Voice line 1 (bits 0-3)
    fileDataBuf.Append( ( voiceLine2 << 4 ) | ( voiceLine1 & 0x0F ) );
    // Byte 2: Data calls (bits 4-7) and Fax calls (bits 0-3)
    fileDataBuf.Append( ( dataFlag << 4 ) | ( faxFlag & 0x0F ) );
    params.fileData.Append( fileDataBuf );

    iVMIndicators = aMsgWaiting;
    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::UiccWriteVoiceMsgFlagsRel4Req
// Write voice message flags REL4
// -----------------------------------------------------------------------------
//
TInt CMmSupplServMessHandler::UiccWriteVoiceMsgFlagsRel4Req(
    const RMobilePhone::TMobilePhoneMessageWaitingV1& aMsgWaiting )
    {
TFLOGSTRING("TSY: CMmSupplServMessHandler::UiccHandleCallFwdFlagsResp");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_UICCWRITEVOICEMSGFLAGSREL4REQ, "CMmSupplServMessHandler::UiccWriteVoiceMsgFlagsRel4Req" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccWriteLinearFixed params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = ETrIdWriteVoiceMsgFlagsRel4;
    params.record = 1;
    params.fileId = KElemFileMessageWaitingIndStatus;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.serviceType = UICC_APPL_UPDATE_LINEAR_FIXED;

    // File ID path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    // File data to be updated.
    TBuf8<5> fileDataBuf;
    // CFU indicator status
    fileDataBuf.Append( aMsgWaiting.iDisplayStatus );
    fileDataBuf.Append( aMsgWaiting.iVoiceMsgs);
    fileDataBuf.Append( aMsgWaiting.iFaxMsgs );
    fileDataBuf.Append( aMsgWaiting.iEmailMsgs );
    fileDataBuf.Append( aMsgWaiting.iOtherMsgs );
    params.fileData.Append( fileDataBuf );

    iVMIndicators = aMsgWaiting;
    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmSupplServMessHandler::MapTypeOfNumberMmToIsi
// This method maps Symbian OS number type to Domestic OS number type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::MapTypeOfNumberMmToIsi(
    RMobilePhone::TMobileTON aMmTypeOfNumber,
    RMobilePhone::TMobileNPI aMmNumberingPlan,
    TUint8& aIsiTypeOfNumber )
    {
TFLOGSTRING("TSY: CMmCallMessHandler::MapTypeOfNumberMmToIsi");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_MAPTYPEOFNUMBERMMTOISI, "CMmSupplServMessHandler::MapTypeOfNumberMmToIsi" );

     //get the right numbering plan
    switch ( aMmNumberingPlan )
        // isi numbering plan is included in bits 1-4 (lsb)
        {
        case RMobilePhone::EUnknownNumberingPlan:
            {
            aIsiTypeOfNumber = SS_NBR_PLAN_UNKNOWN;
            break;
            }
        case RMobilePhone::EIsdnNumberPlan:
            {
            aIsiTypeOfNumber = SS_NBR_PLAN_ISDN_TELEPHONY;
            break;
            }
        case RMobilePhone::EDataNumberPlan:
            {
            aIsiTypeOfNumber = SS_NBR_PLAN_DATA;
            break;
            }
        case RMobilePhone::ETelexNumberPlan:
            {
            aIsiTypeOfNumber = SS_NBR_PLAN_TELEX;
            break;
            }
        case RMobilePhone::ENationalNumberPlan:
            {
            aIsiTypeOfNumber = SS_NBR_PLAN_NATIONAL;
            break;
            }
        case RMobilePhone::EPrivateNumberPlan:
            {
            aIsiTypeOfNumber = SS_NBR_PLAN_PRIVATE;
            break;
            }
        case RMobilePhone::EServiceCentreSpecificPlan1:
        case RMobilePhone::EServiceCentreSpecificPlan2:
        case RMobilePhone::EERMESNumberPlan:
        default:
            {
TFLOGSTRING2("TSY: CMmCallMessHandler::MapTypeOfNumberMmToIsi -\
              Not supported numbering plan: %d", aMmNumberingPlan);
OstTrace1( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_MAPTYPEOFNUMBERMMTOISI, "CMmSupplServMessHandler::MapTypeOfNumberMmToIsi;aMmNumberingPlan=%d", aMmNumberingPlan );
            break;
            }
        }

    //get the right type of number
    switch ( aMmTypeOfNumber )
        {
        // isi numbering type is included in bits 5-7
        case RMobilePhone::EUnknownNumber:
            {
            aIsiTypeOfNumber = static_cast<TUint8>( aIsiTypeOfNumber |
                ( static_cast<TUint8>( SS_NBR_TYPE_UNKNOWN << 4 ) ) );
            }
            break;
        case RMobilePhone::EInternationalNumber:
            {
            aIsiTypeOfNumber = static_cast<TUint8>( aIsiTypeOfNumber |
                ( static_cast<TUint8>( SS_NBR_TYPE_INTERNATIONAL << 4 ) ) );
            break;
            }
        case RMobilePhone::ENationalNumber:
            {
            aIsiTypeOfNumber = static_cast<TUint8>( aIsiTypeOfNumber |
                ( static_cast<TUint8>( SS_NBR_TYPE_NATIONAL << 4 ) ) );
            break;
            }
        case RMobilePhone::ENetworkSpecificNumber:
            {
            aIsiTypeOfNumber = static_cast<TUint8>( aIsiTypeOfNumber |
                ( static_cast<TUint8>( SS_NBR_TYPE_NETWORK_SPECIFIC << 4 ) ) );
            break;
            }
        case RMobilePhone::ESubscriberNumber:
            {
            aIsiTypeOfNumber = static_cast<TUint8>( aIsiTypeOfNumber |
                ( static_cast<TUint8>( SS_NBR_TYPE_SUBSCRIBER << 4 ) ) );
            break;
            }
        case RMobilePhone::EAlphanumericNumber:
            {
            aIsiTypeOfNumber = static_cast<TUint8>( aIsiTypeOfNumber |
                ( static_cast<TUint8>( SS_NBR_TYPE_ALPHANUMERIC ) ) );
            break;
            }
        case RMobilePhone::EAbbreviatedNumber:
            {
            aIsiTypeOfNumber = static_cast<TUint8>( aIsiTypeOfNumber |
                ( static_cast<TUint8>( SS_NBR_TYPE_ABBREVIATED << 4 ) ) );
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmCallMessHandler::MapTypeOfNumberMmToIsi -\
              Not supported number type: %d", aMmTypeOfNumber);
OstTrace1( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_MAPTYPEOFNUMBERMMTOISI, "CMmSupplServMessHandler::MapTypeOfNumberMmToIsi;aMmTypeOfNumber=%d", aMmTypeOfNumber );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::SetVoiceCallForwardLine
// Updates internal variable that keeps track of which ALS line is used
// -----------------------------------------------------------------------------
//
void CMmSupplServMessHandler::SetVoiceCallForwardLine
    (
    RMobilePhone::TMobilePhoneALSLine& aAlsLine
    )
{
TFLOGSTRING2("TSY: CMmSupplServMessHandler::SetVoiceCallForwardLine: %d",
              aAlsLine);
OstTrace1( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_SETVOICECALLFORWARDLINE, "CMmSupplServMessHandler::SetVoiceCallForwardLine;aAlsLine=%d", aAlsLine );
    iAlsLine = aAlsLine;
}

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::ProcessIfSsRequestChanged
// Checks if original SS request is changed or initiated by Call Control
// -----------------------------------------------------------------------------
//
TBool CMmSupplServMessHandler::ProcessIfSsRequestChanged(
        const TPreprocessedSsServiceCompleteResponse& aResponse)
    {
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessIfSsRequestChanged");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSIFSSREQUESTCHANGED, "CMmSupplServMessHandler::ProcessIfSsRequestChanged" );
    TBool ret( EFalse );

    // original SS request is changed, or initiated by Call Control
    if ( ESSOperationTypeUnknown == aResponse.iTraId && iSsCode != aResponse.iSsCode )
        {
        ret = ETrue;
        if ( KSsCodeNoValue != iSsCode )
            {
TFLOGSTRING("TSY: call control has changed SS request -> complete with error");
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_PROCESSIFSSREQUESTCHANGED, "CMmSupplServMessHandler::ProcessIfSsRequestChanged, call control has changed SS request -> complete with error" );
            //call control has changed SS request -> complete with error
            iMessageRouter->Complete(
                iIpc,
                CMmStaticUtility::EpocErrorCode(
                    KErrAccessDenied,
                    KErrSatControl ) );
            }
        else
            {
TFLOGSTRING("TSY: call control has changed call request to SS request -> ignored");
OstTrace0( TRACE_NORMAL, DUP2_CMMSUPPLSERVMESSHANDLER_PROCESSIFSSREQUESTCHANGED, "CMmSupplServMessHandler::ProcessIfSsRequestChanged,call control has changed call request to SS request -> ignored" );
            // call control has changed call request to SS request.
            // CTSY doesn't expect the completion, ignore this (error EXTL-6XJ9KC)
            }
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::SetVoiceCallForwardLine
// Checks if IPC requested allows simple completion
// -----------------------------------------------------------------------------
//
TBool CMmSupplServMessHandler::ProcessIfSimpleCompletionPossible(
        const TPreprocessedSsServiceCompleteResponse& /*aResponse*/)
    {
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessIfSimpleCompletionPossible");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSIFSIMPLECOMPLETIONPOSSIBLE, "CMmSupplServMessHandler::ProcessIfSimpleCompletionPossible" );
    TBool ret( EFalse );

    if ( EMobilePhoneSendNetworkServiceRequestNoFdnCheck == iIpc ||
              EMobilePhoneSendNetworkServiceRequest == iIpc )
        {
        ret = ETrue;
        iMessageRouter->Complete( iIpc, KErrNone );
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::ProcessSsGsmPasswordRegistration
// Processes SS_GSM_PASSWORD_REGISTRATION response
// -----------------------------------------------------------------------------
//
TBool CMmSupplServMessHandler::ProcessSsGsmPasswordRegistration(
        const TPreprocessedSsServiceCompleteResponse& aResponse)
    {
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSSSGSMPASSWORDREGISTRATION, "CMmSupplServMessHandler::ProcessSsGsmPasswordRegistration" );
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessSsGsmPasswordRegistration");
    TBool ret( EFalse );

    if ( SS_GSM_PASSWORD_REGISTRATION == aResponse.iOperation )
        {
        ret = ETrue;
        // password changed successfully
        iMessageRouter->Complete( EMobilePhoneSetSSPassword, KErrNone );
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::ProcessIfCallForwarding
// Checks if this is related to call forwarding
// -----------------------------------------------------------------------------
//
TBool CMmSupplServMessHandler::ProcessIfCallForwardingL(
        const TPreprocessedSsServiceCompleteResponse& aResponse)
    {
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessIfCallForwardingL");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSIFCALLFORWARDINGL, "CMmSupplServMessHandler::ProcessIfCallForwardingL" );
    TBool ret( EFalse );
    // offset where the subblock starts
    TUint sbStartOffset( 0 );

    switch ( aResponse.iSsCode )
        {
        // call forwarding cases
        case SS_GSM_ALL_FORWARDINGS:
        case SS_GSM_ALL_COND_FORWARDINGS:
        case SS_GSM_FORW_UNCONDITIONAL:
        case SS_GSM_FORW_BUSY:
        case SS_GSM_FORW_NO_REPLY:
        case SS_GSM_FORW_NO_REACH:
            {
            ret = ETrue;
            if ( SS_INTERROGATION == aResponse.iOperation )
                {
                // this is the response to a call forwarding interrogation
                // (ETel API method GetCallForwardingStatus)
                // the results will be saved in this list
                delete iCFResults;
                iCFResults = NULL;
                iCFResults = CMobilePhoneCFList::NewL();

                // Check sub blocks in ISA message:
                // - Try to find and interpret FORWARDING_INFO sub block
                // - If not present, try to find and interpret STATUS_RESULT sub block
                // - If also this is not present, Call Forwarding is not active.

                if ( ( 0 != aResponse.iNumSubBlocks )
                    && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_GSM_FORWARDING_INFO,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                    {
                    // get data from ss gsm forwarding sub block, result
                    // will be saved in iCFResults
                    HandleSsGsmForwardingSubBlockL(
                        aResponse.iIsiMessage,
                        sbStartOffset,
                        aResponse.iSsCode );
                    }
                else if ( ( 0 != aResponse.iNumSubBlocks )
                    && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_STATUS_RESULT,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                    {
                    // get data from ss status sub block
                    RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
                    entry.iStatus = RMobilePhone::ECallForwardingStatusUnknown;
                    TUint8 statusIsi = aResponse.iIsiMessage.Get8bit(
                        sbStartOffset + SS_STATUS_RESULT_OFFSET_SSSTATUS );
                    MapCFStatusIsiToMm( statusIsi, &entry.iStatus );
                    MapCFConditionIsiToMm( aResponse.iSsCode, &entry.iCondition );
                    MapBasicServiceCodeIsiToMm(
                        aResponse.iBasicService,
                        &entry.iServiceGroup );
                    // save result to iCFResults
                    iCFResults->AddEntryL( entry );
                    }
                else
                    {
                    // if this was an interrogation, and no SsGsmForwardingInfo
                    // or SsStatus sub block was found, the requested divert
                    // is not active. We have to build an entry for the
                    // 'Divert Not Active' case ourselves.
                    RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
                    entry.iStatus =
                        RMobilePhone::ECallForwardingStatusNotRegistered;
                    MapCFConditionIsiToMm( aResponse.iSsCode, &entry.iCondition );
                    MapBasicServiceCodeIsiToMm(
                        aResponse.iBasicService,
                        &entry.iServiceGroup );
                    // save result to iCFResults
                    iCFResults->AddEntryL( entry );
                    }

                // send result to SOS layer
                CMmDataPackage dataPackage;
                dataPackage.PackData( iCFResults );
                iMessageRouter->Complete(
                    EMobilePhoneGetCallForwardingStatusPhase1,
                    &dataPackage,
                    KErrNone );

                delete iCFResults;
                iCFResults = NULL;
                }
            else
                {
                // this is the response to a call forwarding action
                // (ETel API method SetCallForwardingStatus)

                // we know that the request was successful: In the case of a
                // failure, a SS_SERVICE_FAILED_RESP is received instead

                // complete CF status changed notification to SOS layer
                // as we have no cached values of the statuses,
                // we _always_ trigger the changed notification (although we
                // don't know if something has actually changed)
                RMobilePhone::TMobilePhoneCFCondition cfCondition;
                MapCFConditionIsiToMm( aResponse.iSsCode, &cfCondition );
                // packed parameter: a RMobilePhone::TMobilePhoneCFCondition
                CMmDataPackage dataPackage;
                dataPackage.PackData( &cfCondition );
                iMessageRouter->Complete(
                    EMobilePhoneNotifyCallForwardingStatusChange,
                    &dataPackage,
                    KErrNone );

                // SS request was successfull, write CallForwarding flags
                // to the SIM.
                // IPC EMobilePhoneSetCallForwardingStatus will be
                // completed from UiccHandleCallFwdFlagsResp.
                UiccReadCallFwdFlagsRel4Req();
                }
            break;
            } // call forwarding cases
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::ProcessIfCallBarring
// Checks if this is related to call barring
// -----------------------------------------------------------------------------
//
TBool CMmSupplServMessHandler::ProcessIfCallBarringL(
        const TPreprocessedSsServiceCompleteResponse& aResponse)
    {
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessIfCallBarringL");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSIFCALLBARRINGL, "CMmSupplServMessHandler::ProcessIfCallBarringL" );
    TBool ret( EFalse );
    // offset where the subblock starts
    TUint sbStartOffset( 0 );

    switch ( aResponse.iSsCode )
        {
        // call barring cases
        case SS_GSM_ALL_BARRINGS:
        case SS_GSM_BARR_ALL_OUT:
        case SS_GSM_BARR_OUT_INTER:
        case SS_GSM_BARR_OUT_INTER_EXC_HOME:
        case SS_GSM_BARR_ALL_IN:
        case SS_GSM_BARR_ALL_IN_ROAM:
        case SS_GSM_OUTGOING_BARR_SERV:
        case SS_GSM_INCOMING_BARR_SERV:
            {
            ret = ETrue;
            }
            if ( SS_INTERROGATION == aResponse.iOperation )
                {
                // this is the response to a call barring interrogation
                // (ETel API method GetCallBarringStatus)

                // the results will be saved in this list
                delete iCBResults;
                iCBResults = NULL;
                iCBResults = CMobilePhoneCBList::NewL();

                // Check sub blocks in ISA message:
                // - Try to find and interpret BSC_INFO sub block
                // - If not present, try to find and interpret STATUS_RESULT sub block
                // - If also this is not present, Call Barring is not active.

                if ( ( 0 != aResponse.iNumSubBlocks )
                    && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_GSM_BSC_INFO,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                    {
                    // get data from ss bsc info sub block, result
                    // will be saved in iCBResults
                    HandleBarringSsGsmBscInfoSubBlockL(
                        aResponse.iIsiMessage,
                        sbStartOffset,
                        aResponse.iSsCode );
                    }
                else if ( ( 0 != aResponse.iNumSubBlocks )
                    && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_STATUS_RESULT,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                    {
                    // get data from ss status sub block
                    RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;
                    entry.iStatus = RMobilePhone::ECallBarringStatusUnknown;
                    TUint8 statusIsi = aResponse.iIsiMessage.Get8bit(
                        sbStartOffset + SS_STATUS_RESULT_OFFSET_SSSTATUS );
                    MapCBStatusIsiToMm( statusIsi, &entry.iStatus );
                    MapCBConditionIsiToMm( aResponse.iSsCode, &entry.iCondition );
                    MapBasicServiceCodeIsiToMm(
                        aResponse.iBasicService,
                        &entry.iServiceGroup );
                    // save result to iCBResults
                    iCBResults->AddEntryL( entry );
                    }
                else
                    {
                    // if this was an interrogation, and no SsGsmBscInfo or SsStatus
                    // sub block was found, the requested barring is not active.
                    // We have to build an entry for the 'Barring Not Active'
                    // case ourselves.
                    RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;
                    entry.iStatus =
                        RMobilePhone::ECallBarringStatusNotActive;
                    MapCBConditionIsiToMm( aResponse.iSsCode, &entry.iCondition );
                    MapBasicServiceCodeIsiToMm(
                        aResponse.iBasicService,
                        &entry.iServiceGroup );
                    // save result to iCBResults
                    iCBResults->AddEntryL( entry );
                    }

                // send result to SOS layer
                CMmDataPackage dataPackage;
                dataPackage.PackData( iCBResults );
                iMessageRouter->Complete(
                    EMobilePhoneGetBarringStatusPhase1,
                    &dataPackage,
                    KErrNone );
                delete iCBResults;
                iCBResults = NULL;
                }
            else
                {
                // this is the response to a call barring action
                // (ETel API method SetCallBarringStatus)

                // we know that the request was successful: In the case of a
                // failure, a SS_SERVICE_FAILED_RESP is received instead

                // complete CB status changed notification to SOS layer
                // as we have no cached values of the statuses,
                // we _always_ trigger the changed notification (although
                // we don't know if something has actually changed)
                RMobilePhone::TMobilePhoneCBCondition cbCondition;
                MapCBConditionIsiToMm( aResponse.iSsCode, &cbCondition );
                // packed parameter: a RMobilePhone::TMobilePhoneCBCondition
                CMmDataPackage dataPackage;
                dataPackage.PackData( &cbCondition );
                iMessageRouter->Complete(
                    EMobilePhoneNotifyCallBarringStatusChange,
                    &dataPackage,
                    KErrNone );

                // complete the SetCallBarringStatus to SOS layer
                iMessageRouter->Complete(
                    EMobilePhoneSetCallBarringStatus,
                    KErrNone );
                }
            break;
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::ProcessIfCallWaiting
// Checks if this is related to call waiting
// -----------------------------------------------------------------------------
//
TBool CMmSupplServMessHandler::ProcessIfCallWaitingL(
        const TPreprocessedSsServiceCompleteResponse& aResponse)
    {
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessIfCallWaitingL");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSIFCALLWAITINGL, "CMmSupplServMessHandler::ProcessIfCallWaitingL" );
    TBool ret( EFalse );
    // offset where the subblock starts
    TUint sbStartOffset( 0 );

    switch ( aResponse.iSsCode )
        {
        // call waiting cases
        case SS_GSM_CALL_WAITING:
            {
            ret = ETrue;
            if ( SS_INTERROGATION == aResponse.iOperation )
                {
                // this is the response to a call waiting interrogation
                // (ETel API method GetCallWaitingStatus)
                // the results will be saved in this list
                delete iCWResults;
                iCWResults = NULL;
                iCWResults = CMobilePhoneCWList::NewL();

                // Check sub blocks in ISA message:
                // - Try to find and interpret BSC_INFO sub block
                // - If not present, try to find and interpret STATUS_RESULT sub block
                // - If also this is not present, Call Waiting is not active.

                if ( ( 0 != aResponse.iNumSubBlocks )
                    && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_GSM_BSC_INFO,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                    {
                    // get data from ss bsc info sub block, result
                    // will be saved in iCWResults
                    HandleWaitingSsGsmBscInfoSubBlockL(
                        aResponse.iIsiMessage,
                        sbStartOffset );
                    }
                else if ( ( 0 != aResponse.iNumSubBlocks )
                    && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_STATUS_RESULT,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                    {
                    // get data from ss status sub block
                    RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;
                    entry.iStatus = RMobilePhone::ECallWaitingStatusUnknown;
                    TUint8 statusIsi = aResponse.iIsiMessage.Get8bit(
                        sbStartOffset + SS_STATUS_RESULT_OFFSET_SSSTATUS );
                    MapCWStatusIsiToMm( statusIsi, &entry.iStatus );
                    MapBasicServiceCodeIsiToMm(
                        aResponse.iBasicService,
                        &entry.iServiceGroup );
                    // save result to iCWResults
                    iCWResults->AddEntryL( entry );
                    }
                else
                    {
                    // if this was an interrogation, and no SsGsmBscInfo or
                    // SsStatus sub block was found, waiting is not active.
                    // We have to build an entry for the 'Waiting Not Active'
                    // case ourselves.
                    RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;
                    entry.iStatus = RMobilePhone::ECallWaitingStatusNotActive;
                    MapBasicServiceCodeIsiToMm(
                        aResponse.iBasicService,
                        &entry.iServiceGroup );
                    // save result to iCWResults
                    iCWResults->AddEntryL( entry );
                    }

                // send result to SOS layer
                CMmDataPackage dataPackage;
                dataPackage.PackData( iCWResults );
                iMessageRouter->Complete(
                    EMobilePhoneGetWaitingStatusPhase1,
                    &dataPackage,
                    KErrNone );

                delete iCWResults;
                iCWResults = NULL;
                }
            else
                {
                // this is the response to a call waiting action
                // (ETel API method SetCallWaitingStatus)

                // we know that the request was successful: In the case of a
                // failure, a SS_SERVICE_FAILED_RESP is received instead

                // complete CW status changed notification to SOS layer
                // as we have no cached values of the statuses,
                // we _always_ trigger the changed notification (although we
                // don't know if something has actually changed)
                RMobilePhone::TMobilePhoneCWInfoEntryV1 cwInfo;
                MapBasicServiceCodeIsiToMm( aResponse.iBasicService,
                                            &cwInfo.iServiceGroup );
                // maybe this can be solved in a different way
                if ( SS_ACTIVATION == aResponse.iOperation )
                    {
                    cwInfo.iStatus = RMobilePhone::ECallWaitingStatusActive;
                    }
                else if ( SS_DEACTIVATION == aResponse.iOperation )
                    {
                    cwInfo.iStatus = RMobilePhone::ECallWaitingStatusNotActive;
                    }
                else
                    {
                    // interrogation is already handled above; the other
                    // operations are not allowed for call waiting
                    cwInfo.iStatus = RMobilePhone::ECallWaitingStatusUnknown;
                    }

                // packed parameter: a RMobilePhone::TMobilePhoneCWInfoEntryV1
                CMmDataPackage dataPackage;
                dataPackage.PackData( &cwInfo );
                iMessageRouter->Complete(
                    EMobilePhoneNotifyCallWaitingStatusChange,
                    &dataPackage,
                    KErrNone );

                // complete the SetCallWaitingStatus to SOS layer
                iMessageRouter->Complete(
                    EMobilePhoneSetCallWaitingStatus,
                    KErrNone );
                }
            break;
            } // call waiting cases
        }

    return ret;
    }

// ----------------------------------------------------------------------------
// CMmSupplServMessHandler::ProcessIfIdentityServices
// Checks if this is related to identity services
// -----------------------------------------------------------------------------
//
TBool CMmSupplServMessHandler::ProcessIfIdentityServices(
        const TPreprocessedSsServiceCompleteResponse& aResponse)
    {
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessIfIdentityServices");
OstTrace0( TRACE_NORMAL, CMMSUPPLSERVMESSHANDLER_PROCESSIFIDENTITYSERVICES, "CMmSupplServMessHandler::ProcessIfIdentityServices" );
    TBool ret( EFalse );
    // offset where the subblock starts
    TUint sbStartOffset( 0 );

    switch ( aResponse.iSsCode )
        {
        // identity services cases
        case SS_GSM_CLIP:
        case SS_GSM_CLIR:
        case SS_GSM_COLP:
        case SS_GSM_COLR:
        case SS_GSM_CNAP:
            {
            ret = ETrue;
            if ( SS_INTERROGATION == aResponse.iOperation )
                {
                // this is the response to a identity services interrogation
                // (ETel API method GetIdentityServiceStatus).
                // (Setting the status through ETel API method
                // SetIdentityServiceStatus is not supported).

                RMobilePhone::TMobilePhoneIdServiceStatus statusETel =
                    RMobilePhone::EIdServiceUnknown;

                // yet another special case: CLIR response may include a more
                // detailed SsGsmGenericServiceInfo sub block instead of the
                // SsStatusResult sub block.
                TBool clirInfoFound = EFalse;
                if ( SS_GSM_CLIR == aResponse.iSsCode )
                    {
                    TUint8 clirOption = 0xFF; // initialise with illegal value
                    if ( ( 0 != aResponse.iNumSubBlocks )
                        && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_GSM_GENERIC_SERVICE_INFO,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                        {
TFLOGSTRING("TSY:CMmSupplServMessHandler::ProcessIfIdentityServices: KSsGsmGenericServiceInfo found");
OstTrace0( TRACE_NORMAL, DUP1_CMMSUPPLSERVMESSHANDLER_PROCESSIFIDENTITYSERVICES, "CMmSupplServMessHandler::ProcessIfIdentityServices, KSsGsmGenericServiceInfo found" );
                        clirInfoFound = ETrue;
                        // get status
                        TUint8 statusIsi = aResponse.iIsiMessage.Get8bit(
                            sbStartOffset +
                            SS_GSM_GENERIC_SERVICE_INFO_OFFSET_SSSTATUS );

                        // get GSM_CLIR_INFO if available

                        if ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP +
                            SIZE_SS_GSM_GENERIC_SERVICE_INFO,
                            SS_GSM_CLIR_INFO,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) )
                            {
                            clirOption = aResponse.iIsiMessage.Get8bit(
                                sbStartOffset +
                                SS_GSM_CLIR_INFO_OFFSET_CLIRESTRICTIONOPTION );
                            }
                        MapIdentityServiceStatusIsiToMm(
                            statusIsi,
                            clirOption,
                            &statusETel );
                        }
                    }

                if ( !clirInfoFound )
                    {
                    // no extra CLIR info ==> handle SsStatusResult sub block.
                    if ( ( 0 != aResponse.iNumSubBlocks )
                        && ( KErrNone == aResponse.iIsiMessage.FindSubBlockOffsetById(
                            ISI_HEADER_SIZE + SIZE_SS_SERVICE_COMPLETED_RESP,
                            SS_STATUS_RESULT,
                            EIsiSubBlockTypeId8Len8,
                            sbStartOffset ) ) )
                        {
                        TUint8 statusIsi = aResponse.iIsiMessage.Get8bit(
                            sbStartOffset + SS_STATUS_RESULT_OFFSET_SSSTATUS );

                        MapIdentityServiceStatusIsiToMm(
                            statusIsi,
                            SS_GSM_CLI_PERMANENT,
                            &statusETel );
                        }
                    }

                // send result to SOS layer
                // packed parameter: a RMobilePhone::TMobilePhoneIdServiceStatus
                CMmDataPackage dataPackage;
                dataPackage.PackData( &statusETel );
                iMessageRouter->Complete(
                    EMobilePhoneGetIdentityServiceStatus,
                    &dataPackage,
                    KErrNone );
                }
            break;
            } // identity services cases
        }

    return ret;
    }

//  End of File
