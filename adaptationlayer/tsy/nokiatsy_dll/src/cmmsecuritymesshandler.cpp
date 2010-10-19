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

//  INCLUDE FILES
#include "cmmsecuritymesshandler.h"
#include "cmmphonetsender.h"
#include "cmmstaticutility.h"
#include "cmmmessagerouter.h"
#include "cmmnetmesshandler.h"
#include "cmmcallmesshandler.h"
#include "tsylogger.h" // logging
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsecuritymesshandlerTraces.h"
#endif

#include <etelmm.h>
#include <tisi.h>
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
#include <mtcisi.h>
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <mceisi.h>
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <uiccisi.h>

// EXTERNAL DATA STRUCTURES
    //None

// EXTERNAL FUNCTION PROTOTYPES
    //None

// CONSTANTS
// hard-coded transaction id for ISA messages
const TUint8 KSecurityTransId = 7;

// Maximum size of UICC_SB_PIN and UICC_SB_PUK
const TUint8 KMaxSizeUiccSbPin = 16;
const TUint8 KMaxSizeUiccSbPuk = 16;

// Key reference for universal PIN
const TUint8 KUniversalPinKeyReference = 0x11;

// constants for getting/setting FDN state
const TUint8 KUiccServiceFdn      = 0x02; // service in EFust
const TUint8 KIccServiceFdn       = 0x03; // service in EFsst
const TUint8 KIccServiceAdn       = 0x02; // service in EFsst
const TUint8 KFdnStateMask        = 0x01;
const TUint8 KServiceActivated    = 0x01;
const TUint8 KServiceNotActivated = 0x00;
const TUint8 KInvalidated         = 0x00;
const TUint8 KNotInvalidated      = 0x01;
const TUint8 KInvalidateFlagMask  = 0x01;

const TUint8 KSw1Position = 0x02;
const TUint8 KSw2Position = 0x01;

const TUint16 KElemAdnIcc = 0x6F3A;

// MACROS
    //None

// LOCAL CONSTANTS AND MACROS
const TUint8 KSecPadding = 0x00; // Filler byte for ISI messages

//TICCType enumerates the SIM card types
enum TICCType
    {
    EICCTypeSim2GGsm = 0,
    EICCTypeSim3G,
    EICCTypeSimUnknown
    };

// MODULE DATA STRUCTURES
    //None

// LOCAL FUNCTION PROTOTYPES
    //None


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::CMmSecurityMessHandler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMmSecurityMessHandler::CMmSecurityMessHandler()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::CMmSecurityMessHandler");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_CMMSECURITYMESSHANDLER_TD, "CMmSecurityMessHandler::CMmSecurityMessHandler" );
    //none
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmSecurityMessHandler* CMmSecurityMessHandler::NewL(
    CMmPhoNetSender* aPhoNetSender, //pointer to the phonet sender
    CMmPhoNetReceiver* aPhoNetReceiver, //pointer to the phonet sender
    CMmNetMessHandler* aNetMessHandler, //pointer to the net mess handler
    CMmCallMessHandler* aCallMessHandler, //pointer to the call mess handler
    CMmMessageRouter* aMessageRouter,
    CMmUiccMessHandler* aUiccMessHandler )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::NewL");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_NEWL_TD, "CMmSecurityMessHandler::NewL" );
    CMmSecurityMessHandler* const securityMessHandler =
        new ( ELeave ) CMmSecurityMessHandler();

    CleanupStack::PushL( securityMessHandler );
    securityMessHandler->iPhoNetSender = aPhoNetSender;
    securityMessHandler->iNetMessHandler = aNetMessHandler;
    securityMessHandler->iCallMessHandler = aCallMessHandler;
    securityMessHandler->iMessageRouter = aMessageRouter;
    securityMessHandler->iMmUiccMessHandler = aUiccMessHandler;

    securityMessHandler->ConstructL();

    // UICC
    aPhoNetReceiver->RegisterL( securityMessHandler, PN_UICC, UICC_IND );
    aPhoNetReceiver->RegisterL( securityMessHandler, PN_UICC, UICC_RESP );
    aPhoNetReceiver->RegisterL( securityMessHandler, PN_UICC, UICC_CARD_IND );
    aPhoNetReceiver->RegisterL( securityMessHandler, PN_UICC, UICC_PIN_IND );
    aPhoNetReceiver->RegisterL( securityMessHandler, PN_UICC, UICC_PIN_RESP );

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    //MTC
    aPhoNetReceiver->RegisterL(
        securityMessHandler, PN_MTC, MTC_STATE_INFO_IND );
    aPhoNetReceiver->RegisterL(
        securityMessHandler, PN_MTC, MTC_RF_STATUS_QUERY_RESP );
    aPhoNetReceiver->RegisterL(
        securityMessHandler, PN_MTC, MTC_STATE_QUERY_RESP );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // MCE
    aPhoNetReceiver->RegisterL(
        securityMessHandler, PN_MODEM_MCE, MCE_MODEM_STATE_IND );
    aPhoNetReceiver->RegisterL(
        securityMessHandler, PN_MODEM_MCE, MCE_MODEM_STATE_QUERY_RESP );
    aPhoNetReceiver->RegisterL(
        securityMessHandler, PN_MODEM_MCE, MCE_RF_STATE_QUERY_RESP );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    CleanupStack::Pop( securityMessHandler );
    return securityMessHandler;
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::ConstructL()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::ConstructL");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_CONSTRUCTL_TD, "CMmSecurityMessHandler::ConstructL" );

#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
    // Initialize the current MTC state to poweroff, and do the first state query
    iBootState.iMtcCurrentState = MTC_POWER_OFF;
    MtcStateQueryReq( KSecurityTransId );
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
    // Initialize the current modem state to poweroff
    iBootState.iMceCurrentState = MCE_POWER_OFF;

    // Query modem state and RF state
    MceModemStateQueryReq();
    MceRfStateQueryReq();
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

    iPukCodeRequired = EFalse;

    iApplicationId = 0;
    iPinId = 0;
    // This flags indicates if PIN attempts left query in case
    iPinAttemptsLeftQuery = EFalse;
    // This flags indicates if lock state query in case
    iLockStateQuery = EFalse;
    iCodeType = RMobilePhone::ESecurityCodePin1;

    iFdnSetting = RMobilePhone::EFdnSetOff;
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::~CMmSecurityMessHandler
// Destructor
// -----------------------------------------------------------------------------
//
CMmSecurityMessHandler::~CMmSecurityMessHandler()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::~CMmSecurityMessHandler");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_CMMSECURITYMESSHANDLER_TD, "CMmSecurityMessHandler::~CMmSecurityMessHandler" );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::ExtFuncL
// Dispatches Etel requests to DOS level handlers
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::ExtFuncL(
    TInt aIpc,
    const CMmDataPackage* aDataPackage )
    {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::ExtFuncL, aIpc: %d", aIpc);
OstTrace1( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_EXTFUNCL_TD, "CMmSecurityMessHandler::ExtFuncL;aIpc=%d", aIpc );

    TInt ret( KErrNone );

    TUint8 transId( KSecurityTransId );

    switch ( aIpc )
        {
        case EMobilePhoneGetLockInfo:
            {
            // Unpack data
            RMobilePhone::TMobilePhoneLock* aLock( NULL );
            RMobilePhone::TMobilePhoneLockInfoV1* aLockInfo( NULL );
            aDataPackage->UnPackData( &aLock, &aLockInfo);
            // Set flag to indicate that this is lock state query
            iLockStateQuery = ETrue;
            switch( *aLock )
                {
                case RMobilePhone::ELockPhoneDevice: // Device lock
                    {
                    // Not yet supported in modemadaptation
                    ret = KErrNotSupported;
                    break;
                    }
                case RMobilePhone::ELockPhoneToICC: // Lock to current (U)SIM
                    {
                    // Not yet supported in modemadaptation
                    ret = KErrNotSupported;
                    break;
                    }
                case RMobilePhone::ELockICC: // PIN
                case RMobilePhone::ELockPin2: // PIN2
                    {
                    UiccPinReqStateQuery( *aLock );
                    break;
                    }
                case RMobilePhone::ELockUniversalPin:
                    {
                    // UPIN is supported only in 3G
                    if ( UICC_CARD_TYPE_UICC ==
                        iMmUiccMessHandler->GetCardType() )
                        {
                        UiccPinReqStateQuery( *aLock );
                        }
                    else
                        {
                        ret = KErrNotSupported;
                        }
                    break;
                    }
                default:
                    {
                    ret = KErrNotSupported;
                    break;
                    }
                } // End of switch( *aLock )
            break;
            }
        case EMobilePhoneChangeSecurityCode:
            {
            //unpack data
            RMobilePhone::TMobilePhoneSecurityCode* type( NULL );
            RMobilePhone::TMobilePhonePasswordChangeV1* change( NULL );
            aDataPackage->UnPackData( &type, &change );
            ret = SecCodeChangeReq( transId, type, change );
            break;
            }
        case EMobilePhoneAbortSecurityCode:
            {
            // Just complete the cancelling to client
            // UICC server doesn't support cancelling process.
            iMessageRouter->Complete(
                EMobilePhoneAbortSecurityCode,
                KErrNone );
            break;
            }
        case EMobilePhoneVerifySecurityCode:
            {
            //unpack data
            RMobilePhone::TMobilePhoneSecurityCode* type( NULL );
            RMobilePhone::TCodeAndUnblockCode* codes( NULL );
            aDataPackage->UnPackData( &type, &codes );
            ret = VerifySecurityCode( type, codes );
            break;
            }
        case EMobilePhoneGetSecurityCodeInfo:
            {
            // Set flag to indicate that this is PIN attempts left query
            iPinAttemptsLeftQuery = ETrue;
            //unpack data
            RMobilePhone::TMobilePhoneSecurityCode* type( NULL );
            aDataPackage->UnPackData( &type );
            // Save code type for completing
            iCodeType = *type;
            TUint8 cardType( iMmUiccMessHandler->GetCardType() );

            // Initialize lock type to PIN1
            RMobilePhone::TMobilePhoneLock lock( RMobilePhone::ELockICC );
            if ( RMobilePhone::ESecurityCodePin2 == *type )
                {
                if ( UICC_CARD_TYPE_UICC != cardType )
                    {
                    ret = KErrNotSupported;
                    }
                lock = RMobilePhone::ELockPin2;
                }
            else if ( RMobilePhone::ESecurityUniversalPin == *type )
                {
                if ( UICC_CARD_TYPE_UICC != cardType )
                    {
                    ret = KErrNotSupported;
                    }
                lock = RMobilePhone::ELockUniversalPin;
                }
            if ( KErrNone == ret )
                {
                ret = UiccPinReqStateQuery( lock );
                }
            break;
            }
        case EMobilePhoneSetLockSetting:
            {
            //unpack data
            TLockAndSetting* lockAndSetting( NULL );
            RMobilePhone::TMobilePassword* password( NULL );
            aDataPackage->UnPackData( &lockAndSetting, &password );

            RMobilePhone::TMobilePhoneLock lock( *(lockAndSetting->iLock ) );
            RMobilePhone::TMobilePhoneLockSetting setting(
                *(lockAndSetting->iSetting ) );

            // PIN code disabling/enabling is don in UICC server
            if ( lock == RMobilePhone::ELockICC ||
                lock == RMobilePhone::ELockPin2 ||
                lock == RMobilePhone::ELockUniversalPin )
                {
                ret = UiccPinReqChangeState( lock, setting, *password );
                }
            else
                {
                // Not yet supported in modemadaptation
                ret = KErrNotSupported;
                }
            break;
            }
        case EMmTsySecurityGetSimActivePinStateIPC:
            {
            GetActivePin();
            break;
            }
        case EMmTsyBootNotifySimStatusReadyIPC:
            {
            // In case of UICC server has started before TSY, application
            // activation might not be done and in that case it is started now
            if ( UICC_STATUS_APPL_ACTIVE != 
                iMmUiccMessHandler->GetUiccApplicationStatus() )
                {
                iMmUiccMessHandler->CreateUiccReq();
                }
            else
                {
                // Application activation was already done
                iMessageRouter->Complete(
                    EMmTsyBootNotifySimStatusReadyIPC,
                    KErrNone );
                }

            break;
            }
        case EMmTsySimGetICCType:
            {
            GetIccType();
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
            //NOTE:
            // It is possible that the query below is not needed anymore, because always
            // in CMmSecurityMessHandler::ConstructL(), the MtcStateQueryReq() is called, which
            // eventually in its response calls also MtcRfStatusQueryReq()
            ret = MtcRfStatusQueryReq( transId );
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
            break;
            }
        case EMobilePhoneSetFdnSetting:
            {
            TUint8 cardType( iMmUiccMessHandler->GetCardType() );
            RMobilePhone::TMobilePhoneFdnSetting* fdnSetting( NULL );
            aDataPackage->UnPackData( &fdnSetting );
            iFdnSetting = *fdnSetting;

            if( UICC_CARD_TYPE_UICC == cardType )
                {
                ret = ReadEfEst( ETrIdSetFdnStateReadEst );
                }
            else if( UICC_CARD_TYPE_ICC == cardType )
                {
                ret = ReadEfAdnFileInfo( ETrIdSetFdnStateReadFileInfo );
                }
            else
                {
TFLOGSTRING("TSY: CMmSecurityMessHandler::ExtFuncL: unknown card type, FDN state not set");
OstTrace0( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_EXTFUNCL_TD, "CMmSecurityMessHandler::ExtFuncL: unknown card type, FDN state not set" );
                ret = KErrGeneral;
                }
            break;
            }
        case EMobilePhoneGetFdnStatus:
            {
            TUint8 cardType( iMmUiccMessHandler->GetCardType() );
            if( UICC_CARD_TYPE_UICC == cardType )
                {
                // read EFest
                ret = ReadEfEst( ETrIdGetFdnStateReadEst );
                }
            else if( UICC_CARD_TYPE_ICC == cardType )
                {
                // read file info for EFadn
                ret = ReadEfAdnFileInfo( ETrIdGetFdnStateReadFileInfo );
                }
            else
                {
TFLOGSTRING("TSY: CMmSecurityMessHandler::ExtFuncL: unknown card type, FDN state cannot be solved");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_EXTFUNCL_TD, "CMmSecurityMessHandler::ExtFuncL: unknown card type, FDN state cannot be solved" );
                ret = KErrGeneral;
                }
            break;
            }
        case EMobilePhoneGetCurrentActiveUSimApplication:
            {
            GetActiveUsimApplication();
            break;
            }
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case EMmTsyBootGetRFStatusIPC:
            {
            //This should not be in any use at the moment
TFLOGSTRING("TSY: CMmSecurityMessHandler::ExtFuncL - EMmTsyBootGetRFStatusIPC (NOT IN USE!!!)");
OstTrace0( TRACE_NORMAL,  DUP5_CMMSECURITYMESSHANDLER_EXTFUNCL_TD, "CMmSecurityMessHandler::ExtFuncL, EMmTsyBootGetRFStatusIPC (NOT IN USE!!!)" );
            ret = MtcRfStatusQueryReq( transId );
            break;
            }
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        default:
            {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::ExtFuncL - Unknown IPC: %d", aIpc);
OstTrace1( TRACE_NORMAL,  DUP6_CMMSECURITYMESSHANDLER_EXTFUNCL_TD, "CMmSecurityMessHandler::ExtFuncL;Unknown aIpc=%d", aIpc );
            ret = KErrNotSupported;
            break;
            }
        }

    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::ReceiveMessageL
// Called when an ISI message has been received.
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::ReceiveMessageL(
    const TIsiReceiveC &aIsiMessage ) // received ISI message
    {
    TInt resource (aIsiMessage.Get8bit(ISI_HEADER_OFFSET_RESOURCEID));
    TInt messageId(aIsiMessage.Get8bit(ISI_HEADER_OFFSET_MESSAGEID));

TFLOGSTRING3("TSY: CMmSecurityMessHandler::ReceiveMessageL - resource: %d, msgId: %d", resource, messageId);
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmSecurityMessHandler::ReceiveMessageL;resource=%d;messageId=%d", resource, messageId );

    switch ( resource )
        {
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
        case PN_MTC:
            {
            switch( messageId )
                {
                case MTC_STATE_INFO_IND:
                    {
                    MtcStateInfoIndL( aIsiMessage );
                    break;
                    }
                case MTC_STATE_QUERY_RESP:
                    {
                    MtcStateQueryRespL( aIsiMessage );
                    break;
                    }
                case MTC_RF_STATUS_QUERY_RESP:
                    {
                    MtcRfStatusQueryResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::ReceiveMessageL - PN_MTC - unknown msgId: %d", messageId);
//OstTrace1( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmSecurityMessHandler::ReceiveMessageL;PN_MTC - unknown messageId=%d", messageId );
                    break;
                    }
                } // end switch ( messageId )
            break; // end case PN_MTC
            }
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        case PN_MODEM_MCE:
            {
            switch( messageId )
                {
                case MCE_MODEM_STATE_IND:
                    {
                    MceModemStateInd( aIsiMessage );
                    break;
                    }
                case MCE_MODEM_STATE_QUERY_RESP:
                    {
                    MceModemStateQueryResp( aIsiMessage );
                    break;
                    }
                case MCE_RF_STATE_QUERY_RESP:
                    {
                    MceRfStateQueryResp( aIsiMessage );
                    break;
                    }
                default:
                    {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::ReceiveMessageL - PN_MTC - unknown msgId: %d", messageId);
OstTrace1( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmSecurityMessHandler::ReceiveMessageL;PN_MTC - unknown messageId=%d", messageId );
                    break;
                    }
                } // end switch ( messageId )
            break; // end case PN_MODEM_MCE
            }
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
        case PN_UICC:
            {
            switch( messageId )
                {
                case UICC_CARD_IND:
                    {
                    UiccCardInd( aIsiMessage );
                    break;
                    }
                case UICC_PIN_IND:
                    {
                    UiccPinInd( aIsiMessage );
                    break;
                    }
                case UICC_PIN_RESP:
                    {
                    UiccPinResp( aIsiMessage );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            break;
            }
        default:
            {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::ReceiveMessageL - unknown resource: %d", resource);
OstTrace1( TRACE_NORMAL,  DUP5_CMMSECURITYMESSHANDLER_RECEIVEMESSAGEL_TD, "CMmSecurityMessHandler::ReceiveMessageL;resource=%d", resource );
            break; // server not known
            }
        } // end of switch
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::VerifySecurityCode
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::VerifySecurityCode(
    RMobilePhone::TMobilePhoneSecurityCode* aType,
    RMobilePhone::TCodeAndUnblockCode* aCodes )
    {
    TInt ret( KErrNotReady );
    RMobilePhone::TMobilePassword code( aCodes->iCode );
    RMobilePhone::TMobilePassword unblockCode( aCodes->iUnblockCode );

TFLOGSTRING4("TSY: CMmSecurityMessHandler::VerifySecurityCode - code type: %d, code: %S, unblock code: %S",*aType, &code, &unblockCode);
OstTraceExt3( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_SECCODEVERIFYREQ_TD, "CMmSecurityMessHandler::VerifySecurityCode;code=%S;unblockCode=%S;aType=%hhu", code, unblockCode, *aType );

    iSecurityCode = *aType;

    switch( *aType )
        {
        // PIN codes are verified in UICC server
        case RMobilePhone::ESecurityCodePin1:
        case RMobilePhone::ESecurityCodePin2:
        case RMobilePhone::ESecurityUniversalPin:
            {
            ret = UiccPinReqVerify( UICC_PIN_VERIFY, code, unblockCode );
            break;
            }
        // PUK codes are verified in UICC server
        case RMobilePhone::ESecurityCodePuk1:
        case RMobilePhone::ESecurityCodePuk2:
        case RMobilePhone::ESecurityUniversalPuk:
            {
            if ( *aType == RMobilePhone::ESecurityCodePuk1 )
                {
                iPukCodeRequired = EFalse;
                }
            ret = UiccPinReqVerify( UICC_PIN_UNBLOCK, code, unblockCode );
            break;
            }
        default:
            {
            break;
            }
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::SecCodeChangeReq
//
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::SecCodeChangeReq(
    TUint8 aTransactionId,
    RMobilePhone::TMobilePhoneSecurityCode* aType,
    RMobilePhone::TMobilePhonePasswordChangeV1* aChange ) // Old&new password
    {
TFLOGSTRING3("TSY: CMmSecurityMessHandler::SecCodeChangeReq - traId: %d, type: %d", aTransactionId, *aType);
OstTraceExt2( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_SECCODECHANGEREQ_TD, "CMmSecurityMessHandler::SecCodeChangeReq;aTransactionId=%hhu;aType=%d", aTransactionId, *aType );
TFLOGSTRING2("TSY: CMmSecurityMessHandler::SecCodeChangeReq - old password: %S", &(aChange->iOldPassword));
OstTraceExt1( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_SECCODECHANGEREQ_TD, "CMmSecurityMessHandler::SecCodeChangeReq;aChange->iOldPassword=%S", aChange->iOldPassword );
TFLOGSTRING2("TSY: CMmSecurityMessHandler::SecCodeChangeReq - new password: %S", &(aChange->iNewPassword));
OstTraceExt1( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_SECCODECHANGEREQ_TD, "CMmSecurityMessHandler::SecCodeChangeReq;aChange->iNewPassword=%S", aChange->iNewPassword );

    TInt ret( KErrNone );

    if ( RMobilePhone::ESecurityCodePin1 == *aType ||
        RMobilePhone::ESecurityCodePin2 == *aType ||
        RMobilePhone::ESecurityUniversalPin == *aType )
        {
        ret = UiccPinReqChange(
            *aType,
            aChange->iOldPassword,
            aChange->iNewPassword );
        }
    else
        {
        ret = KErrNotSupported;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::UiccCardInd
// Breaks UICC_CARD_IND ISI-message
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::UiccCardInd( const TIsiReceiveC& aIsiMessage )
    {
    // Get service type
    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_CARD_IND_OFFSET_SERVICETYPE ) );

TFLOGSTRING2("TSY: CMmCustomMessHandler::UiccCardInd, service type: %d", serviceType );
OstTraceExt1( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_UICCCARDIND_TD, "CMmSecurityMessHandler::UiccCardInd;serviceType=%hhu", serviceType );

    if ( UICC_CARD_REMOVED == serviceType )
        {
TFLOGSTRING("TSY: CMmSecurityMessHandler::UiccCardInd - SIM Removed!");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_UICCCARDIND_TD, "CMmSecurityMessHandler::UiccCardInd - SIM Removed!" );
        // Change old boot state from TSY to not ready
        iBootState.iSecReady = EFalse;
        iBootState.iPinRequired = EFalse;
        iBootState.iPinVerified = EFalse;
        }
    }
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MtcStateInfoIndL
// Breaks a SIM_IND MTC_STATE_INFO_IND-message ISI-message and completes
// EMmTsyBootNotifyModemStatusReadyIPC to client
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::MtcStateInfoIndL(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::MtcStateInfoIndL");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MTCSTATEINFOINDL_TD, "CMmSecurityMessHandler::MtcStateInfoIndL" );

    TUint8 state( aIsiMessage.Get8bit( ISI_HEADER_SIZE + MTC_STATE_INFO_IND_OFFSET_STATE ) );
    TUint8 action( aIsiMessage.Get8bit( ISI_HEADER_SIZE + MTC_STATE_INFO_IND_OFFSET_ACTION ) );

TFLOGSTRING3("TSY: CMmSecurityMessHandler::MtcStateInfoIndL action: 0x%02x, state: 0x%02x", action, state);
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_MTCSTATEINFOINDL_TD, "CMmSecurityMessHandler::MtcStateInfoIndL;action=%hhx;state=%hhx", action, state );

    //1. CMT_STATE_READY _AND_ CURRENT_STATE_NEW
    if ( ( MTC_NOS_READY == action || MTC_READY == action ) &&
          iBootState.iMtcCurrentState != state )
        {
        iBootState.iMtcCurrentState = state;

TFLOGSTRING("NTSY: CMmSecurityMessHandler::MtcStateInfoIndL - CMT state transition occurred");
OstTrace0( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_MTCSTATEINFOINDL_TD, "CMmSecurityMessHandler::MtcStateInfoIndL, CMT state transition occurred" );
        if ( MTC_NORMAL == state )
            {
TFLOGSTRING("TSY: CMmSecurityMessHandler::MtcStateInfoIndL - MTC_NORMAL");
OstTrace0( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_MTCSTATEINFOINDL_TD, "CMmSecurityMessHandler::MtcStateInfoIndL, MTC_NORMAL" );
            iMessageRouter->Complete( EMmTsyBootNotifyModemStatusReadyIPC,
                           KErrNone );
            }
        //no else

        //to complete EMmTsyBootGetRFStatusIPC
        MtcRfStatusQueryReq( KSecurityTransId );

        if ( MTC_NORMAL == state || MTC_RF_INACTIVE == state )
            {
            //Dataport opening must be delayed to here instead of
            //CallMessHandler::ConstructL() to make sure CommonTSY is ready.
            //Either dataport should not be opened before CSD server
            //is started (not started in MTC_ALARM or MTC_CHARGING states).
            iCallMessHandler->InitializeDataportL();
            }
        //no else
        }
    //2. CMT_STATE_READY ( _AND_ CURRENT_STATE_OLD )
    else if( ( MTC_NOS_READY == action || MTC_READY == action ) )
        {
TFLOGSTRING("TSY: CMmSecurityMessHandler::MtcStateInfoIndL - CMT state already active");
OstTrace0( TRACE_NORMAL,  DUP4_CMMSECURITYMESSHANDLER_MTCSTATEINFOINDL_TD, "CMmSecurityMessHandler::MtcStateInfoIndL, CMT state already active" );
        }
    //no else //3. CMT_STATE_NOT_READY  - no action done in between state transition
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MtcRfStatusQueryReq
// Creates MTC_RF_STATUS_QUERY_REQ-message ISI-message and sends it via
// phonet
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::MtcRfStatusQueryReq(
    TUint8 aTransactionId ) const
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::MtcRfStatusQueryReq");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MTCRFSTATUSQUERYREQ_TD, "CMmSecurityMessHandler::MtcRfStatusQueryReq" );

    TBuf8<2> fillerData;
    fillerData.AppendFill( KSecPadding, 2 );        // Padding bytes

    TInt ret = iPhoNetSender->Send( PN_MTC,
                                    aTransactionId,
                                    MTC_RF_STATUS_QUERY_REQ, fillerData );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MtcRfStatusQueryResp
// Breaks a MTC_RF_STATUS_QUERY_RESP-message ISI-message and completes
// to client
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::MtcRfStatusQueryResp(
    const TIsiReceiveC& aIsiMessage )  const
    {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MtcRfStatusQueryResp" );
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MTCRFSTATUSQUERYRESP_TD, "CMmSecurityMessHandler::MtcRfStatusQueryResp" );

    TUint8 currentRfState = aIsiMessage.Get8bit( ISI_HEADER_SIZE +
        MTC_RF_STATUS_QUERY_RESP_OFFSET_CURRENT );

    //Defaults to RF ON
    TRfStateInfo statusInfo ( ERfsStateInfoNormal );

    //RF IS OFF ( this state is not updated when MTC_NORMAL )
    if( MTC_RF_OFF == currentRfState )
        {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MtcRfStatusQueryResp - RF OFF" );
OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_MTCRFSTATUSQUERYRESP_TD, "CMmSecurityMessHandler::MtcRfStatusQueryResp, RF OFF" );
        statusInfo = ERfsStateInfoInactive;
        }
TFLOGSTRING2("TSY: OFFLINE MODE IS: %d", statusInfo );
OstTrace1( TRACE_NORMAL,  DUP4_CMMSECURITYMESSHANDLER_MTCRFSTATUSQUERYRESP_TD, "CMmSecurityMessHandler::MtcRfStatusQueryResp;statusInfo=%d", statusInfo );

    CMmDataPackage dataPackage;
    dataPackage.PackData ( &statusInfo );

    //inform the upper layer about the new status of the rf
    iMessageRouter->Complete( EMmTsyBootGetRFStatusIPC, &dataPackage, KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MtcStateQueryReq
// Creates a MTC_STATE_QUERY_REQ-message ISI-message and sends it via
// phonet
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::MtcStateQueryReq(
    TUint8 aTransactionId
    ) const
    {

TFLOGSTRING("TSY: CMmSecurityMessHandler::MtcStateQueryReq called" );
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MTCSTATEQUERYREQ_TD, "CMmSecurityMessHandler::MtcStateQueryReq" );

    TBuf8<2> fillerData;
    fillerData.AppendFill( KSecPadding, 2 );        // Padding bytes

    TInt ret = iPhoNetSender->Send( PN_MTC, aTransactionId, MTC_STATE_QUERY_REQ, fillerData );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MtcStateQueryRespL
// Breaks a SIM_IND MTC_STATE_QUERY_RESP-message ISI-message and completes
// EMmTsyBootNotifyModemStatusReadyIPC to client. If the CMT is not ready with
// its transition, renews MTC_STATE_QUERY_REQ
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::MtcStateQueryRespL(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MtcStateQueryRespL");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MTCSTATEQUERYRESPL_TD, "CMmSecurityMessHandler::MtcStateQueryRespL" );

    TUint8 currentState = aIsiMessage.Get8bit(ISI_HEADER_SIZE + MTC_STATE_QUERY_RESP_OFFSET_CURRENT);
    TUint8 nextState    = aIsiMessage.Get8bit(ISI_HEADER_SIZE + MTC_STATE_QUERY_RESP_OFFSET_TARGET);

TFLOGSTRING3("NTSY: CMmSecurityMessHandler::MtcStateQueryRespL - (current: 0x%02x, next: 0x%02x)",currentState, nextState );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_MTCSTATEQUERYRESPL_TD, "CMmSecurityMessHandler::MtcStateQueryRespL;currentState=%hhx;nextState=%hhx", currentState, nextState );

    //CMT side is ready when state transistion is completed (in all normal cases this should be the case)

    //1. CMT_STATE_READY _AND_ CURRENT_STATE_NEW
    if( currentState == nextState && iBootState.iMtcCurrentState != currentState )
        {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MtcStateQueryRespL - CMT ready." );
OstTrace0( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_MTCSTATEQUERYRESPL_TD, "CMmSecurityMessHandler::MtcStateQueryRespL, CMT ready" );
        iBootState.iMtcCurrentState = currentState;

        if( MTC_NORMAL == currentState || MTC_RF_INACTIVE == currentState )
            {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MtcStateQueryRespL - EMmTsyBootNotifyModemStatusReadyIPC");
OstTrace0( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_MTCSTATEQUERYRESPL_TD, "CMmSecurityMessHandler::MtcStateQueryRespL, EMmTsyBootNotifyModemStatusReadyIPC" );
            iMessageRouter->Complete( EMmTsyBootNotifyModemStatusReadyIPC, KErrNone );
            }
        //to complete EMmTsyBootGetRFStatusIPC
        MtcRfStatusQueryReq( KSecurityTransId );
        }
    //2. CMT_STATE_NOT_READY - renew state query, if transition is not yet ready
    else if ( currentState != nextState )
        {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MtcStateQueryRespL - CMT state transition not ready.");
OstTrace0( TRACE_NORMAL,  DUP4_CMMSECURITYMESSHANDLER_MTCSTATEQUERYRESPL_TD, "CMmSecurityMessHandler::MtcStateQueryRespL, CMT state transition not ready" );
        MtcStateQueryReq( KSecurityTransId );
        }
    //no else //3. CMT_STATE_READY _AND_ CURRENT_STATE_OLD - no action needed if state already active

    if ( nextState == currentState &&
         ( MTC_NORMAL == currentState || MTC_RF_INACTIVE == currentState ) )
        {
        //Dataport opening must be delayed to here instead of
        //CallMessHandler::ConstructL() to make sure CommonTSY is ready.
        //Either dataport should not be opened before CSD server
        //is started (not started in MTC_ALARM or MTC_CHARGING states).
        iCallMessHandler->InitializeDataportL();
        }
    //no else
    }
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MceModemStateInd
// Breaks a MCE_MODEM_STATE_IND ISI-message and completes
// EMmTsyBootNotifyModemStatusReadyIPC to client
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::MceModemStateInd( const TIsiReceiveC& aIsiMessage )
    {
    TUint8 state( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + MCE_MODEM_STATE_IND_OFFSET_STATE ) );
    TUint8 action( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + MCE_MODEM_STATE_IND_OFFSET_ACTION ) );

TFLOGSTRING3("TSY: CMmSecurityMessHandler::MceModemStateInd action: 0x%02x, state: 0x%02x", action, state);
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_MCEMODEMSTATEIND_TD, "CMmSecurityMessHandler::MceModemStateInd;state=%hhx;action=%hhx", state, action );

    // Modem is ready and CMT status has been changed
    if ( MCE_READY == action && iBootState.iMceCurrentState != state )
        {
        iBootState.iMceCurrentState = state;

TFLOGSTRING("NTSY: CMmSecurityMessHandler::MceModemStateInd - CMT state transition occurred - MCE_NORMAL");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MCEMODEMSTATEIND_TD, "CMmSecurityMessHandler::MceModemStateInd - CMT state transition occurred - MCE_NORMAL" );

        iMessageRouter->Complete(
            EMmTsyBootNotifyModemStatusReadyIPC,
            KErrNone );

        // Dataport opening must be delayed to here instead of
        // CallMessHandler::ConstructL() to make sure CommonTSY is ready.
        iCallMessHandler->InitializeDataportL();

        // To complete EMmTsyBootGetRFStatusIPC
        MceRfStateQueryReq();
        }
    // Modem is ready but CMT status has not been changed
    else if( MCE_READY == action && iBootState.iMceCurrentState == state )
        {
TFLOGSTRING("TSY: CMmSecurityMessHandler::MceModemStateInd - CMT state mot changed");
OstTrace0( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_MCEMODEMSTATEIND_TD, "CMmSecurityMessHandler::MceModemStateInd - CMT state not changed" );
        }
    // No else, modem state not ready - no action needed
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MceRfStateQueryReq
// Creates MCE_RF_STATE_QUERY_REQ ISI-message and sends it via phonet
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::MceRfStateQueryReq() const
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::MceRfStateQueryReq");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MCERFSTATEQUERYREQ_TD, "CMmSecurityMessHandler::MceRfStateQueryReq" );

    // Create MCE_RF_STATE_QUERY_REQ message for querying modem state
    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_MCE );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, KSecurityTransId );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, MCE_RF_STATE_QUERY_REQ );
    isiMsg.Set16bit(
        ISI_HEADER_SIZE + MCE_RF_STATE_QUERY_REQ_OFFSET_FILLERBYTE1,
        KSecPadding );

    return iPhoNetSender->Send( isiMsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MceRfStateQueryResp
// Breaks a MCE_RF_STATE_QUERY_RESP ISI-message and completes to client
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::MceRfStateQueryResp(
    const TIsiReceiveC& aIsiMessage ) const
    {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MceRfStateQueryResp" );
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MCERFSTATEQUERYRESP_TD, "CMmSecurityMessHandler::MceRfStateQueryResp" );

    TUint8 currentRfState( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + MCE_RF_STATE_QUERY_RESP_OFFSET_CURRENT ) );

    // Default is RF OFF
    TRfStateInfo statusInfo( ERfsStateInfoInactive );

    if( MCE_RF_ON == currentRfState )
        {
        statusInfo = ERfsStateInfoNormal;
        }

TFLOGSTRING2("NTSY: CMmSecurityMessHandler::MceRfStateQueryResp: RF State is: %d", statusInfo );
OstTrace1( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_MCERFSTATEQUERYRESP_TD, "CMmSecurityMessHandler::MceRfStateQueryResp;statusInfo=%d", statusInfo );

    CMmDataPackage dataPackage;
    dataPackage.PackData ( &statusInfo );
    // Complete RF state
    iMessageRouter->Complete(
        EMmTsyBootGetRFStatusIPC,
        &dataPackage,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MceModemStateQueryReq
// Creates a MCE_MODEM_STATE_QUERY_REQ ISI-message and sends it via
// phonet
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::MceModemStateQueryReq() const
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::MceModemStateQueryReq" );
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MCEMODEMSTATEQUERYREQ_TD, "CMmSecurityMessHandler::MceModemStateQueryReq" );

    // Create MCE_MODEM_STATE_QUERY_REQ message for querying modem state
    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_MODEM_MCE );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, KSecurityTransId );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, MCE_MODEM_STATE_QUERY_REQ );
    isiMsg.Set16bit(
        ISI_HEADER_SIZE +
        MCE_MODEM_STATE_QUERY_REQ_OFFSET_FILLERBYTE1, KSecPadding );

    return iPhoNetSender->Send( isiMsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::MceStateQueryRespL
// Breaks a MCE_MODEM_STATE_QUERY_RESP message ISI-message and completes
// EMmTsyBootNotifyModemStatusReadyIPC to client. If the CMT is not ready with
// its transition, renews MTC_STATE_QUERY_REQ
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::MceModemStateQueryResp(
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MceModemStateQueryResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_MCEMODEMSTATEQUERYRESP_TD, "CMmSecurityMessHandler::MceModemStateQueryResp" );

    TUint8 currentState( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + MCE_MODEM_STATE_QUERY_RESP_OFFSET_CURRENT ) );
    TUint8 nextState( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + MCE_MODEM_STATE_QUERY_RESP_OFFSET_TARGET ) );

TFLOGSTRING3("NTSY: CMmSecurityMessHandler::MceModemStateQueryResp - (current: 0x%02x, next: 0x%02x)",currentState, nextState );
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_MCEMODEMSTATEQUERYRESP_TD, "CMmSecurityMessHandler::MceModemStateQueryResp;currentState=%hhx;nextState=%hhx", currentState, nextState );

    // CMT side is ready when state transistion is completed
    // (in all normal cases this should be the case)

    // Modem is ready and CMT status has been changed
    if( currentState == nextState &&
        iBootState.iMceCurrentState != currentState )
        {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MceModemStateQueryResp - CMT ready." );
OstTrace0( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_MCEMODEMSTATEQUERYRESP_TD, "CMmSecurityMessHandler::MceModemStateQueryResp - CMT ready" );

        iBootState.iMceCurrentState = currentState;

        if( MCE_NORMAL == currentState )
            {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MceModemStateQueryResp - EMmTsyBootNotifyModemStatusReadyIPC");
OstTrace0( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_MCEMODEMSTATEQUERYRESP_TD, "CMmSecurityMessHandler::MceModemStateQueryResp- EMmTsyBootNotifyModemStatusReadyIPC" );
            iMessageRouter->Complete(
                EMmTsyBootNotifyModemStatusReadyIPC,
                KErrNone );

            // Dataport opening must be delayed to here instead of
            // CallMessHandler::ConstructL() to make sure CommonTSY is ready.
            iCallMessHandler->InitializeDataportL();
            }
        }
    // Renew state query, if transition is not yet ready
    else if ( currentState != nextState )
        {
TFLOGSTRING("NTSY: CMmSecurityMessHandler::MceModemStateQueryResp - CMT state transition not ready.");
OstTrace0( TRACE_NORMAL,  DUP4_CMMSECURITYMESSHANDLER_MCEMODEMSTATEQUERYRESP_TD, "CMmSecurityMessHandler::MceModemStateQueryResp -- CMT state transition not ready" );
        MceModemStateQueryReq();
        }
    }
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::UiccPinInd
// Breaks a UICC_PIN_IND ISI-message.
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::UiccPinInd( const TIsiReceiveC& aIsiMessage )
    {
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_UICCPININD_TD, "CMmSecurityMessHandler::UiccPinInd" );
TFLOGSTRING("TSY: CMmSecurityMessHandler::UiccPinInd");

    // Event to be completed
    RMobilePhone::TMobilePhoneSecurityEvent event( RMobilePhone::ENoICCFound );

    // Save application ID, needed in UICC_PIN_VERIFY_REQ
    iApplicationId = aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_PIN_IND_OFFSET_APPLID );

    // Get service type
    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_PIN_IND_OFFSET_SERVICETYPE ) );
    // Get PIN ID
    // TS 102.221 Table 9.3: PIN mapping into key references
    // PIN1: '01','02','03','04','05','06','07','08'
    // PIN2: '81','82','83','84', '85','86','87','88'
    // UPIN:  '11'
    // PIN ID is needed when verifying PIN/PUK code
    iPinId =  aIsiMessage.Get8bit(
            ISI_HEADER_SIZE + UICC_PIN_IND_OFFSET_PINID );

    if ( UICC_PIN_VERIFY_NEEDED == serviceType )
        {
        // PIN1
        if ( 0x01 <= iPinId && 0x08 >= iPinId )
            {
            event = RMobilePhone::EPin1Required;
            }
        // PIN2
        else if ( 0x81 <= iPinId && 0x88 >= iPinId )
            {
            event = RMobilePhone::EPin2Required;
            }
        // UPIN
        else if ( 0x11 == iPinId )
            {
            event = RMobilePhone::EUniversalPinRequired;
            }
        // No else
        }
    else if ( UICC_PIN_UNBLOCK_NEEDED == serviceType )
        {
        // PIN1
        if ( 0x01 <= iPinId && 0x08 >= iPinId )
            {
            event = RMobilePhone::EPuk1Required;
            iPukCodeRequired = ETrue;
            }
        // PIN2
        else if ( 0x81 <= iPinId && 0x88 >= iPinId )
            {
            event = RMobilePhone::EPuk2Required;
            }
        // UPIN
        else if ( 0x11 == iPinId )
            {
            event = RMobilePhone::EUniversalPukRequired;
            }
        // No else
        }
    // Complete notify security event
    CMmDataPackage dataPackage;
    dataPackage.PackData( &event );
    iMessageRouter->Complete(
        EMobilePhoneNotifySecurityEvent,
        &dataPackage,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::UiccPinReqVerify
// Creates and sends UICC_PIN_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::UiccPinReqVerify(
    const TUint8 aServiceType,
    const RMobilePhone::TMobilePassword& aCode,
    const RMobilePhone::TMobilePassword& aUnblockCode )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::UiccPinReqVerify");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_UICCPINREQ_TD, "CMmSecurityMessHandler::UiccPinReqVerify" );

    TUint8 numOfSubblocks( 1 ); // in case of PIN verify
    TUint8 pinQualifier( UICC_PIN_OLD ); // in case of PIN verify
    TUint messageOffset( ISI_HEADER_SIZE + SIZE_UICC_PIN_REQ );

    if ( UICC_PIN_UNBLOCK == aServiceType )
        {
        numOfSubblocks = 2;
        pinQualifier = UICC_PIN_NEW;
        }

    // Create UICC_REQ message for querying card status
    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, KSecurityTransId );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, UICC_PIN_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_SERVICETYPE,
        aServiceType );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_APPLID,
        iApplicationId );
    // 3x filler
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 1,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 2,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_NSB,
        numOfSubblocks );

    // In case unblocking request, create UICC_SB_PUK
    if ( UICC_PIN_UNBLOCK == aServiceType )
        {
        TBuf8<KMaxSizeUiccSbPuk> uiccSbPukBuf( 0 );
        TIsiSubBlock uiccSbPuk(
            uiccSbPukBuf,
            UICC_SB_PUK,
            EIsiSubBlockTypeId16Len16 );

        uiccSbPukBuf.Append( iPinId ); // PIN ID
        uiccSbPukBuf.Append( aUnblockCode.Length() ); // Length of PUK code
        uiccSbPukBuf.Append( aUnblockCode ); // PUK code

        // Append subblock to ISI message
        isiMsg.CopyData(
            messageOffset,
            uiccSbPuk.CompleteSubBlock() );

        messageOffset += uiccSbPukBuf.Length();
        }

    // Create succlock UICC_SB_PIN
    TBuf8<KMaxSizeUiccSbPin> uiccSbPinBuf( 0 );
    TIsiSubBlock uiccSbPin(
        uiccSbPinBuf,
        UICC_SB_PIN,
        EIsiSubBlockTypeId16Len16 );

    // PIN ID
    uiccSbPinBuf.Append( iPinId );
    uiccSbPinBuf.Append( pinQualifier ); // PIN qualifier
    uiccSbPinBuf.Append( aCode.Length() ); // Length of PIN code
    uiccSbPinBuf.Append( aCode ); // PIN code

    // Append subblock to ISI message
    isiMsg.CopyData(
        messageOffset,
        uiccSbPin.CompleteSubBlock() );

    return iPhoNetSender->Send( isiMsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::UiccPinResp
// Breaks a UICC_PIN_IND ISI-message.
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::UiccPinResp( const TIsiReceiveC& aIsiMessage )
    {
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_UICCPINRESP_TD, "CMmSecurityMessHandler::UiccPinResp" );
TFLOGSTRING("TSY: CMmSecurityMessHandler::UiccPinResp");

    // Get service type and status
    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_PIN_RESP_OFFSET_SERVICETYPE ) );
    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_PIN_RESP_OFFSET_STATUS ) );

    switch( serviceType )
        {
        case UICC_PIN_VERIFY:
        case UICC_PIN_UNBLOCK:
            {
            HandleUiccPinVerifyResp( status, aIsiMessage );
            CompleteIfCodeVerified( status );
            break;
            }
        case UICC_PIN_INFO:
            {
            HandleUiccPinInfoResp( status, aIsiMessage );
            break;
            }
        case UICC_PIN_CHANGE:
            {
            HandleUiccPinChangeResp( status, aIsiMessage );
            break;
            }
        case UICC_PIN_ENABLE:
        case UICC_PIN_DISABLE:
        case UICC_PIN_SUBSTITUTE:
            {
            HandleUiccPinStateChangeResp( status, aIsiMessage );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::CompleteIfCodeVerified
// Breaks a UICC_PIN_RESP ISI-message.
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::CompleteIfCodeVerified( const TUint8 aStatus )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::CompleteIfCodeVerified");
OstTrace0( TRACE_NORMAL, CMMSECURITYMESSHANDLER_COMPLETEIFCODEVERIFIED_TD, "CMmSecurityMessHandler::CompleteIfCodeVerified" );

    RMobilePhone::TMobilePhoneSecurityEvent event( RMobilePhone::ENoICCFound );

    if ( UICC_STATUS_OK == aStatus )
        {
        switch ( iSecurityCode )
            {
            case RMobilePhone::ESecurityUniversalPin:
                {
                event = RMobilePhone::EUniversalPinVerified;
                break;
                }
            case RMobilePhone::ESecurityCodePin1:
                {
                event = RMobilePhone::EPin1Verified;
                break;
                }
            case RMobilePhone::ESecurityCodePin2:
                {
                event = RMobilePhone::EPin2Verified;
                break;
                }
            case RMobilePhone::ESecurityUniversalPuk:
                {
                event = RMobilePhone::EUniversalPukVerified;
                break;
                }
            case RMobilePhone::ESecurityCodePuk1:
                {
                event = RMobilePhone::EPuk1Verified;
                break;
                }
            case RMobilePhone::ESecurityCodePuk2:
                {
                event = RMobilePhone::EPuk2Verified;
                break;
                }
            default:
                {
OstTrace0( TRACE_NORMAL, DUP1_CMMSECURITYMESSHANDLER_COMPLETEIFCODEVERIFIED_TD, "CMmSecurityMessHandler::CompleteIfCodeVerified - Default" );
TFLOGSTRING("TSY: CMmSecurityMessHandler::CompleteIfCodeVerified - Default!");
                break;
                }

            } //end of switch
        }
    if ( RMobilePhone::ENoICCFound != event )
        {
        // Complete notify security event
        CMmDataPackage dataPackage;
        dataPackage.PackData( &event );

        iMessageRouter->Complete(
            EMobilePhoneNotifySecurityEvent,
            &dataPackage,
            KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::UiccPinReqStateQuery
// Creates and sends UICC_PIN_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::UiccPinReqStateQuery(
    const RMobilePhone::TMobilePhoneLock aLock )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::UiccPinReqStateQuery");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_UICCPINREQSTATEQUERY_TD, "CMmSecurityMessHandler::UiccPinReqStateQuery" );

    TUint8 pinId( 0 );

    if ( RMobilePhone::ELockICC == aLock )
        {
        pinId = iMmUiccMessHandler->GetPin1KeyReference();
        }
    else if ( RMobilePhone::ELockPin2 == aLock )
        {
        pinId = iMmUiccMessHandler->GetPin2KeyReference();
        }
    else
        {
        pinId = 0x11; // UPIN
        }
    // Create UICC_REQ message for querying card status
    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, KSecurityTransId );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, UICC_PIN_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_SERVICETYPE,
        UICC_PIN_INFO );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_APPLID,
        iApplicationId );
    // 3x filler
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 1,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 2,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_NSB,
        1 ); // One subblock, UICC_SB_PIN_REF

    // Create succlock UICC_SB_PIN_REF
    TBuf8<KMaxSizeUiccSbPin> uiccSbPinRefBuf( 0 );
    TIsiSubBlock uiccSbPinRef(
        uiccSbPinRefBuf,
        UICC_SB_PIN_REF,
        EIsiSubBlockTypeId16Len16 );

    // PIN ID
    uiccSbPinRefBuf.Append( pinId );
    uiccSbPinRefBuf.Append( KSecPadding ); // PIN qualifier
    uiccSbPinRefBuf.Append( KSecPadding ); // Length of PIN code
    uiccSbPinRefBuf.Append( KSecPadding ); // PIN code

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_PIN_REQ,
        uiccSbPinRef.CompleteSubBlock() );

    return iPhoNetSender->Send( isiMsg.Complete() );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::UiccPinReqChange
// Creates and sends UICC_PIN_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::UiccPinReqChange(
    RMobilePhone::TMobilePhoneSecurityCode& aType,
    const RMobilePhone::TMobilePassword& aOldCode,
    const RMobilePhone::TMobilePassword& aNewCode )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::UiccPinReqChange");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_UICCPINREQCHANGE_TD, "CMmSecurityMessHandler::UiccPinReqChange" );

    TUint8 pinId( 0 );
    if ( RMobilePhone::ESecurityCodePin1 == aType )
        {
        pinId = iMmUiccMessHandler->GetPin1KeyReference();
        }
    else if ( RMobilePhone::ESecurityCodePin2 == aType )
        {
        pinId = iMmUiccMessHandler->GetPin2KeyReference();
        }
    else
        {
        pinId = KUniversalPinKeyReference;
        }

    // Create UICC_REQ message for changing PIN code
    TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, KSecurityTransId );
    isiMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, UICC_PIN_REQ );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_SERVICETYPE,
        UICC_PIN_CHANGE );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_APPLID,
        iApplicationId );
    // 3x filler
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 1,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 2,
        KSecPadding );
    isiMsg.Set8bit(
        ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_NSB,
        2 ); // Two UICC_SB_PIN subblocks

    // Create succlock UICC_SB_PIN for new PIN code
    TBuf8<KMaxSizeUiccSbPin> uiccSbPinBuf( 0 );
    TIsiSubBlock uiccSbPinOld(
        uiccSbPinBuf,
        UICC_SB_PIN,
        EIsiSubBlockTypeId16Len16 );

    // PIN ID
    uiccSbPinBuf.Append( pinId );
    uiccSbPinBuf.Append( UICC_PIN_OLD ); // PIN qualifier
    uiccSbPinBuf.Append( aOldCode.Length() ); // Length of PIN code
    uiccSbPinBuf.Append( aOldCode ); // PIN code

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_PIN_REQ,
        uiccSbPinOld.CompleteSubBlock() );

    TUint sbLength( uiccSbPinBuf.Length() );

    // Create succlock UICC_SB_PIN for old PIN code
    uiccSbPinBuf.Zero();
    TIsiSubBlock uiccSbPinNew(
        uiccSbPinBuf,
        UICC_SB_PIN,
        EIsiSubBlockTypeId16Len16 );

    // PIN ID
    uiccSbPinBuf.Append( pinId );
    uiccSbPinBuf.Append( UICC_PIN_NEW ); // PIN qualifier
    uiccSbPinBuf.Append( aNewCode.Length() ); // Length of PIN code
    uiccSbPinBuf.Append( aNewCode ); // PIN code

    // Append subblock to ISI message
    isiMsg.CopyData(
        ISI_HEADER_SIZE + SIZE_UICC_PIN_REQ + sbLength,
        uiccSbPinNew.CompleteSubBlock() );

    return iPhoNetSender->Send( isiMsg.Complete() );
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::UiccPinReqChangeState
// Creates and sends UICC_PIN_REQ ISI message.
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::UiccPinReqChangeState(
    const RMobilePhone::TMobilePhoneLock aLock,
    const RMobilePhone::TMobilePhoneLockSetting& aSetting,
    const RMobilePhone::TMobilePassword& aCode )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::UiccPinReqChangeState");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_UICCPINREQCHANGESTATE_TD, "CMmSecurityMessHandler::UiccPinReqChangeState" );

    TInt ret( KErrNone );
    TUint8 pinId( KUniversalPinKeyReference );
    TUint8 pinIdToBeSubstituted( KUniversalPinKeyReference );
    TUint8 pinIdSubstitution( KUniversalPinKeyReference );
    TUint8 setting( UICC_PIN_SUBSTITUTE );
    // Set PIN ID
    if ( RMobilePhone::ELockICC == aLock )
        {
        pinId = iMmUiccMessHandler->GetPin1KeyReference();
        pinIdSubstitution = pinId; // Replace UPIN by PIN
        }
    else if ( RMobilePhone::ELockPin2 == aLock )
        {
        pinId = iMmUiccMessHandler->GetPin2KeyReference();
        }
    else
        {
        pinIdToBeSubstituted = iMmUiccMessHandler->GetPin1KeyReference();
        }

    // Set new state to be changed
    if ( RMobilePhone::ELockSetEnabled == aSetting )
        {
        setting = UICC_PIN_ENABLE;
        }
    else if ( RMobilePhone::ELockSetDisabled == aSetting )
        {
        setting = UICC_PIN_DISABLE;
        }
    else if ( RMobilePhone::ELockReplaced == aSetting )
        {
        // PIN 1 ID is always used in case of substitute
        pinId = iMmUiccMessHandler->GetPin1KeyReference();
        }
    else
        {
        ret = KErrNotSupported;
        }

    if ( KErrNone == ret )
        {
        // Create UICC_REQ message for changing PIN code state
        TIsiSend isiMsg( iPhoNetSender->SendBufferDes() );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_UICC );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_TRANSID, KSecurityTransId );
        isiMsg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, UICC_PIN_REQ );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_SERVICETYPE,
            setting );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_APPLID,
            iApplicationId );
        // 3x filler
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1,
            KSecPadding );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 1,
            KSecPadding );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_FILLERBYTE1 + 2,
            KSecPadding );
        isiMsg.Set8bit(
            ISI_HEADER_SIZE + UICC_PIN_REQ_OFFSET_NSB,
            1 ); // UICC_SB_PIN

        // Create succlock UICC_SB_PIN for PIN code
        TBuf8<KMaxSizeUiccSbPin> uiccSbPinBuf( 0 );
        TIsiSubBlock uiccSbPin(
            uiccSbPinBuf,
            UICC_SB_PIN,
            EIsiSubBlockTypeId16Len16 );
        uiccSbPinBuf.Append( pinId ); // PIN ID
        uiccSbPinBuf.Append( UICC_PIN_OLD ); // PIN qualifier
        uiccSbPinBuf.Append( aCode.Length() ); // Length of PIN code
        uiccSbPinBuf.Append( aCode ); // PIN code
        // Append subblock to ISI message
        isiMsg.CopyData(
            ISI_HEADER_SIZE + SIZE_UICC_PIN_REQ,
            uiccSbPin.CompleteSubBlock() );

        // If request was to replace PIN by UPIN or vice versa
        // subblock UICC_SB_PIN_SUBST is also added
        if ( UICC_PIN_SUBSTITUTE == setting )
            {
            TBuf8<SIZE_UICC_SB_PIN_SUBST> uiccSbPinSubstBuf( 0 );
            TIsiSubBlock uiccSbPinSubst(
                uiccSbPinSubstBuf,
                UICC_SB_PIN_SUBST,
                EIsiSubBlockTypeId16Len16 );
            uiccSbPinSubstBuf.Append( pinIdToBeSubstituted );
            uiccSbPinSubstBuf.Append( pinIdSubstitution );
            uiccSbPinSubstBuf.Append( KSecPadding );
            uiccSbPinSubstBuf.Append( KSecPadding );
            // Append subblock to ISI message
            isiMsg.CopyData(
                ISI_HEADER_SIZE + SIZE_UICC_PIN_REQ + uiccSbPinBuf.Length(),
                uiccSbPinSubst.CompleteSubBlock() );
            }

        ret = iPhoNetSender->Send( isiMsg.Complete() );
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::HandleUiccPinVerifyResp
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::HandleUiccPinVerifyResp(
    const TUint8 aStatus,
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::HandleUiccPinVerifyResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_HANDLEUICCPINVERIFYRESP_TD, "CMmSecurityMessHandler::HandleUiccPinVerifyResp" );

    TInt ret( KErrNone );

    if ( UICC_STATUS_OK != aStatus )
        {
        ret = KErrGeneral;
        // Subblock UICC_SB_STATUS_WORD contains cause information
        TUint uiccSbStatusWordOffset( 0 );
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_UICC_PIN_RESP,
            UICC_SB_STATUS_WORD,
            EIsiSubBlockTypeId16Len16,
            uiccSbStatusWordOffset ) )
            {
            TUint8 sw1( aIsiMessage.Get8bit(
                uiccSbStatusWordOffset +
                UICC_SB_STATUS_WORD_OFFSET_SW1 ) );
            TUint8 sw2( aIsiMessage.Get8bit(
                uiccSbStatusWordOffset +
                UICC_SB_STATUS_WORD_OFFSET_SW2 ) );

            // Get status words and map to symbian error codes
            TUint16 statusWord( sw1 << 8 | sw2 );

            // Map status word to symbian error codes. See status word coding from
            // TS 102.221 V7.11.0 Chapter 10.2.1

            // 0x63CX: PIN was not correct and there are 'X' retries left
            if ( 0x63 == sw1 && 0x0C == ( sw2 >> 4 ) )
                {
                ret = CMmStaticUtility::EpocErrorCode(
                    KErrAccessDenied,
                    KErrGsm0707IncorrectPassword );
                }
            else if ( 0x6983 == statusWord ) // Authentication/PIN method blocked
                {
                ret = CMmStaticUtility::EpocErrorCode(
                    KErrAccessDenied,
                    KErrGsmSSPasswordAttemptsViolation );
                }
            // No else, KErrGeneral is returned
            }
        } // End of if ( UICC_STATUS_OK != status )

    iMessageRouter->Complete( EMobilePhoneVerifySecurityCode, ret );
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::HandleUiccPinInfoResp
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::HandleUiccPinInfoResp(
    const TUint8 aStatus,
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::HandleUiccPinInfoResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_HANDLEUICCPININFORESP_TD, "CMmSecurityMessHandler::HandleUiccPinInfoResp" );

    TInt ret( KErrNone );
    CMmDataPackage dataPackage;
    RMobilePhone::TMobilePhoneLockInfoV1 lockInfo;
    lockInfo.iStatus = RMobilePhone::EStatusLockUnknown;
    lockInfo.iSetting = RMobilePhone::ELockSetUnknown;
    RMobilePhone::TMobilePhoneSecurityCodeInfoV5 securityInfo;
    TUint8 pinStatus( UICC_STATUS_UNKNOWN );
    TUint8 pinAttemptsLeft( 0 );
    TUint8 pukAttemptsLeft( 0 );

    // Subblock UICC_SB_PIN_INFO
    TUint uiccSbPinInfoOffset( 0 );
    if ( UICC_STATUS_OK == aStatus &&
        KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_PIN_RESP,
        UICC_SB_PIN_INFO,
        EIsiSubBlockTypeId16Len16,
        uiccSbPinInfoOffset ) )
        {
        pinStatus = aIsiMessage.Get8bit(
            uiccSbPinInfoOffset +
            UICC_SB_PIN_INFO_OFFSET_PINSTATUS );
        pinAttemptsLeft = aIsiMessage.Get8bit(
            uiccSbPinInfoOffset +
            UICC_SB_PIN_INFO_OFFSET_PINATT );
        pukAttemptsLeft = aIsiMessage.Get8bit(
            uiccSbPinInfoOffset +
            UICC_SB_PIN_INFO_OFFSET_PUKATT );

        // This is PIN attempts left query
        if ( iPinAttemptsLeftQuery )
            {
            // PIN codes
            if ( RMobilePhone::ESecurityCodePin1 == iCodeType ||
                RMobilePhone::ESecurityCodePin2 == iCodeType ||
                RMobilePhone::ESecurityUniversalPin == iCodeType )
                {
                securityInfo.iRemainingEntryAttempts = pinAttemptsLeft;
                }
            else // PUK codes
                {
                securityInfo.iRemainingEntryAttempts = pukAttemptsLeft;
                }
            }
        if ( iLockStateQuery ) // This is lock state query
            {
            // Set PIN setting
            if ( UICC_STATUS_PIN_ENABLED == pinStatus)
                {
                lockInfo.iSetting = RMobilePhone::ELockSetEnabled;
                }
            else if ( UICC_STATUS_PIN_DISABLED == pinStatus )
                {
                lockInfo.iSetting = RMobilePhone::ELockSetDisabled;
                }
            // Set PIN status
            if ( 0 != pinAttemptsLeft )
                {
                lockInfo.iStatus = RMobilePhone::EStatusUnlocked;
                }
            else
                {
                lockInfo.iStatus = RMobilePhone::EStatusBlocked;
                }
            dataPackage.PackData( &lockInfo.iStatus, & lockInfo.iSetting );
            }
        }
    else // UICC status was not OK or subblock was not found
        {
        ret = KErrNotFound;
        }

    // Complete PIN attempts left query
    if ( iPinAttemptsLeftQuery )
        {
        iPinAttemptsLeftQuery = EFalse;
        dataPackage.PackData( &iCodeType, &securityInfo );
        iMessageRouter->Complete(
            EMobilePhoneGetSecurityCodeInfo,
            &dataPackage,
            ret );
        iPinAttemptsLeftQuery = EFalse;
        }
    if ( iLockStateQuery ) // Complete PIN state query
        {
        dataPackage.PackData( &lockInfo.iStatus, & lockInfo.iSetting );
        // Complete PIN info
        iMessageRouter->Complete(
            EMobilePhoneGetLockInfo,
            &dataPackage,
            ret );
        iLockStateQuery = EFalse;
        }
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::HandleUiccPinChangeResp
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::HandleUiccPinChangeResp(
    const TUint8 aStatus,
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::HandleUiccPinChangeResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_HANDLEUICCPINCHANGERESP_TD, "CMmSecurityMessHandler::HandleUiccPinChangeResp" );

    TInt ret( KErrNone );
    if ( UICC_STATUS_OK != aStatus )
        {
        ret = KErrNotSupported;
        // Subblock UICC_SB_STATUS_WORD contains cause information
        TUint uiccSbStatusWordOffset( 0 );
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_UICC_PIN_RESP,
            UICC_SB_STATUS_WORD,
            EIsiSubBlockTypeId16Len16,
            uiccSbStatusWordOffset ) )
            {
            TUint8 sw1( aIsiMessage.Get8bit(
                uiccSbStatusWordOffset +
                UICC_SB_STATUS_WORD_OFFSET_SW1 ) );
            TUint8 sw2( aIsiMessage.Get8bit(
                uiccSbStatusWordOffset +
                UICC_SB_STATUS_WORD_OFFSET_SW2 ) );

            // Get status words and map to symbian error codes
            TUint16 statusWord( sw1 << 8 | sw2 );

            // Map status word to symbian error codes. See status word coding
            // from TS 102.221 V7.11.0 Chapter 10.2.1

            // 0x63CX: PIN was not correct and there are 'X' retries left
            if ( 0x63 == sw1 && 0x0C == ( sw2 >> 4 ) )
                {
                ret = CMmStaticUtility::EpocErrorCode(
                    KErrAccessDenied,
                    KErrGsm0707IncorrectPassword );
                }
            else if ( 0x6983 == statusWord ) // Authent./PIN method blocked
                {
                ret = CMmStaticUtility::EpocErrorCode(
                    KErrLocked,
                    KErrGsmSSPasswordAttemptsViolation );
                }
            else if ( 0x6A81 == statusWord ) // Wrong parameters
                {
                ret = CMmStaticUtility::EpocErrorCode(
                    KErrArgument,
                    KErrGsmInvalidParameter );
                }
            // No else, KErrNotSupported is returned
            }
        }
    iMessageRouter->Complete ( EMobilePhoneChangeSecurityCode, ret );
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::HandleUiccPinStateChangeResp
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::HandleUiccPinStateChangeResp(
    const TUint8 aStatus,
    const TIsiReceiveC& aIsiMessage )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::HandleUiccPinStateChangeResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_HANDLEUICCPINSTATECHANGERESP_TD, "CMmSecurityMessHandler::HandleUiccPinStateChangeResp" );

    TInt ret( KErrNone );
    // Status and setting values are not used in CTSY, set to 'unknown'
    RMobilePhone::TMobilePhoneLockStatus status(
        RMobilePhone::EStatusLockUnknown );
    RMobilePhone::TMobilePhoneLockSetting setting(
        RMobilePhone::ELockSetUnknown );

    TUint8 serviceType( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_PIN_RESP_OFFSET_SERVICETYPE ) );

    if ( UICC_STATUS_OK != aStatus )
        {
        ret = KErrNotSupported;
        // Subblock UICC_SB_STATUS_WORD contains cause information
        TUint uiccSbStatusWordOffset( 0 );
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_UICC_PIN_RESP,
            UICC_SB_STATUS_WORD,
            EIsiSubBlockTypeId16Len16,
            uiccSbStatusWordOffset ) )
            {
            TUint8 sw1( aIsiMessage.Get8bit(
                uiccSbStatusWordOffset +
                UICC_SB_STATUS_WORD_OFFSET_SW1 ) );
            TUint8 sw2( aIsiMessage.Get8bit(
                uiccSbStatusWordOffset +
                UICC_SB_STATUS_WORD_OFFSET_SW2 ) );

            // Get status words and map to symbian error codes
            TUint16 statusWord( sw1 << 8 | sw2 );

            // Map status word to symbian error codes. See status word coding from
            // TS 102.221 V7.11.0 Chapter 10.2.1

            // 0x63CX: PIN was not correct and there are 'X' retries left
            if ( 0x63 == sw1 && 0x0C == ( sw2 >> 4 ) )
                {
                ret = CMmStaticUtility::EpocErrorCode(
                    KErrAccessDenied,
                    KErrGsm0707IncorrectPassword );
                }
            else if ( 0x6983 == statusWord ) // Authentication/PIN method blocked
                {
                ret = CMmStaticUtility::EpocErrorCode(
                    KErrAccessDenied,
                    KErrGsmSSPasswordAttemptsViolation );
                }
            // No else, KErrNotSupported is returned
            }
        }
        else if ( UICC_PIN_SUBSTITUTE == serviceType )
            {
            // If response status is OK PIN/UPIN was substituted.
            // Update active pin
            iMmUiccMessHandler->ChangeActivePin();
            }

    CMmDataPackage dataPackage;
    dataPackage.PackData( &status, &setting );
    iMessageRouter->Complete(
         EMobilePhoneSetLockSetting,
         &dataPackage,
         ret );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::GetIccType
// Read ICC type and completes it
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::GetIccType()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::GetIccType");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_GETICCTYPE_TD, "CMmSecurityMessHandler::GetIccType" );

    TInt ret( KErrNone );
    TICCType type ( EICCTypeSimUnknown );
    TUint8 applicationType( iMmUiccMessHandler->GetApplicationType() );

    if ( UICC_APPL_TYPE_UICC_USIM == applicationType )
        {
        type = EICCTypeSim3G;
        }
    else if ( UICC_APPL_TYPE_ICC_SIM == applicationType )
        {
        type = EICCTypeSim2GGsm;
        }
    else
        {
        ret = KErrGeneral;
        }

    // Complete the request
    CMmDataPackage dataPackage;
    dataPackage.PackData( &type );

    iMessageRouter->Complete( EMmTsySimGetICCType, &dataPackage, ret );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::GetActivePin
// Reads active PIN and completes it
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::GetActivePin()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::GetActivePin");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_GETACTIVEPIN_TD, "CMmSecurityMessHandler::GetActivePin" );
    RMobilePhone::TMobilePhoneSecurityCode activePin(
        iMmUiccMessHandler->GetActivePin());
    CMmDataPackage dataPackage;
    dataPackage.PackData( &activePin );
    iMessageRouter->Complete(
        EMmTsySecurityGetSimActivePinStateIPC,
        &dataPackage,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::GetActiveUsimApplication
// Read AID of active USIM application and complete
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::GetActiveUsimApplication()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::GetActiveUsimApplication");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_GETACTIVEUSIMAPPLICATION_TD, "CMmSecurityMessHandler::GetActiveUsimApplication" );

    RMobilePhone::TAID aid( iMmUiccMessHandler->GetAid() );
    CMmDataPackage dataPackage;
    dataPackage.PackData( &aid );
    iMessageRouter->Complete(
        EMobilePhoneGetCurrentActiveUSimApplication,
        &dataPackage,
        KErrNone );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::GetPukCodeReq
// Returns iPukCodeRequired
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::GetPukCodeReq()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::GetPukCodeReq");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_GETPUKCODEREQ_TD, "CMmSecurityMessHandler::GetPukCodeReq" );
    return iPukCodeRequired;
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::SetFdnStateUicc
// sets FDN state in case of UICC card
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::ReadEfEst( TUiccTrId aTraId )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::ReadEfEst");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_READEFEST_TD, "CMmSecurityMessHandler::ReadEfEst" );

    // Set parameters for UICC_APPL_CMD_REQ message
    TUiccReadTransparent params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.trId = aTraId;
    params.dataAmount = 0;
    params.dataOffset = 0;
    params.fileId = KElemEst;
    params.fileIdSfi = 0x05;
    params.serviceType = UICC_APPL_READ_TRANSPARENT;

    // File id path
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params ); 
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::ProcessUiccMsg
// Handles data received from UICC server
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::ProcessUiccMsg(
    TInt aTraId,
    TInt aStatus,
    TUint8 /*aDetails*/,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::ProcessUiccMsg");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_PROCESSUICCMSG_TD, "CMmSecurityMessHandler::ProcessUiccMsg" );

    TInt ret( KErrNone );

    switch( aTraId )
        {
        case ETrIdSetFdnStateReadEst:
            {
            FdnSetReadEfEstResp( aStatus, aFileData );
            break;
            }
        case ETrIdGetFdnStateReadEst:
            {
            FdnGetReadEfEstResp( aStatus, aFileData );
            break;
            }
        case ETrIdSetFdnStateWriteEst:
            {
            WriteEfEstResp( aStatus );
            break;
            }
        case ETrIdSetFdnIcc:
            {
            FdnStateCommandResp( aStatus, aFileData );
            break;
            }
        case ETrIdSetFdnStateReadFileInfo:
            {
            FdnSetReadEfAdnFileInfoResp( aStatus, aFileData );
            break;
            }
        case ETrIdGetFdnStateReadFileInfo:
            {
            FdnGetReadEfAdnFileInfoResp( aStatus, aFileData );
            break;
            }
        default:
            {
TFLOGSTRING("TSY: CMmSecurityMessHandler::ProcessUiccMsg - unknown transaction ID" );
OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_PROCESSUICCMSG_TD, "CMmSecurityMessHandler::ProcessUiccMsg - unknown transaction ID" );
            break;
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::FdnSetReadEfEstResp
// Handles response for EFest reading in case of setting FDN state
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::FdnSetReadEfEstResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnSetReadEfEstResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_FDNSETREADEFESTRESP_TD, "CMmSecurityMessHandler::FdnSetReadEfEstResp" );

    if( UICC_STATUS_OK == aStatus )
        {
        TUint8 fdnState( aFileData[0] & KFdnStateMask );
        TUint8 fdnStateToBeSet( 0 );

        if( iMmUiccMessHandler->GetServiceStatus( KUiccServiceFdn ) )
            {
            if( RMobilePhone::EFdnSetOn == iFdnSetting )
                {
                fdnStateToBeSet = KServiceActivated;
                }
            else
                {
                fdnStateToBeSet = KServiceNotActivated;
                }

            if( fdnState != fdnStateToBeSet )
                {
                // update the EFest
                TUiccWriteTransparent params;
                params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
                params.trId = ETrIdSetFdnStateWriteEst;
                params.dataOffset = 0;
                params.dataAmount = 1; // only one byte is update
                params.fileId = KElemEst;
                params.fileIdSfi = 0x05;
                params.serviceType = UICC_APPL_UPDATE_TRANSPARENT;
                // File id path
                params.filePath.Append( KMasterFileId >> 8 );
                params.filePath.Append( KMasterFileId );
                params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );

                // File data to be updated.
                TUint8 data = aFileData[0] & 0xFE;
                data += fdnStateToBeSet;
                params.fileData.Append( data );

                iMmUiccMessHandler->CreateUiccApplCmdReq( params );            
                }
            else
                {
                // state is already correct, let's just complete the request
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnSetReadEfEstResp: FDN state already correct, let's complete");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_FDNSETREADEFESTRESP_TD, "CMmSecurityMessHandler::FdnSetReadEfEstResp: FDN state already correct, let's complete" );
                iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrNone );
                }
            }
        else
            {
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnSetReadEfEstResp: FDN state not supported in EFust");
OstTrace0( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_FDNSETREADEFESTRESP_TD, "CMmSecurityMessHandler::FdnSetReadEfEstResp: FDN state not supported in EFust" );
            iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrNotSupported );
            }
        }
    else
        {
        // error in reading EFest, let's complete the request
TFLOGSTRING2("TSY: CMmSecurityMessHandler::FdnSetReadEfEstResp: reading failed, 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_FDNSETREADEFESTRESP_TD, "CMmSecurityMessHandler::FdnSetReadEfEstResp: reading failed, 0x%x", aStatus );

        // compete setting of FDN state 
        iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::FdnGetReadEfEstResp
// Handles response for EFest reading in case of getting FDN state
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::FdnGetReadEfEstResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnGetReadEfEstResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_FDNGETREADEFESTRESP_TD, "CMmSecurityMessHandler::FdnGetReadEfEstResp" );

    RMobilePhone::TMobilePhoneFdnStatus fdnSetting( RMobilePhone::EFdnNotActive );

    if( UICC_STATUS_OK == aStatus )
        {
        TUint8 fdnState( aFileData[0] & KFdnStateMask );
        
        // let's check that FDN is supported in EFust
        if( iMmUiccMessHandler->GetServiceStatus( KUiccServiceFdn ) )
            {
            if( KServiceActivated == fdnState )
                {
                fdnSetting = RMobilePhone::EFdnActive;
                }
            // no else because of in this case FDN is not active
            // and fdnSetting is already set to state not active.
            }
        else
            {
            // FDN not supported in EFust
            fdnSetting = RMobilePhone::EFdnNotSupported;
            }
        CMmDataPackage dataPackage;
        dataPackage.PackData ( &fdnSetting );
        // Complete the status to the client
        iMessageRouter->Complete( EMobilePhoneGetFdnStatus, &dataPackage, KErrNone );
        }
    else
        {
        // error in reading EFest, let's complete the request
TFLOGSTRING2("TSY: CMmSecurityMessHandler::FdnGetReadEfEstResp: reading failed, 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_FDNGETREADEFESTRESP_TD, "CMmSecurityMessHandler::FdnGetReadEfEstResp: reading failed, 0x%x", aStatus );

        fdnSetting  = RMobilePhone::EFdnNotSupported;
        CMmDataPackage dataPackage;
        dataPackage.PackData ( &fdnSetting );
        iMessageRouter->Complete( EMobilePhoneGetFdnStatus, &dataPackage, KErrGeneral );
        }
    }


// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::HandleWriteEfEstResp
// Handles response for EFest update
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::WriteEfEstResp( const TInt aStatus )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::WriteEfEstResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_WRITEEFESTRESP_TD, "CMmSecurityMessHandler::WriteEfEstResp" );

    if( UICC_STATUS_OK == aStatus )
        {
TFLOGSTRING("TSY: CMmSecurityMessHandler::WriteEfEstResp: FDN state set succesfully");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_WRITEEFESTRESP_TD, "CMmSecurityMessHandler::WriteEfEstResp: FDN state set succesfully" );
        iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrNone );
        }
    else
        {
TFLOGSTRING("TSY: CMmSecurityMessHandler::WriteEfEstResp: FDN state set failed");
OstTrace0( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_WRITEEFESTRESP_TD, "CMmSecurityMessHandler::WriteEfEstResp: FDN state set failed" );
        iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::SendFdnStateCommand
// sends INVALIDATE/REHABILITATE command for setting FDN state
// in case of ICC card
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::SendFdnStateCommand()
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::SendFdnStateCommand");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_SENDFDNSTATECOMMAND_TD, "CMmSecurityMessHandler::SendFdnStateCommand" );

    TInt ret( 0 );
    TUiccSendApdu params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.serviceType = UICC_APPL_APDU_SEND;
    params.fileId = UICC_EF_ID_NOT_PRESENT;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
    params.trId = ETrIdSetFdnIcc;
    params.apdu.Append( 0xA0 );             // CLA

    if( RMobilePhone::EFdnSetOn == iFdnSetting )
        {
        //INVALIDATE
        params.apdu.Append( 0x04 ); // INS
        params.apdu.Append( 0 ); // P1
        params.apdu.Append( 0 ); // P2
        params.apdu.Append( 2 ); // Lc
        params.apdu.Append( KElemAdnIcc >> 8 ); // File id
        params.apdu.Append( KElemAdnIcc ); // File id
        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    else if( RMobilePhone::EFdnSetOff == iFdnSetting )
        {
        // REHABILITATE
        params.apdu.Append( 0x44 ); // INS
        params.apdu.Append( 0 ); // P1
        params.apdu.Append( 0 ); // P2
        params.apdu.Append( 2 ); // Lc
        params.apdu.Append( KElemAdnIcc >> 8 ); // File id
        params.apdu.Append( KElemAdnIcc ); // File id
        ret = iMmUiccMessHandler->CreateUiccApplCmdReq( params );
        }
    else
        {
TFLOGSTRING("TSY: CMmSecurityMessHandler::SendFdnStateCommand: unknown FDN state");
        OstTrace0( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_SENDFDNSTATECOMMAND_TD, "CMmSecurityMessHandler::SendFdnStateCommand: unknown FDN state" );
        ret = KErrArgument;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::FdnStateCommandResp
// handles response to the INVALIDATE/REHABILITATE command in case of ICC card
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::FdnStateCommandResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::SendFdnStateCommand");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_FDNSTATECOMMANDRESP_TD, "CMmSecurityMessHandler::FdnStateCommandResp" );

    if( UICC_STATUS_OK == aStatus )
        {
        // get the status word
        TUint8 sw1( aFileData[aFileData.Length() - KSw1Position] );
        TUint8 sw2( aFileData[aFileData.Length() - KSw2Position] );

TFLOGSTRING3("TSY: CMmSecurityMessHandler::SendFdnStateCommand: sw1: 0x%x, sw2: 0x%x", sw1, sw2);
OstTraceExt2( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_FDNSTATECOMMANDRESP_TD, "CMmSecurityMessHandler::FdnStateCommandResp: sw1: 0x%x, sw2: 0x%x", sw1, sw2 );

        TInt ret( KErrGeneral );

        if( ( 0x90 == sw1 && 0x00 == sw2 ) ||
            ( 0x91 == sw1 ) )
            {
            ret = KErrNone;
            }
        else if( 0x69 == sw1 )
            {
            ret = KErrAccessDenied;
            }
        iMessageRouter->Complete( EMobilePhoneSetFdnSetting, ret );
        }
    else
        {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::SendFdnStateCommand, Fdn state set failed: 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_FDNSTATECOMMANDRESP_TD, "CMmSecurityMessHandler::FdnStateCommandResp, Fdn state set failed: 0x%x", aStatus );
        iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::ReadEfAdnFileInfo
// Sends file info request for EFadn in case of ICC card
// -----------------------------------------------------------------------------
//
TInt CMmSecurityMessHandler::ReadEfAdnFileInfo( TUiccTrId aTraId )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::ReadEfAdnFileInfo");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_READEFADNFILEINFO_TD, "CMmSecurityMessHandler::ReadEfAdnFileInfo" );

    TUiccApplFileInfo params;
    params.messHandlerPtr = static_cast<MUiccOperationBase*>( this );
    params.serviceType = UICC_APPL_FILE_INFO;
    params.fileId = KElemAdnIcc;
    params.fileIdSfi = UICC_SFI_NOT_PRESENT;
    params.filePath.Append( KMasterFileId >> 8 );
    params.filePath.Append( KMasterFileId );
    params.filePath.Append( iMmUiccMessHandler->GetApplicationFileId() );
    params.trId = aTraId;

    return iMmUiccMessHandler->CreateUiccApplCmdReq( params );
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp
// Handles response for file info reading for EFadn in case of ICC card
// when FDN state is set
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_FDNSETREADEFADNFILEINFORESP_TD, "CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp" );

    if( UICC_STATUS_OK == aStatus )
        {
        // let's check is ADN and FDN supported in EFest
        if( iMmUiccMessHandler->GetServiceStatus( KIccServiceFdn ) &&
            iMmUiccMessHandler->GetServiceStatus( KIccServiceAdn ) )
            {
            // let's check the current FDN status. 
            // If EFadn is invalidated, FDN is enabled, otherwise FDN
            // is disabled
            TFci fci( aFileData );
            TUint8 invalidationFlag( fci.GetFileStatus() & KInvalidateFlagMask );

            if( ( KInvalidated == invalidationFlag && 
                  RMobilePhone::EFdnSetOn == iFdnSetting ) ||
                ( KNotInvalidated == invalidationFlag &&
                  RMobilePhone::EFdnSetOff == iFdnSetting ) )
                {
                // Current FDN state is already correct, so we can 
                // complete the request
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp: FDN state already correct, let's complete");
OstTrace0( TRACE_NORMAL,  DUP2_CMMSECURITYMESSHANDLER_FDNSETREADEFADNFILEINFORESP_TD, "CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp: FDN state already correct, let's complete" );
                iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrNone );
                }
            else
                {
                // Current FDN state is not correct, we need to change it
                SendFdnStateCommand();
                }
            }
        else
            {
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp: FDN or ADN not supported in EFsst");
OstTrace0( TRACE_NORMAL,  DUP3_CMMSECURITYMESSHANDLER_FDNSETREADEFADNFILEINFORESP_TD, "CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp: FDN or ADN not supported in EFsst" );

                iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrNotSupported );
            }
        }
    else
        {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp: reading failed: 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_FDNSETREADEFADNFILEINFORESP_TD, "CMmSecurityMessHandler::FdnSetReadEfAdnFileInfoResp: reading failed: 0x%x", aStatus );

        iMessageRouter->Complete( EMobilePhoneSetFdnSetting, KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CMmSecurityMessHandler::FdnGetReadEfAdnFileInfoResp
// Handles response for file info reading for EFadn in case of ICC card
// when FDN state is get
// -----------------------------------------------------------------------------
//
void CMmSecurityMessHandler::FdnGetReadEfAdnFileInfoResp(
    TInt aStatus,
    const TDesC8& aFileData )
    {
TFLOGSTRING("TSY: CMmSecurityMessHandler::FdnGetReadEfAdnFileInfoResp");
OstTrace0( TRACE_NORMAL,  CMMSECURITYMESSHANDLER_FDNGETREADEFADNFILEINFORESP_TD, "CMmSecurityMessHandler::FdnGetReadEfAdnFileInfoResp" );

    RMobilePhone::TMobilePhoneFdnStatus fdnSetting( RMobilePhone::EFdnNotActive );

    if( UICC_STATUS_OK == aStatus )
        {
        if( iMmUiccMessHandler->GetServiceStatus( KIccServiceFdn ) &&
            iMmUiccMessHandler->GetServiceStatus( KIccServiceAdn ) )
            {
            TFci fci( aFileData );
            TUint8 invalidationFlag( fci.GetFileStatus() & KInvalidateFlagMask );

            if( KInvalidated == invalidationFlag )
                {
                fdnSetting = RMobilePhone::EFdnActive;
                }
            // no else because of FDN is not active and fdnSetting is 
            // already set to this value.
            }
        else
            {
            fdnSetting = RMobilePhone::EFdnNotSupported;
            }

        CMmDataPackage dataPackage;
        dataPackage.PackData ( &fdnSetting );
        // Complete the status to the client
        iMessageRouter->Complete( EMobilePhoneGetFdnStatus, &dataPackage, KErrNone );
        }
    else
        {
TFLOGSTRING2("TSY: CMmSecurityMessHandler::FdnGetReadEfAdnFileInfoResp: reading failed: 0x%x", aStatus);
OstTrace1( TRACE_NORMAL,  DUP1_CMMSECURITYMESSHANDLER_FDNGETREADEFADNFILEINFORESP_TD, "CMmSecurityMessHandler::FdnGetReadEfAdnFileInfoResp: reading failed: 0x%x", aStatus );

        fdnSetting = RMobilePhone::EFdnNotSupported;
        CMmDataPackage dataPackage;
        dataPackage.PackData ( &fdnSetting );
        iMessageRouter->Complete( EMobilePhoneGetFdnStatus, &dataPackage, KErrNotSupported );
        }
    }

//  End of File
