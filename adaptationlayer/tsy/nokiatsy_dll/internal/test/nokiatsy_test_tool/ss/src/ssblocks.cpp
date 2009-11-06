/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Specific class for SS tests
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include <stifparser.h>
#include <stiftestinterface.h>
#include <mmlist.h>   // list types
#include "ss.h"

// CONSTANTS
_LIT8( KLine, "---------------------------------" );
_LIT( KDontCare, "DONT_CARE");

// Indicates what kind IccMessageWaitingIndicators is on going
enum TIccMessageWaitingIndicatorsType
    {
    EIccIndicationRel4 = 1,
    EIccIndicationCPHS
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIM::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CSS::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CSIM::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CSS::RunMethodL(
    CStifItemParser& aItem )
    {
    _LIT8 (KLog, "SS: RunMethodL" );
    iSSLog->Log((TDesC8)KLog);

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "GetCallBarringStatus",             CSS::GetCallBarringStatus ),
        ENTRY( "GetCallForwardingStatus",          CSS::GetCallForwardingStatus ),
        ENTRY( "GetCallWaitingStatus",             CSS::GetCallWaitingStatus),
        ENTRY( "GetIdentityServiceStatus",         CSS::GetIdentityServiceStatus),
        ENTRY( "SetCallBarringStatus",             CSS::SetCallBarringStatus),
        ENTRY( "SetCallForwardingStatus",          CSS::SetCallForwardingStatus),
        ENTRY( "SetCallWaitingStatus",             CSS::SetCallWaitingStatus),
        ENTRY( "SetSupplementaryServicesPassword", CSS::SetSupplementaryServicesPassword),
        ENTRY( "GetCallForwardingNumber",          CSS::GetCallForwardingNumber),
        ENTRY( "SetIccMessageWaitingIndicators",   CSS::SetIccMessageWaitingIndicators),
        ENTRY( "GetIccMessageWaitingIndicators",   CSS::GetIccMessageWaitingIndicators),
        ENTRY( "GetIccCallForwardingStatus",       CSS::GetIccCallForwardingStatus),
        ENTRY( "SendUSSDMessage",                  CSS::SendUSSDMessage),
        ENTRY( "RunIscTestCase",                   CCore::RunIscTestCase),
        ENTRY( "SyncIscTestCase",                  CCore::SyncIscTestCase),
        ENTRY( "SSServiceFailed",                  CSS::SSServiceFailed),
        ENTRY( "USSDSendRelease",                  CSS::USSDSendRelease),
        ENTRY( "SendNetworkServiceRequest",        CSS::SendNetworkServiceRequest),
        ENTRY( "SendNetworkServiceRequestNoFdnCheck",
                CSS::SendNetworkServiceRequestNoFdnCheck),
        ENTRY( "SendSatNotifyCallControlRequest",  CSS::SendSatNotifyCallControlRequest),
        ENTRY( "SetSendNetworkServiceRequestTestOngoingFlag",
                CSS::SetSendNetworkServiceRequestTestOngoingFlag),
        ENTRY( "RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined",
                CCore::RunIscTestCaseIgnoreXMLsWhenRealCellmoInUseFlagIsDefined),
        ENTRY( "SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined",
                CCore::SyncIscTestCaseDoNotSyncIfRealCellmoInUseFlagIsDefined),
        ENTRY( "HandleSpecifiedRequests", CCore::HandleSpecifiedRequests),
        ENTRY( "InitializeIPCRequestTable", CCore::InitializeIPCRequestTable),
        ENTRY( "SetExpectedCompleteIPC", CCore::SetExpectedCompleteIPC),
        };


    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CSS::GetCallBarringStatus
// Getting call barring status
// -----------------------------------------------------------------------------
//
TInt CSS::GetCallBarringStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::GetCallBarringStatus");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString ( string );

    RMobilePhone::TMobilePhoneCBCondition condition;

    if ( _L("BarAllIncoming") == string )
      {
      condition = RMobilePhone::EBarAllIncoming;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EBarAllIncoming") == string )
              {
              iCBInfoEntry.iCondition = RMobilePhone::EBarAllIncoming;
              }

          aItem.GetNextString ( string );
          if ( _L("EShortMessageService") == string )
              {
              iCBInfoEntry.iServiceGroup = RMobilePhone::EShortMessageService;
              }

          aItem.GetNextString ( string );
          if ( _L("ECallBarringStatusActive") == string )
              {
              iCBInfoEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
              }
          }
      }
    else if ( _L("BarIncomingRoaming") == string )
      {
      condition = RMobilePhone::EBarIncomingRoaming;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EBarIncomingRoaming") == string )
              {
              iCBInfoEntry.iCondition = RMobilePhone::EBarIncomingRoaming;
              }

          aItem.GetNextString ( string );
          if ( _L("EAllServices") == string )
              {
              iCBInfoEntry.iServiceGroup = RMobilePhone::EAllServices;
              }

          aItem.GetNextString ( string );
          if ( _L("ECallBarringStatusNotActive") == string )
              {
              iCBInfoEntry.iStatus = RMobilePhone::ECallBarringStatusNotActive;
              }
          }
      }
    else if ( _L("BarAllOutgoing") == string )
      {
      condition = RMobilePhone::EBarAllOutgoing;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EBarAllOutgoing") == string )
              {
              iCBInfoEntry.iCondition = RMobilePhone::EBarAllOutgoing;
              }

          aItem.GetNextString ( string );
          if ( _L("EAllServices") == string )
              {
              iCBInfoEntry.iServiceGroup = RMobilePhone::EAllServices;
              }

          aItem.GetNextString ( string );
          if ( _L("ECallBarringStatusNotActive") == string )
              {
              iCBInfoEntry.iStatus = RMobilePhone::ECallBarringStatusNotActive;
              }
          }
      }
    else if ( _L("BarOutgoingInternational") == string )
      {
      condition = RMobilePhone::EBarOutgoingInternational;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EBarOutgoingInternational") == string )
              {
              iCBInfoEntry.iCondition = RMobilePhone::EBarOutgoingInternational;
              }

          aItem.GetNextString ( string );
          if ( _L("EAllServices") == string )
              {
              iCBInfoEntry.iServiceGroup = RMobilePhone::EAllServices;
              }

          aItem.GetNextString ( string );
          if ( _L("ECallBarringStatusNotActive") == string )
              {
              iCBInfoEntry.iStatus = RMobilePhone::ECallBarringStatusNotActive;
              }
          }
      }
    else if ( _L("BarOutgoingInternationalExHC") == string )
      {
      condition = RMobilePhone::EBarOutgoingInternationalExHC;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EBarOutgoingInternationalExHC") == string )
              {
              iCBInfoEntry.iCondition = RMobilePhone::EBarOutgoingInternationalExHC;
              }

          aItem.GetNextString ( string );
          if ( _L("EAllServices") == string )
              {
              iCBInfoEntry.iServiceGroup = RMobilePhone::EAllServices;
              }

          aItem.GetNextString ( string );
          if ( _L("ECallBarringStatusNotActive") == string )
              {
              iCBInfoEntry.iStatus = RMobilePhone::ECallBarringStatusNotActive;
              }
          }
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected call barring not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }

    if( supported )
      {
       //Create package
      CMmDataPackage package;
      // Set package data
      package.PackData( &condition );

      ret = HandleRequestL( EMobilePhoneGetBarringStatusPhase1,
                            &package );

      if ( KErrNone == ret )
          {
          _LIT8( KSendOk, "GetCallBarringStatus request send ok" );
          iSSLog->Log((TDesC8)KSendOk );
          }
      else
          {
           _LIT8( KSendFailed, "GetCallBarringStatus request send failed: %d" );
          iSSLog->Log((TDesC8)KSendFailed, ret );
          }
      }
    else
      {
      ret = KErrNotSupported;
      }

  return ret;

  }

// -----------------------------------------------------------------------------
// CSS::CompleteGetCallBarringStatus
// Complete GetCallBarringStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteGetCallBarringStatus
  (
  TInt aResult,                // Completion result value
  CMmDataPackage* aDataPackage // pointer to DataPackage
  )
  {
  CMobilePhoneCBList* callBlockingList = NULL;
  RMobilePhone::TMobilePhoneCBInfoEntryV1 unpackedCBInfoEntry;

  _LIT8( KResult, "CompleteGetCallBarringStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );


  if ( ( iHandleSpecifiedRequests ) && ( KErrNone == aResult ) )
      {
      aDataPackage->UnPackData( &callBlockingList );
      for ( TInt i = 0; i < callBlockingList->Enumerate(); ++i )
          {
          unpackedCBInfoEntry = callBlockingList->GetEntryL( i );
          _LIT8( KStatus,
              "CompleteGetCallBarringStatus unpackedCBInfoEntry.iStatus: %d" );
          iSSLog->Log((TDesC8)KStatus, unpackedCBInfoEntry.iStatus );

          _LIT8( KCondition,
              "CompleteGetCallBarringStatus unpackedCBInfoEntry.iCondition: %d" );
          iSSLog->Log((TDesC8)KCondition, unpackedCBInfoEntry.iCondition );

          _LIT8( KServiceGroup,
              "CompleteGetCallBarringStatus unpackedCBInfoEntry.iServiceGroup: %d" );
          iSSLog->Log((TDesC8)KServiceGroup, unpackedCBInfoEntry.iServiceGroup );

          if ( ( iCBInfoEntry.iStatus != unpackedCBInfoEntry.iStatus ) ||
               ( iCBInfoEntry.iCondition != unpackedCBInfoEntry.iCondition ) ||
               ( iCBInfoEntry.iServiceGroup != unpackedCBInfoEntry.iServiceGroup ) )
              {
              // Last table element handled and none of elements matched.
              if ( ( i + 1 ) >= callBlockingList->Enumerate() )
                  {
                  aResult = KErrGeneral;
                  break;
                  }
              }
          }
      }
  if ( KErrNone != aResult )
      {
      _LIT8( KFail, "CompleteGetCallBarringStatus Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
      }

  Signal( aResult );

  }


// -----------------------------------------------------------------------------
// CSS::GetCallForwardingStatus
// Getting call forwarding status
// -----------------------------------------------------------------------------
//

TInt CSS::GetCallForwardingStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::GetCallForwardingStatus");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    TPtrC servicetype;


    aItem.GetNextString ( string );
    aItem.GetNextString ( servicetype );


    RMobilePhone::TMobilePhoneCFCondition condition;


    if ( _L("CallForwardingUnconditional") == string )
      {
      condition = RMobilePhone::ECallForwardingUnconditional;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("ECallForwardingUnconditional") == string )
              {
              iCFInfoEntry.iCondition = RMobilePhone::ECallForwardingUnconditional;
              }

          aItem.GetNextString ( string );
          if ( _L("EFaxService") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EFaxService;
              }
          else if ( _L("EAllServices") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllServices;
              }
          else if ( _L("ESyncData") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::ESyncData;
              }

          aItem.GetNextString ( string );
          if ( _L("ECallForwardingStatusNotRegistered") == string )
              {
              iCFInfoEntry.iStatus = RMobilePhone::ECallForwardingStatusNotRegistered;
              }
          }
      }
    else if ( _L("CallForwardingBusy") == string )
      {
      condition = RMobilePhone::ECallForwardingBusy;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("ECallForwardingBusy") == string )
              {
              iCFInfoEntry.iCondition = RMobilePhone::ECallForwardingBusy;
              }

          aItem.GetNextString ( string );
          if ( _L("ETelephony") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::ETelephony;
              }
          else if ( _L("EAllTele") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllTele;
              }
          else if ( _L("EAllTeleExcSms") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllTeleExcSms;
              }
          else if ( _L("EPlmnTele2") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele2;
              }
          else if ( _L("EPlmnTele3") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele3;
              }
          else if ( _L("EPlmnTele4") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele4;
              }
          else if ( _L("EPlmnTele5") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele5;
              }
          else if ( _L("EPlmnTele7") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele7;
              }
          else if ( _L("EPlmnTele8") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele8;
              }
          else if ( _L("EPlmnTele9") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele9;
              }
          else if ( _L("EPlmnTeleA") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTeleA;
              }
          else if ( _L("EPlmnTeleB") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTeleB;
              }
          else if ( _L("EPlmnTeleC") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTeleC;
              }
          else if ( _L("EPlmnTeleD") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTeleD;
              }
          else if ( _L("EPlmnTeleE") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTeleE;
              }
          else if ( _L("EPlmnTeleF") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTeleF;
              }
          else if ( _L("EAllBearer") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllBearer;
              }
          else if ( _L("EAllAsync") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllAsync;
              }
          else if ( _L("EAsyncData") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAsyncData;
              }
          else if ( _L("EPlmnBearerServ1") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ1;
              }
          else if ( _L("EPlmnBearerServ2") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ2;
              }
          else if ( _L("EPlmnBearerServ3") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ3;
              }
          else if ( _L("EPlmnBearerServ4") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ4;
              }
          else if ( _L("EPlmnBearerServ5") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ5;
              }
          else if ( _L("EPlmnBearerServ6") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ6;
              }
          else if ( _L("EPlmnBearerServ7") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ7;
              }
          else if ( _L("EPlmnBearerServ8") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ8;
              }
          else if ( _L("EPlmnBearerServ9") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServ9;
              }
          else if ( _L("EPlmnBearerServA") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServA;
              }
          else if ( _L("EPlmnBearerServB") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServB;
              }
          else if ( _L("EPlmnBearerServC") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServC;
              }
          else if ( _L("EPlmnBearerServD") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServD;
              }
          else if ( _L("EPlmnBearerServE") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServE;
              }
          else if ( _L("EPlmnBearerServF") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnBearerServF;
              }
          else if ( _L("EPacketDataService") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPacketDataService;
              }
          else if ( _L("EVoiceGroupCall") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EVoiceGroupCall;
              }
          else if ( _L("EAllPlmnTele") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllPlmnTele;
              }
          else if ( _L("EPadAccess") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPadAccess;
              }
          else if ( _L("EAllPlmnBearer") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllPlmnBearer;
              }
          else if ( _L("EAuxVoiceService") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAuxVoiceService;
              }
          else if ( _L("EPlmnTele6") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EPlmnTele6;
              }
          else if ( _L("EServiceUnspecified") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EServiceUnspecified;
              }

          aItem.GetNextString ( string );
          if ( _L("ECallForwardingStatusNotRegistered") == string )
              {
              iCFInfoEntry.iStatus = RMobilePhone::ECallForwardingStatusNotRegistered;
              }
          else if ( _L("ECallForwardingStatusNotProvisioned") == string )
              {
              iCFInfoEntry.iStatus = RMobilePhone::ECallForwardingStatusNotProvisioned;
              }
          }
      }
    else if ( _L("CallForwardingNoReply") == string )
      {
      condition = RMobilePhone::ECallForwardingNoReply;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("ECallForwardingNoReply") == string )
              {
              iCFInfoEntry.iCondition = RMobilePhone::ECallForwardingNoReply;
              }

          aItem.GetNextString ( string );
          if ( _L("ECircuitDataService") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::ECircuitDataService;
              }
          else if ( _L("ETelephony") == string )
        	  {
        	  iCFInfoEntry.iServiceGroup = RMobilePhone::ETelephony;
        	  }
          else if ( _L("EVoiceService") == string )
        	  {
        	  iCFInfoEntry.iServiceGroup = RMobilePhone::EVoiceService;
        	  }
          else if ( _L("EAllAsync") == string )
        	  {
        	  iCFInfoEntry.iServiceGroup = RMobilePhone::EAllAsync;
        	  }

          aItem.GetNextString ( string );
          if ( _L("ECallForwardingStatusNotRegistered") == string )
              {
              iCFInfoEntry.iStatus = RMobilePhone::ECallForwardingStatusNotRegistered;
              }
          else if ( _L("ECallForwardingStatusNotProvisioned") == string )
              {
              iCFInfoEntry.iStatus = RMobilePhone::ECallForwardingStatusNotProvisioned;
              }
          }
      }
    else if ( _L("CallForwardingNotReachable") == string )
      {
      condition = RMobilePhone::ECallForwardingNotReachable;

      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("ECallForwardingNotReachable") == string )
              {
              iCFInfoEntry.iCondition = RMobilePhone::ECallForwardingNotReachable;
              }

          aItem.GetNextString ( string );
          if ( _L("EAllDataTele") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EAllDataTele;
              }

          else if ( _L("EShortMessageService") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone::EShortMessageService;
              }

          else if ( _L("EPlmnTele1") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone:: EPlmnTele1;
              }
          else if ( _L("EServiceUnspecified") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone:: EServiceUnspecified;
              }
          else if ( _L("EAllSync") == string )
              {
              iCFInfoEntry.iServiceGroup = RMobilePhone:: EAllSync;
              }
          aItem.GetNextString ( string );
          if ( _L("ECallForwardingStatusNotRegistered") == string )
              {
              iCFInfoEntry.iStatus = RMobilePhone::ECallForwardingStatusNotRegistered;
              }

          else if ( _L("ECallForwardingStatusNotProvisioned") == string )
              {
              iCFInfoEntry.iStatus = RMobilePhone::ECallForwardingStatusNotProvisioned;
              }
          }
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected condition for call forwarding not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }


    RMobilePhone::TMobileService servicegroup;


    if ( _L("ServiceUnspecified") == servicetype )
      {
      servicegroup = RMobilePhone::EServiceUnspecified;
      }
    else if ( _L("VoiceService") == servicetype )
      {
      servicegroup = RMobilePhone::EVoiceService;
      }
    else if ( _L("AuxVoiceService") == servicetype )
      {
      servicegroup = RMobilePhone::EAuxVoiceService;
      }
    else if ( _L("CircuitDataService") == servicetype )
      {
      servicegroup = RMobilePhone::ECircuitDataService;
      }
    else if ( _L("PacketDataService") == servicetype )
      {
      servicegroup = RMobilePhone::EPacketDataService;
      }
    else if ( _L("FaxService") == servicetype )
      {
      servicegroup = RMobilePhone::EFaxService;
      }
    else if ( _L("ShortMessageService") == servicetype )
      {
      servicegroup = RMobilePhone::EShortMessageService;
      }
    else if ( _L("AllServices") == servicetype )
      {
      servicegroup = RMobilePhone::EAllServices;
      }
    else if ( _L("AllTele") == servicetype )
      {
      servicegroup = RMobilePhone::EAllTele;
      }
    else if ( _L("Telephony") == servicetype )
      {
      servicegroup = RMobilePhone::ETelephony;
      }
    else if ( _L("AllDataTele") == servicetype )
      {
      servicegroup = RMobilePhone::EAllDataTele;
      }
    else if ( _L("AllDataExSms") == servicetype )
      {
      servicegroup = RMobilePhone::EAllDataExSms;
      }
    else if ( _L("AllTeleExcSms") == servicetype )
      {
      servicegroup = RMobilePhone::EAllTeleExcSms;
      }
    else if ( _L("AllPlmnTele") == servicetype )
      {
      servicegroup = RMobilePhone::EAllPlmnTele;
      }
    else if ( _L("PlmnTele1") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele1;
      }
    else if ( _L("PlmnTele2") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele2;
      }
    else if ( _L("PlmnTele3") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele3;
      }
    else if ( _L("PlmnTele4") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele4;
      }
    else if ( _L("PlmnTele5") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele5;
      }
    else if ( _L("PlmnTele6") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele6;
      }
    else if ( _L("PlmnTele7") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele7;
      }
    else if ( _L("PlmnTele8") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele8;
      }
    else if ( _L("PlmnTele9") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTele9;
      }
    else if ( _L("PlmnTeleA") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTeleA;
      }
    else if ( _L("PlmnTeleB") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTeleB;
      }
    else if ( _L("PlmnTeleC") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTeleC;
      }
    else if ( _L("PlmnTeleD") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTeleD;
      }
    else if ( _L("PlmnTeleE") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTeleE;
      }
    else if ( _L("PlmnTeleF") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnTeleF;
      }
    else if ( _L("AllBearer") == servicetype )
      {
      servicegroup = RMobilePhone::EAllBearer;
      }
    else if ( _L("AllAsync") == servicetype )
      {
      servicegroup = RMobilePhone::EAllAsync;
      }
    else if ( _L("AllSync") == servicetype )
      {
      servicegroup = RMobilePhone::EAllSync;
      }
    else if ( _L("SyncData") == servicetype )
      {
      servicegroup = RMobilePhone::ESyncData;
      }
    else if ( _L("AsyncData") == servicetype )
      {
      servicegroup = RMobilePhone::EAsyncData;
      }
    else if ( _L("PacketData") == servicetype )
      {
      servicegroup = RMobilePhone::EPacketData;
      }
    else if ( _L("PadAccess") == servicetype )
      {
      servicegroup = RMobilePhone::EPadAccess;
      }
    else if ( _L("AllPlmnBearer") == servicetype )
      {
      servicegroup = RMobilePhone::EAllPlmnBearer;
      }
    else if ( _L("PlmnBearerServ1") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ1;
      }
    else if ( _L("PlmnBearerServ2") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ2;
      }
    else if ( _L("PlmnBearerServ3") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ3;
      }
    else if ( _L("PlmnBearerServ4") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ4;
      }
    else if ( _L("PlmnBearerServ5") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ5;
      }
    else if ( _L("PlmnBearerServ6") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ6;
      }
    else if ( _L("PlmnBearerServ7") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ7;
      }
    else if ( _L("PlmnBearerServ8") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ8;
      }
    else if ( _L("PlmnBearerServ9") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServ9;
      }
    else if ( _L("PlmnBearerServA") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServA;
      }
    else if ( _L("PlmnBearerServB") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServB;
      }
    else if ( _L("PlmnBearerServC") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServC;
      }
    else if ( _L("PlmnBearerServD") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServD;
      }
    else if ( _L("PlmnBearerServE") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServE;
      }
    else if ( _L("PlmnBearerServF") == servicetype )
      {
      servicegroup = RMobilePhone::EPlmnBearerServF;
      }
    else if ( _L("AltTele") == servicetype )
      {
      servicegroup = RMobilePhone::EAltTele;
      }
    else if ( _L("VoiceGroupCall") == servicetype )
      {
      servicegroup = RMobilePhone::EVoiceGroupCall;
      }
    else if ( _L("VoiceBroadcast") == servicetype )
      {
      servicegroup = RMobilePhone::EVoiceBroadcast;
      }
    else if ( _L("AllGprsBearer") == servicetype )
      {
      servicegroup = RMobilePhone::EAllGprsBearer;
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected servicegroup for call forwarding not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }




    if( supported )
      {
       //Create package
     CMmDataPackage package;
      // Set package data
      package.PackData( &condition, &servicegroup );

     ret = HandleRequestL( EMobilePhoneGetCallForwardingStatusPhase1,
                           &package );

      if ( KErrNone == ret )
          {
          _LIT8( KSendOk, "GetCallForwardingStatus request send ok" );
          iSSLog->Log((TDesC8)KSendOk );
          }
      else
          {
           _LIT8( KSendFailed, "GetCallForwardingStatus request send failed: %d" );
          iSSLog->Log((TDesC8)KSendFailed, ret );
          }
      }
    else
      {
      ret = KErrNotSupported;
      }

  return ret;

  }


// -----------------------------------------------------------------------------
// CSS::CompleteGetCallForwardingStatus
// Complete GetCallForwardingStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteGetCallForwardingStatus
    (
    TInt aResult,                // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
  {
  _LIT8( KResult, "CompleteGetCallForwardingStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  CMobilePhoneCFList* callForwardingList = NULL;
  RMobilePhone::TMobilePhoneCFInfoEntryV1 unpackedCFInfoEntry;


  if ( ( iHandleSpecifiedRequests ) && ( KErrNone == aResult ) )
      {
      aDataPackage->UnPackData( &callForwardingList );
      for ( TInt i = 0; i < callForwardingList->Enumerate(); ++i )
          {
          unpackedCFInfoEntry = callForwardingList->GetEntryL( i );
          _LIT8( KStatus,
              "CompleteGetCallForwardingStatus unpackedCFInfoEntry.iStatus: %d" );
          iSSLog->Log((TDesC8)KStatus, unpackedCFInfoEntry.iStatus );

          _LIT8( KCondition,
              "CompleteGetCallForwardingStatus unpackedCFInfoEntry.iCondition: %d" );
          iSSLog->Log((TDesC8)KCondition, unpackedCFInfoEntry.iCondition );

          _LIT8( KServiceGroup,
              "CompleteGetCallForwardingStatus unpackedCFInfoEntry.iServiceGroup: %d" );
          iSSLog->Log((TDesC8)KServiceGroup, unpackedCFInfoEntry.iServiceGroup );

          if ( ( iCFInfoEntry.iStatus != unpackedCFInfoEntry.iStatus ) ||
               ( iCFInfoEntry.iCondition != unpackedCFInfoEntry.iCondition ) ||
               ( iCFInfoEntry.iServiceGroup != unpackedCFInfoEntry.iServiceGroup ) )
              {
              // Last table element handled and none of elements matched.
              if ( ( i + 1 ) >= callForwardingList->Enumerate() )
                  {
                  aResult = KErrGeneral;
                  break;
                  }
              }
              Signal( aResult );
          }
      }
  else
      {
      if ( KErrNone != aResult )
          {
          _LIT8( KFail, "CompleteGetCallForwardingStatus Failed: %d" );
            iSSLog->Log((TDesC8)KFail, aResult );
          }
      }

  Signal( aResult );
  }

// -----------------------------------------------------------------------------
// CSS::GetCallWaitingStatus
// Getting call waiting status
// -----------------------------------------------------------------------------
//

TInt CSS::GetCallWaitingStatus( CStifItemParser& aItem )
  {

  _LIT8(KPbInit, "SS::GetCallWaitingStatus");
  iSSLog->Log((TDesC8)KPbInit);

  TInt ret( KErrNotFound );
  TPtrC string;

  if ( iHandleSpecifiedRequests )
      {
      aItem.GetNextString ( string );
      if ( _L("EAllServices") == string )
          {
          iCWInfoEntry.iServiceGroup = RMobilePhone::EAllServices;
          }

      aItem.GetNextString ( string );
      if ( _L("ECallWaitingStatusNotActive") == string )
          {
          iCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotActive;
          }
      }

  ret = HandleRequestL( EMobilePhoneGetWaitingStatusPhase1 );

  if ( KErrNone == ret )
      {
      _LIT8( KSendOk, "GetCallWaitingStatus request send ok" );
      iSSLog->Log((TDesC8)KSendOk );
      }
  else
      {
      _LIT8( KSendFailed, "GetCallWaitingStatus request send failed: %d" );
      iSSLog->Log((TDesC8)KSendFailed, ret );
      }

  return ret;

  }

// -----------------------------------------------------------------------------
// CSS::CompleteGetCallWaitingStatus
// Complete GetCallWaitingStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteGetCallWaitingStatus
    (
    TInt aResult,                // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
  {
  CMobilePhoneCWList* callWaitingList = NULL;
  RMobilePhone::TMobilePhoneCWInfoEntryV1 unpackedCWInfoEntry;;

  _LIT8( KResult, "CompleteGetCallWaitingStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if ( ( iHandleSpecifiedRequests ) && ( KErrNone == aResult ) )
      {
      aDataPackage->UnPackData( &callWaitingList );
      for ( TInt i = 0; i < callWaitingList->Enumerate(); ++i )
          {
          unpackedCWInfoEntry = callWaitingList->GetEntryL( i );
          _LIT8( KStatus,
              "CompleteGetCallWaitingStatus unpackedCWInfoEntry.iStatus: %d" );
          iSSLog->Log((TDesC8)KStatus, unpackedCWInfoEntry.iStatus );

          _LIT8( KServiceGroup,
              "CompleteGetCallWaitingStatus unpackedCWInfoEntry.iServiceGroup: %d" );
          iSSLog->Log((TDesC8)KServiceGroup, unpackedCWInfoEntry.iServiceGroup );

          if ( ( iCWInfoEntry.iStatus != unpackedCWInfoEntry.iStatus ) ||
               ( iCWInfoEntry.iServiceGroup != unpackedCWInfoEntry.iServiceGroup ) )
              {
              // Last table element handled and none of elements matched.
              if ( ( i + 1 ) >= callWaitingList->Enumerate() )
                  {
                    aResult = KErrGeneral;
                    break;
                  }
              }
          }
      }

  if( KErrNone != aResult )
      {
      _LIT8( KFail, "CompleteGetCallWaitingStatus Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
      }

  Signal( aResult );
  }

// -----------------------------------------------------------------------------
// CSS::GetIdentityServiceStatus
// Getting idetity service status
// -----------------------------------------------------------------------------
//
TInt CSS::GetIdentityServiceStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::GetIdentityServiceStatus");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    aItem.GetNextString ( string );

    RMobilePhone::TMobilePhoneIdService condition;

    if ( _L("IdServiceCallerPresentation") == string )
      {
      condition = RMobilePhone::EIdServiceCallerPresentation;
      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EIdServiceActivePermanent") == string )
              {
              iStatusETel = RMobilePhone::EIdServiceActivePermanent;
              }
          }
      }
    else if ( _L("IdServiceCallerRestriction") == string )
      {
      condition = RMobilePhone::EIdServiceCallerRestriction;
      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EIdServiceActiveDefaultAllowed") == string )
              {
              iStatusETel = RMobilePhone::EIdServiceActiveDefaultAllowed;
              }
          }
      }
    else if ( _L("IdServiceConnectedPresentation") == string )
      {
      condition = RMobilePhone::EIdServiceConnectedPresentation;
      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EIdServiceNotProvisioned") == string )
              {
              iStatusETel = RMobilePhone::EIdServiceNotProvisioned;
              }
          }
      }
    else if ( _L("IdServiceConnectedRestriction") == string )
      {
      condition = RMobilePhone::EIdServiceConnectedRestriction;
      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EIdServiceNotProvisioned") == string )
              {
              iStatusETel = RMobilePhone::EIdServiceNotProvisioned;
              }
          }
      }
    else if ( _L("IdServiceCallerName") == string )
      {
      condition = RMobilePhone::EIdServiceCallerName;
      if ( iHandleSpecifiedRequests )
          {
          aItem.GetNextString ( string );
          if ( _L("EIdServiceNotProvisioned") == string )
              {
              iStatusETel = RMobilePhone::EIdServiceNotProvisioned;
              }
          }
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected identity service status not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }

    if( supported )
      {
       //Create package
     CMmDataPackage package;
      // Set package data
      package.PackData( &condition );

      ret = HandleRequestL( EMobilePhoneGetIdentityServiceStatus,
                            &package );

      if ( KErrNone == ret )
          {
          _LIT8( KSendOk, "GetIdentityServiceStatus request send ok" );
          iSSLog->Log((TDesC8)KSendOk );
        }
      else
          {
          _LIT8( KSendFailed, "GetIdentityService request send failed: %d" );
          iSSLog->Log((TDesC8)KSendFailed, ret );
          }
      }
    else
      {
      ret = KErrNotSupported;
      }

  return ret;

  }

// -----------------------------------------------------------------------------
// CSS::CompleteGetIdentityServiceStatus
// Complete CompleteGetIdentityServiceStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteGetIdentityServiceStatus
    (
    TInt aResult,                // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
  {
  RMobilePhone::TMobilePhoneIdServiceStatus unpackedStatusETel;

  _LIT8( KResult, "CompleteGetIdentityServiceStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if ( ( iHandleSpecifiedRequests ) && ( KErrNone == aResult ) )
      {
      aDataPackage->UnPackData( unpackedStatusETel );
      _LIT8( KStatusETel,
          "CompleteGetIdentityServiceStatus unpackedStatusETel: %d" );
      iSSLog->Log((TDesC8)KStatusETel, unpackedStatusETel );

      if ( iStatusETel != unpackedStatusETel )
          {
          aResult = KErrGeneral;
          }
      }

  if ( KErrNone != aResult )
    {
    _LIT8( KFail, "CompleteGetIdentityServiceStatus Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
    }

  Signal( aResult );

  }

// -----------------------------------------------------------------------------
// CSS::GetIccCallForwardingStatus
// Getting call forwarding status
// -----------------------------------------------------------------------------
//

TInt CSS::GetIccCallForwardingStatus( CStifItemParser& aItem )
    {
    TPtrC checkForwardings;

    aItem.GetNextString ( checkForwardings );

    if ( ( _L("CHECK_ALL_FORWARDINGS") == checkForwardings ) &&
         ( iHandleSpecifiedRequests ) )
        {
        iAllForwardingsOn = ETrue;
        }
    else
        {
        iAllForwardingsOn = EFalse;
        }

    _LIT8(KSsInit, "SS::GetIccCallForwardingStatus");
    iSSLog->Log((TDesC8)KSsInit);

    TInt ret( KErrNotFound );
    ret = HandleRequestL( ECustomGetIccCallForwardingStatusIPC );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetIccCallForwardingStatus request send ok" );
        iSSLog->Log((TDesC8)KSendOk );
      }
    else
        {
        _LIT8( KSendFailed, "GetIccCallForwardingStatus request send failed: %d" );
        iSSLog->Log((TDesC8)KSendFailed, ret );
      }

    return ret;

    }


// -----------------------------------------------------------------------------
// CSS::CompleteGetIccCallForwardingStatus
// Complete GetIccCallForwardingStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteGetIccCallForwardingStatus
    (
    TInt aResult, //Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
  {
  _LIT8( KResult, "CompleteGetIccCallForwardingStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  // checking results and unpacking message
  if ( KErrNone == aResult )
      {
      RMmCustomAPI::TCFIndicators* tCFIndicators = NULL;
      aDataPackage->UnPackData( &tCFIndicators );

      iSSLog->Log( _L("Subscriber Profile ID = 0x%x"), &tCFIndicators->iMultipleSubscriberProfileID );

      iSSLog->Log( _L("CF Number = %S"), &tCFIndicators->iCFNumber );

      if ( tCFIndicators->iIndicator & RMobilePhone::KCFUIndicatorVoice )
          {
          _LIT8(KResultOk, "CF Voice Indicator status Active" );
          iSSLog->Log((TDesC8)KResultOk);
          }
      if ( tCFIndicators->iIndicator & RMobilePhone::KCFUIndicatorFax )
          {
          _LIT8(KResultOk, "CF Fax Indicator status Active" );
          iSSLog->Log((TDesC8)KResultOk);
          }
      if ( tCFIndicators->iIndicator & RMobilePhone::KCFUIndicatorData )
          {
          _LIT8(KResultOk, "CF Data Indicator status Active" );
          iSSLog->Log((TDesC8)KResultOk);
          }

      if ( ( iAllForwardingsOn ) &&
           ( iHandleSpecifiedRequests ) )
          {
          if ( ( ! ( tCFIndicators->iIndicator & RMobilePhone::KCFUIndicatorVoice ) ) ||
               ( ! ( tCFIndicators->iIndicator & RMobilePhone::KCFUIndicatorFax ) ) ||
               ( ! ( tCFIndicators->iIndicator & RMobilePhone::KCFUIndicatorData ) ) )
              {
              aResult = KErrGeneral;
              }
          }
      }
  else
      {
      _LIT8( KFail, "CompleteGetIccCallForwardingStatus Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
      }

  iAllForwardingsOn = EFalse;

  Signal( aResult );

  }

// -----------------------------------------------------------------------------
// CSS::GetIccMessageWaitingIndicators
// Setting call forwarding status
// -----------------------------------------------------------------------------
//

TInt CSS::GetIccMessageWaitingIndicators( CStifItemParser& aItem )
    {

    TPtrC typeString;
    TPtrC voiceMailString;
    TInt numberOfVoicemails( 0 );
    TPtrC faxMailString;
    TInt numberOfFaxmails( 0 );
    TPtrC emailMailString;
    TInt numberOfEmails( 0 );
    TInt ret( KErrNotFound );

    _LIT8(KSsInit, "SS::GetIccMessageWaitingIndicators");
    iSSLog->Log((TDesC8)KSsInit);

    aItem.GetNextString ( typeString );

    if ( _L("Rel4") == typeString )
        {
        iIccMessageWaitingIndicatorsType = EIccIndicationRel4;
        }
    else if ( _L("CPHS") == typeString )
        {
        iIccMessageWaitingIndicatorsType = EIccIndicationCPHS;
        }
    else
        {
        _LIT8( KNotSupported, "GetIccMessageWaitingIndicators - unknown type" );
        iSSLog->Log((TDesC8)KNotSupported );
        }

    if ( iHandleSpecifiedRequests )
        {
        aItem.GetNextString ( voiceMailString );
        if ( _L("KDisplayVoicemailActive") == voiceMailString )
            {
            iMSGWaiting.iDisplayStatus
                |= RMobilePhone::KDisplayVoicemailActive;
            aItem.GetNextInt ( numberOfVoicemails );
            iMSGWaiting.iVoiceMsgs = numberOfVoicemails;
            }

        aItem.GetNextString ( faxMailString );
        if ( _L("KDisplayFaxActive") == faxMailString )
            {
            iMSGWaiting.iDisplayStatus |= RMobilePhone::KDisplayFaxActive;
            aItem.GetNextInt ( numberOfFaxmails );
            iMSGWaiting.iFaxMsgs = numberOfFaxmails;
            }

        aItem.GetNextString ( emailMailString );
        if ( _L("KDisplayEmailActive") == emailMailString )
            {
            iMSGWaiting.iDisplayStatus |= RMobilePhone::KDisplayEmailActive;
            aItem.GetNextInt ( numberOfEmails );
            iMSGWaiting.iEmailMsgs = numberOfEmails;
            }
        }

    ret = HandleRequestL( EMobilePhoneGetIccMessageWaitingIndicators );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "GetIccMessageWaitingIndicators request send ok" );
        iSSLog->Log((TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "GetIccMessageWaitingIndicators request send failed: %d" );
        iSSLog->Log((TDesC8)KSendFailed, ret );
        }

  return ret;

  }


// -----------------------------------------------------------------------------
// CSS::CompleteGetIccMessageWaitingIndicators
// Complete GetIccMessageWaitingIndicators method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteGetIccMessageWaitingIndicators
    (
    TInt aResult, //Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    _LIT8( KResult, "CompleteGetIccMessageWaitingIndicators result: %d" );
    iSSLog->Log((TDesC8)KResult, aResult );

    if ( KErrNone == aResult )
        {
        RMobilePhone::TMobilePhoneMessageWaitingV1* msgWaiting = NULL;

        aDataPackage->UnPackData( &msgWaiting );

        if ( EIccIndicationRel4 == iIccMessageWaitingIndicatorsType )
            {

            // Check data returned by Nokia TSY
            if ( iHandleSpecifiedRequests )
                {
                if ( iMSGWaiting.iDisplayStatus & RMobilePhone::KDisplayVoicemailActive )
                    {
                    if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayVoicemailActive )
                        {
                        if ( iMSGWaiting.iVoiceMsgs != msgWaiting->iVoiceMsgs )
                            {
                            _LIT8(KVoiceMsgs, "Number of Voice messages incorrect" );
                            iSSLog->Log((TDesC8)KVoiceMsgs);
                            aResult = KErrGeneral;
                            }
                        }
                    else
                        {
                        _LIT8(KDisplayVoicemailActive, "Display Status KDisplayVoicemailActive NOT expected" );
                        iSSLog->Log((TDesC8)KDisplayVoicemailActive);
                        aResult = KErrGeneral;
                        }
                    }

                if ( iMSGWaiting.iDisplayStatus & RMobilePhone::KDisplayFaxActive )
                    {
                    if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayFaxActive )
                        {
                        if ( iMSGWaiting.iFaxMsgs != msgWaiting->iFaxMsgs )
                            {
                            _LIT8(KFaxMsgs, "Number of Fax messages incorrect" );
                            iSSLog->Log((TDesC8)KFaxMsgs);
                            aResult = KErrGeneral;
                            }
                        }
                    else
                        {
                        _LIT8(KDisplayFaxActive, "Display Status KDisplayFaxActive NOT expected" );
                        iSSLog->Log((TDesC8)KDisplayFaxActive);
                        aResult = KErrGeneral;
                        }
                    }

                if ( iMSGWaiting.iDisplayStatus & RMobilePhone::KDisplayEmailActive )
                    {
                    if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayEmailActive )
                        {
                        if ( iMSGWaiting.iEmailMsgs != msgWaiting->iEmailMsgs )
                            {
                            _LIT8(KMailMsgs, "Number of EMail messages incorrect" );
                            iSSLog->Log((TDesC8)KMailMsgs);
                            aResult = KErrGeneral;
                            }
                        }
                    else
                        {
                        _LIT8(KDisplayEmailActive, "Display Status KDisplayEmailActive NOT expected" );
                        iSSLog->Log((TDesC8)KDisplayEmailActive);
                        aResult = KErrGeneral;
                        }
                    }
                }

                if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayVoicemailActive )
                    {
                    _LIT8(KResultOk, "Display Voicemail Active" );
                    iSSLog->Log((TDesC8)KResultOk);
                    if ( msgWaiting->iVoiceMsgs )
                        {
                        iSSLog->Log( _L("Voice Messages: %d"), msgWaiting->iVoiceMsgs );
                        }
                    }
                if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayAuxVoicemailActive )
                    {
                    _LIT8(KResultOk, "Display Aux Voicemail Active" );
                    iSSLog->Log((TDesC8)KResultOk);
                    if ( msgWaiting->iAuxVoiceMsgs )
                        {
                        iSSLog->Log( _L("Aux. Voice Messages: %d"), msgWaiting->iAuxVoiceMsgs );
                        }
                    }
                if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayFaxActive )
                    {
                    _LIT8(KResultOk, "Display Fax Active" );
                    iSSLog->Log((TDesC8)KResultOk);
                    if ( msgWaiting->iFaxMsgs )
                        {
                        iSSLog->Log( _L("Fax Messages: %d"), msgWaiting->iFaxMsgs );
                        }
                    }
                if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayEmailActive )
                    {
                    _LIT8(KResultOk, "Display Email Active" );
                    iSSLog->Log((TDesC8)KResultOk);
                    if ( msgWaiting->iEmailMsgs )
                        {
                        iSSLog->Log( _L("Email Messages: %d"), msgWaiting->iEmailMsgs );
                        }
                    }
                if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayOtherActive )
                    {
                    _LIT8(KResultOk, "Display Other Active" );
                    iSSLog->Log((TDesC8)KResultOk);
                    if ( msgWaiting->iOtherMsgs )
                        {
                        iSSLog->Log( _L("Other Messages: %d"), msgWaiting->iOtherMsgs );
                        }
                    }
                if ( msgWaiting->iDisplayStatus & RMobilePhone::KDisplayDataActive )
                    {
                    _LIT8(KResultOk, "Display Data Active" );
                    iSSLog->Log((TDesC8)KResultOk);
                    if ( msgWaiting->iDataMsgs )
                        {
                        iSSLog->Log( _L("Data Messages: %d"), msgWaiting->iDataMsgs );
                        }
                    }

                iSSLog->Log(_L8("CSS::CompleteGetIccMessageWaitingIndicators OK."));
            }
        else if ( EIccIndicationCPHS == iIccMessageWaitingIndicatorsType )
            {
            // Check data returned by Nokia TSY
            if ( iHandleSpecifiedRequests )
                {
                if ( iMSGWaiting.iDisplayStatus
                    & RMobilePhone::KDisplayVoicemailActive )
                    {
                    if ( !( msgWaiting->iDisplayStatus
                        & RMobilePhone::KDisplayVoicemailActive ) )
                        {
                        _LIT8( KDisplayVoicemailActive, "Display Status KDisplayVoicemailActive NOT expected" );
                        iSSLog->Log( ( TDesC8 )KDisplayVoicemailActive );
                        aResult = KErrGeneral;
                        }
                    }

                if ( iMSGWaiting.iDisplayStatus
                    & RMobilePhone::KDisplayFaxActive )
                    {
                    if ( !( msgWaiting->iDisplayStatus
                        & RMobilePhone::KDisplayFaxActive ) )
                        {
                        _LIT8( KDisplayFaxActive, "Display Status KDisplayFaxActive NOT expected" );
                        iSSLog->Log( ( TDesC8 )KDisplayFaxActive );
                        aResult = KErrGeneral;
                        }
                    }

                if ( iMSGWaiting.iDisplayStatus
                    & RMobilePhone::KDisplayEmailActive )
                    {
                    if ( !( msgWaiting->iDisplayStatus
                        & RMobilePhone::KDisplayEmailActive ) )
                        {
                        _LIT8( KDisplayEmailActive, "Display Status KDisplayEmailActive NOT expected" );
                        iSSLog->Log( ( TDesC8 )KDisplayEmailActive );
                        aResult = KErrGeneral;
                        }
                    }
                } // if ( iHandleSpecifiedRequests )
                iSSLog->Log( _L8( "CSS::CompleteGetIccMessageWaitingIndicators OK." ) );
            } //  else if ( EIccIndicationCPHS == iIccMessageWaitingIndicatorsType )
        } // if ( KErrNone == aResult )

    else
        {
        _LIT8( KFail, "CompleteGetIccMessageWaitingIndicators Failed: %d" );
        iSSLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSS::SetIccMessageWaitingIndicators
// Setting call forwarding status
// -----------------------------------------------------------------------------
//
TInt CSS::SetIccMessageWaitingIndicators( CStifItemParser& aItem )
    {
    _LIT8(KSsInit, "SS::SetIccMessageWaitingIndicators");
    iSSLog->Log((TDesC8)KSsInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TInt displayStatus( 0 );
    TInt voiceMsgs( 0 );
    TInt auxVoiceMsgs( 0 );
    TInt dataMsgs( 0 );
    TInt faxMsgs( 0 );
    TInt emailMsgs( 0 );
    TInt otherMsgs( 0 );

    RMobilePhone::TMobilePhoneMessageWaitingV1 displayMsgs;

    aItem.GetNextInt ( displayStatus );
    displayMsgs.iDisplayStatus = displayStatus;

    if ( displayStatus > 0 )
        {
        aItem.GetNextInt ( voiceMsgs );
        aItem.GetNextInt ( auxVoiceMsgs );
        aItem.GetNextInt ( dataMsgs );
        aItem.GetNextInt ( faxMsgs );
        aItem.GetNextInt ( emailMsgs );
        aItem.GetNextInt ( otherMsgs );


        displayMsgs.iVoiceMsgs = voiceMsgs;
        displayMsgs.iAuxVoiceMsgs = auxVoiceMsgs;
        displayMsgs.iDataMsgs = dataMsgs;
        displayMsgs.iFaxMsgs = faxMsgs;
        displayMsgs.iEmailMsgs = emailMsgs;
        displayMsgs.iOtherMsgs = otherMsgs;

        if ( displayMsgs.iDisplayStatus & RMobilePhone::KDisplayVoicemailActive )
            {
                _LIT8(KResultOk, "Display Voicemail Set Active" );
                iSSLog->Log((TDesC8)KResultOk);
                if ( displayMsgs.iVoiceMsgs )
                    {
                    iSSLog->Log( _L("Voice Messages: %d"), displayMsgs.iVoiceMsgs );
                    }
            }
          if ( displayMsgs.iDisplayStatus & RMobilePhone::KDisplayAuxVoicemailActive )
            {
                _LIT8(KResultOk, "Display Aux Voicemail Set Active" );
                iSSLog->Log((TDesC8)KResultOk);
                if ( displayMsgs.iAuxVoiceMsgs )
                    {
                iSSLog->Log( _L("Aux. Voice Messages: %d"), displayMsgs.iAuxVoiceMsgs );
                    }
            }
        if ( displayMsgs.iDisplayStatus & RMobilePhone::KDisplayFaxActive )
            {
                _LIT8(KResultOk, "Display Fax Set Active" );
                iSSLog->Log((TDesC8)KResultOk);
                if ( displayMsgs.iFaxMsgs )
                    {
                iSSLog->Log( _L("Fax Messages: %d"), displayMsgs.iFaxMsgs );
                    }
            }
        if ( displayMsgs.iDisplayStatus & RMobilePhone::KDisplayEmailActive )
            {
                _LIT8(KResultOk, "Display Email Set Active" );
                iSSLog->Log((TDesC8)KResultOk);
                if ( displayMsgs.iEmailMsgs )
                    {
                iSSLog->Log( _L("Email Messages: %d"), displayMsgs.iEmailMsgs );
                    }
            }
        if ( displayMsgs.iDisplayStatus & RMobilePhone::KDisplayOtherActive )
            {
                _LIT8(KResultOk, "Display Other Set Active" );
                iSSLog->Log((TDesC8)KResultOk);
                if ( displayMsgs.iOtherMsgs )
                    {
                iSSLog->Log( _L("Other Messages: %d"), displayMsgs.iOtherMsgs );
                    }
            }
        if ( displayMsgs.iDisplayStatus & RMobilePhone::KDisplayDataActive )
            {
                _LIT8(KResultOk, "Display Data Active" );
                iSSLog->Log((TDesC8)KResultOk);
                if ( displayMsgs.iDataMsgs )
                    {
                iSSLog->Log( _L("Data Messages: %d"), displayMsgs.iDataMsgs );
                    }
            }
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected display status for Icc Message Waiting Indicators not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if( supported )
        {
        //Create package
          CMmDataPackage package;
        // Set package data
        package.PackData( &displayMsgs );

        ret = HandleRequestL( EMobilePhoneSetIccMessageWaitingIndicators,
                              &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetIccMessageWaitingIndicators request send ok" );
            iSSLog->Log((TDesC8)KSendOk );
          }
        else
            {
             _LIT8( KSendFailed, "SetIccMessageWaitingIndicators request send failed: %d" );
            iSSLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

  return ret;
  }

// -----------------------------------------------------------------------------
// CSS::CompleteSetIccMessageWaitingIndicators
// Complete SetIccMessageWaitingIndicators method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteSetIccMessageWaitingIndicators
    (
    TInt aResult
    )
  {
  _LIT8( KResult, "CompleteSetIccMessageWaitingIndicators result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if( KErrNone != aResult )
      {
      _LIT8( KFail, "CompleteSetIccMessageWaitingIndicators Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
      }

  Signal( aResult );
  }

// -----------------------------------------------------------------------------
// CSS::SetCallBarringStatus
// Setting call barring status
// -----------------------------------------------------------------------------
//

TInt CSS::SetCallBarringStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::SetCallBarringStatus");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    TPtrC groupstring;
    TPtrC actionstring;
    TPtrC passwordstring;

    aItem.GetNextString ( string );
    aItem.GetNextString ( groupstring );
    aItem.GetNextString ( actionstring );
    aItem.GetNextString ( passwordstring );


    RMobilePhone::TMobilePhoneCBCondition condition;


    if ( _L("BarUnspecified") == string )
      {
      condition = RMobilePhone::EBarUnspecified;
      }
    else if ( _L("BarAllIncoming") == string )
      {
      condition = RMobilePhone::EBarAllIncoming;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarAllIncoming;
          }
      }
    else if ( _L("BarIncomingRoaming") == string )
      {
      condition = RMobilePhone::EBarIncomingRoaming;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarIncomingRoaming;
          }
      }
    else if ( _L("BarAllOutgoing") == string )
      {
      condition = RMobilePhone::EBarAllOutgoing;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarAllOutgoing;
          }
      }
    else if ( _L("BarOutgoingInternational") == string )
      {
      condition = RMobilePhone::EBarOutgoingInternational;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarOutgoingInternational;
          }
      }
    else if ( _L("BarOutgoingInternationalExHC") == string )
      {
      condition = RMobilePhone::EBarOutgoingInternationalExHC;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarOutgoingInternationalExHC;
          }
      }
    else if ( _L("BarAllCases") == string )
      {
      condition = RMobilePhone::EBarAllCases;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarAllCases;
          }
      }
    else if ( _L("BarAllOutgoingServices") == string )
      {
      condition = RMobilePhone::EBarAllOutgoingServices;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarAllOutgoingServices;
          }
      }
    else if ( _L("BarAllIncomingServices") == string )
      {
      condition = RMobilePhone::EBarAllIncomingServices;
      if ( iHandleSpecifiedRequests )
          {
          iCBCondition = RMobilePhone::EBarAllIncomingServices;
          }
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected condition for set call barring not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }



    RMobilePhone::TMobilePhoneCBChangeV1 group;

    if ( _L("ServiceUnspecified") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EServiceUnspecified;
      }
    else if ( _L("VoiceService") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EVoiceService;
      }
    else if ( _L("AuxVoiceService") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EAuxVoiceService;
      }
    else if ( _L("CircuitDataService") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::ECircuitDataService;
      }
    else if ( _L("PacketDataService") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EPacketDataService;
      }
    else if ( _L("FaxService") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EFaxService;
      }
    else if ( _L("ShortMessageService") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EShortMessageService;
      }
    else if ( _L("AllServices") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EAllServices;
      }
    else if ( _L("AllTele") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EAllTele;
      }
    else if ( _L("Telephony") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::ETelephony;
      }
    else if ( _L("AllDataTele") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EAllDataTele;
      }
    else if ( _L("AllDataExSms") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EAllDataExSms;
      }
    else if ( _L("AllTeleExcSms") == groupstring )
      {
      group.iServiceGroup = RMobilePhone::EAllTeleExcSms;
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected set call forwarding servicegroup not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }

    if ( _L("ServiceActionRegister") == actionstring )
      {
      group.iAction = RMobilePhone::EServiceActionRegister;
      }
    else if ( _L("ServiceActionActivate") == actionstring )
      {
      group.iAction = RMobilePhone::EServiceActionActivate;
      }
    else if ( _L("ServiceActionDeactivate") == actionstring )
      {
      group.iAction = RMobilePhone::EServiceActionDeactivate;
      }
    else if ( _L("ServiceActionErase") == actionstring )
      {
      group.iAction = RMobilePhone::EServiceActionErase;
      }
    else if ( _L("ServiceActionUnspecified") == actionstring )
      {
      group.iAction = RMobilePhone::EServiceActionUnspecified;
      }
    else if ( _L("ServiceActionInvoke") == actionstring )
      {
      group.iAction = RMobilePhone::EServiceActionInvoke;
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected set call barring action not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }


    if ( KDontCare() != passwordstring)
        {
        group.iPassword.Copy(passwordstring);
        }


    if( supported )
        {

          RMobilePhone::TMobilePhoneCBChangeV1* groupPtr = &group;

         //Create package
        CMmDataPackage package;
        // Set package data
        package.PackData( &condition, &groupPtr);

        ret = HandleRequestL( EMobilePhoneSetCallBarringStatus,
                              &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetCallBarringStatus request send ok" );
            iSSLog->Log((TDesC8)KSendOk );
          }
        else
            {
             _LIT8( KSendFailed, "SetCallBarringStatus request send failed: %d" );
            iSSLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

  return ret;


  }

// -----------------------------------------------------------------------------
// CSS::CompleteSetCallBarringStatus
// Complete SetCallBarringStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteSetCallBarringStatus
    (
    TInt aResult                // Completion result value
    )
  {
  _LIT8( KResult, "CompleteSetCallBarringStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if( KErrNone != aResult )
    {
    _LIT8( KFail, "CompleteSetCallBarringStatus Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
    }

  Signal( aResult );

  }

// -----------------------------------------------------------------------------
// CSS::SetCallForwardingStatus
// Setting call forwarding status
// -----------------------------------------------------------------------------
//

TInt CSS::SetCallForwardingStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::SetCallForwardingStatus");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC string;
    TPtrC groupstring;
    TPtrC actionstring;
    TPtrC numberstring;
    TPtrC timestring;
    TPtrC typestring;
    TPtrC planstring;

    aItem.GetNextString ( string );
    aItem.GetNextString ( groupstring );
    aItem.GetNextString ( actionstring );
    aItem.GetNextString ( typestring );
    aItem.GetNextString ( planstring );
    aItem.GetNextString ( numberstring );
    aItem.GetNextString ( timestring );

    RMobilePhone::TMobilePhoneCFCondition condition;


    if ( _L("CallForwardingUnspecified") == string )
        {
        condition = RMobilePhone::ECallForwardingUnspecified;
        }
    else if ( _L("CallForwardingUnconditional") == string )
        {
        condition = RMobilePhone::ECallForwardingUnconditional;
        }
    else if ( _L("CallForwardingBusy") == string )
        {
        condition = RMobilePhone::ECallForwardingBusy;
        }
    else if ( _L("CallForwardingNoReply") == string )
        {
        condition = RMobilePhone::ECallForwardingNoReply;
        }
    else if ( _L("CallForwardingNotReachable") == string )
        {
        condition = RMobilePhone::ECallForwardingNotReachable;
        }
    else if ( _L("CallForwardingAllCases") == string )
        {
        condition = RMobilePhone::ECallForwardingAllCases;
        }
    else if ( _L("CallForwardingAllConditionalCases") == string )
        {
        condition = RMobilePhone::ECallForwardingAllConditionalCases;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected condition for call forwarding not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( supported && iHandleSpecifiedRequests )
        {
        iCFCondition = condition;
        }

    RMobilePhone::TMobilePhoneCFChangeV1 group;

    if ( _L("ServiceUnspecified") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EServiceUnspecified;
        }
    else if ( _L("VoiceService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EVoiceService;
        }
    else if ( _L("AuxVoiceService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAuxVoiceService;
        }
    else if ( _L("CircuitDataService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::ECircuitDataService;
        }
    else if ( _L("PacketDataService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EPacketDataService;
        }
    else if ( _L("FaxService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EFaxService;
        }
    else if ( _L("ShortMessageService") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EShortMessageService;
        }
    else if ( _L("AllServices") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllServices;
        }
    else if ( _L("AllTele") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllTele;
        }
    else if ( _L("Telephony") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::ETelephony;
        }
    else if ( _L("AllDataTele") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllDataTele;
        }
     else if ( _L("AllDataExSms") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllDataExSms;
        }
    else if ( _L("AllTeleExcSms") == groupstring )
        {
        group.iServiceGroup = RMobilePhone::EAllTeleExcSms;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected set call forwarding servicegroup not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( _L("ServiceActionRegister") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionRegister;
        }
    else if ( _L("ServiceActionActivate") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionActivate;
        }
    else if ( _L("ServiceActionDeactivate") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionDeactivate;
        }
    else if ( _L("ServiceActionErase") == actionstring )
        {
        group.iAction = RMobilePhone::EServiceActionErase;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected set call forwarding action not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( _L("UnknownNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
        }
    else if ( _L("InternationalNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
        }
    else if ( _L("NationalNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
        }
    else if ( _L("NetworkSpecificNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::ENetworkSpecificNumber;
        }
    else if ( _L("SubscriberNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::ESubscriberNumber;
        }
    else if ( _L("AlphanumericNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EAlphanumericNumber;
        }
    else if ( _L("AbbreviatedNumber") == typestring )
        {
        group.iNumber.iTypeOfNumber = RMobilePhone::EAbbreviatedNumber;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected set call forwarding type of number not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( _L("UnknownNumberingPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
        }
    else if ( _L("IsdnNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
        }
    else if ( _L("DataNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EDataNumberPlan;
        }
    else if ( _L("TelexNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::ETelexNumberPlan;
        }
    else if ( _L("ServiceCentreSpecificPlan1") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EServiceCentreSpecificPlan1;
        }
    else if ( _L("ServiceCentreSpecificPlan2") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EServiceCentreSpecificPlan2;
        }
    else if ( _L("NationalNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::ENationalNumberPlan;
        }
    else if ( _L("PrivateNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EPrivateNumberPlan;
        }
    else if ( _L("ERMESNumberPlan") == planstring )
        {
        group.iNumber.iNumberPlan = RMobilePhone::EERMESNumberPlan;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected set call forwarding number plan not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    group.iNumber.iTelNumber.Copy(numberstring);

    if ( _L("CallForwardingNoReply") == string && _L("ServiceActionRegister") == actionstring)
        {

        TLex lex(timestring);

        TInt dataValue = 0;
        lex.Val(dataValue);

        for(TInt i = 5; i < 30; i = i+5)
            {
            if( dataValue == i )
                {
                group.iTimeout = i;
                }

            if( dataValue != i && i == 30 )
                {
                _LIT8(KNotSupp, "SS::Selected timeout is not supported");
                    iSSLog->Log((TDesC8)KNotSupp);
                supported = EFalse;
                }
            }
        }
    else
        {
            group.iTimeout = -1;
        }

    if( supported )
        {
        RMobilePhone::TMobilePhoneCFChangeV1* groupPtr = &group;

         //Create package
        CMmDataPackage package;
        // Set package data
        package.PackData( &condition, &groupPtr);

        ret = HandleRequestL(
            EMobilePhoneSetCallForwardingStatus, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetCallForwardingStatus request send ok" );
            iSSLog->Log((TDesC8)KSendOk );
            }
        else
            {
            _LIT8( KSendFailed, "SetCallForwardingStatus request send failed: %d" );
            iSSLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSS::CompleteSetCallForwardingStatus
// Complete SetCallForwardingStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteSetCallForwardingStatus
    (
    TInt aResult                 // Completion result value
    )
  {

  _LIT8( KResult, "CompleteSetCallForwardingStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if ( KErrNone != aResult )
    {
    _LIT8( KFail, "CompleteSetCallForwardingStatus Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
    }

  Signal( aResult );

  }

// -----------------------------------------------------------------------------
// CSS::SetCallWaitingStatus
// Setting call waiting status
// -----------------------------------------------------------------------------
//

TInt CSS::SetCallWaitingStatus( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::SetCallWaitingStatus");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC groupstring;
    TPtrC conditionstring;

    aItem.GetNextString ( groupstring );
    aItem.GetNextString ( conditionstring );

    RMobilePhone::TMobileService group;

    if ( _L("ServiceUnspecified") == groupstring )
      {
      group = RMobilePhone::EServiceUnspecified;
      }
    else if ( _L("VoiceService") == groupstring )
      {
      group = RMobilePhone::EVoiceService;
      }
    else if ( _L("AuxVoiceService") == groupstring )
      {
      group = RMobilePhone::EAuxVoiceService;
      }
    else if ( _L("CircuitDataService") == groupstring )
      {
      group = RMobilePhone::ECircuitDataService;
      }
    else if ( _L("PacketDataService") == groupstring )
      {
      group = RMobilePhone::EPacketDataService;
      }
    else if ( _L("FaxService") == groupstring )
      {
      group = RMobilePhone::EFaxService;
      if ( iHandleSpecifiedRequests )
          {
          iCWInfoEntry.iServiceGroup = RMobilePhone::EFaxService;
          }
      }
    else if ( _L("ShortMessageService") == groupstring )
      {
      group = RMobilePhone::EShortMessageService;
      }
    else if ( _L("AllServices") == groupstring )
      {
      group = RMobilePhone::EAllServices;
      }
    else if ( _L("AllTele") == groupstring )
      {
      group = RMobilePhone::EAllTele;
      if ( iHandleSpecifiedRequests )
          {
          iCWInfoEntry.iServiceGroup = RMobilePhone::EAllTele;
          }
      }
    else if ( _L("Telephony") == groupstring )
      {
      group = RMobilePhone::ETelephony;
      }
    else if ( _L("AllDataTele") == groupstring )
      {
      group = RMobilePhone::EAllDataTele;
      }
     else if ( _L("AllDataExSms") == groupstring )
      {
      group = RMobilePhone::EAllDataExSms;
      }
    else if ( _L("AllTeleExcSms") == groupstring )
      {
      group = RMobilePhone::EAllTeleExcSms;
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected set call waiting servicegroup not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }

    RMobilePhone::TMobilePhoneServiceAction condition;

    if ( _L("ServiceActionActivate") == conditionstring )
        {
        condition = RMobilePhone::EServiceActionActivate;
        if ( iHandleSpecifiedRequests )
            {
            iCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusActive;
            }
        }
      else if ( _L("ServiceActionDeactivate") == conditionstring )
        {
        condition = RMobilePhone::EServiceActionDeactivate;
        if ( iHandleSpecifiedRequests )
            {
            iCWInfoEntry.iStatus = RMobilePhone::ECallWaitingStatusNotActive;
            }
        }
      else
        {
       _LIT8(KNotSupp, "SS::Selected condition for call waiting not supported");
          iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

      if( supported )
        {

         //Create package
        CMmDataPackage package;
        // Set package data
        package.PackData( &group, &condition);

      ret = HandleRequestL( EMobilePhoneSetCallWaitingStatus,
                           &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SetCallWaitingStatus request send ok" );
            iSSLog->Log((TDesC8)KSendOk );
          }
        else
            {
             _LIT8( KSendFailed, "SetCallWaitingStatus request send failed: %d" );
            iSSLog->Log((TDesC8)KSendFailed, ret );
          }
        }
      else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSS::CompleteSetCallWaitingStatus
// Complete SetCallWaitingStatus method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteSetCallWaitingStatus
    (
    TInt aResult
    )
  {
  _LIT8( KResult, "CompleteSetCallWaitingStatus result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if( KErrNone != aResult )
    {
    _LIT8( KFail, "CompleteSetCallWaitingStatus Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
    }

  Signal( aResult );

  }

// -----------------------------------------------------------------------------
// CSS::SetSupplementaryServicesPassword
// Setting supplementary services password
// -----------------------------------------------------------------------------
//

TInt CSS::SetSupplementaryServicesPassword( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::SetCallBarringStatus");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );
    TBool supported( ETrue );
    TPtrC oldpassword;
    TPtrC newpassword;
    TPtrC verifiedpassword;
    TPtrC scstring;

    TUint16 servicecode;

    aItem.GetNextString ( oldpassword );
    aItem.GetNextString ( newpassword );
    aItem.GetNextString ( verifiedpassword );
    aItem.GetNextString ( scstring );

    RMobilePhone::TMobilePhonePasswordChangeV2 passwordgroup;

    if ( KDontCare() != oldpassword )
        {
        passwordgroup.iOldPassword.Copy(oldpassword);
        }
    if ( KDontCare() != newpassword )
        {
        passwordgroup.iNewPassword.Copy(newpassword);
        }
    if ( KDontCare() != verifiedpassword )
        {
        passwordgroup.iVerifiedPassword.Copy(verifiedpassword);
        }

    if ( _L("0") == scstring )
      {
      servicecode = 0;
      }
    else if ( _L("330") == scstring )
      {
      servicecode = 330;
      }
    else
      {
     _LIT8(KNotSupp, "SS::Selected service code not supported");
        iSSLog->Log((TDesC8)KNotSupp);
      supported = EFalse;
      }



    if( supported )
      {


       //Create package
      CMmDataPackage package;
      // Set package data
      package.PackData( &passwordgroup, &servicecode );

    ret = HandleRequestL( EMobilePhoneSetSSPassword,
                         &package );

      if ( KErrNone == ret )
          {
          _LIT8( KSendOk, "SetSupplementaryServicesPassword request send ok" );
          iSSLog->Log((TDesC8)KSendOk );
        }
      else
          {
           _LIT8( KSendFailed, "SetSupplementaryServicesPassword request send failed: %d" );
          iSSLog->Log((TDesC8)KSendFailed, ret );
        }
      }
    else
      {
      ret = KErrNotSupported;
      }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSS::CompleteSetSupplementaryServicesPassword
// Complete SetSupplementaryServicesPassword method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteSetSupplementaryServicesPassword
    (
    TInt aResult
    )
  {
  _LIT8( KResult, "CompleteSetSupplementaryServicesPassword result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if( KErrNone != aResult )
    {
    _LIT8( KFail, "CompleteSetSupplementaryServicesPassword Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
    }
  Signal( aResult );
  }


// -----------------------------------------------------------------------------
// CSS::PrintStatusInfo
// Printing SS status changes
// -----------------------------------------------------------------------------
//

void CSS::PrintStatusInfo( CMmDataPackage* aDataPackage, TInt aIpc )
  {

  switch(aIpc)
      {
      case EMobilePhoneGetIdentityServiceStatus:
          {
          RMobilePhone::TMobilePhoneIdServiceStatus status;
          aDataPackage->UnPackData ( status );

          switch(status)
              {
              case RMobilePhone::EIdServiceActivePermanent:
                  {
                  _LIT8(KStatus, "CSS::Status is EIdServiceActivePermanent");
                  iSSLog->Log((TDesC8)KStatus );
                  break;
                  }
              case RMobilePhone::EIdServiceActiveDefaultAllowed:
                  {
                  _LIT8(KStatus, "CSS::Status is EIdServiceActiveDefaultAllowed");
                  iSSLog->Log((TDesC8)KStatus );
                  break;
                  }
              case RMobilePhone::EIdServiceNotProvisioned:
                  {
                  _LIT8(KStatus, "CSS::Status is EIdServiceNotProvisioned");
                  iSSLog->Log((TDesC8)KStatus );
                  break;
                  }
              case RMobilePhone::EIdServiceUnknown:
                 {
                 _LIT8(KStatus, "CSS::Status is EIdServiceUnknown");
                 iSSLog->Log((TDesC8)KStatus );
                 break;
                 }
              default:
                 {
                 _LIT8(KStatus, "CSS::Not valid status");
                 iSSLog->Log((TDesC8)KStatus );
                 break;
                 }
              }
          }
      default:
            {
            break;
            }
      }

  }



// -----------------------------------------------------------------------------
// CSS::GetCallForwardingNumber
// Getting Call Forwarding Number
// -----------------------------------------------------------------------------
//

TInt CSS::GetCallForwardingNumber( CStifItemParser& /*aItem*/ )
    {
    _LIT8(KPbInit, "SS::GetCallForwardingNumber");
    iSSLog->Log((TDesC8)KPbInit);


    TBool supported( ETrue );
    TInt ret( KErrNotFound );

    if( supported )
        {

        ret = HandleRequestL( EMmTsyGetCallForwardingNumberIPC );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "GetCallForwardingNumber request send ok" );
            iSSLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "GetCallForwardingNumber request send failed: %d" );
            iSSLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;


    }

// -----------------------------------------------------------------------------
// CSS::CompleteGetCallForwardingNumber
// Complete GetCallForwardingNumber method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteGetCallForwardingNumber
    (
    TInt aResult, CMmDataPackage* aDataPackage
    )
  {
  _LIT8( KResult, "CompleteGetCallForwardingNumber result: %d" );
  iSSLog->Log((TDesC8)KResult, aResult );

  if( KErrNone == aResult )
      {
      TBuf<255> number;

      aDataPackage->UnPackData( number );

      TBuf8<255> Buffer;
      _LIT8(KRowFormatter, "Call forwarding number: %S");
      Buffer.Format(KRowFormatter, &number);
      }
  else
      {
      _LIT8( KFail, "CompleteGetCallForwardingNumber Failed: %d" );
      iSSLog->Log((TDesC8)KFail, aResult );
      }

  Signal( aResult );

  }


// -----------------------------------------------------------------------------
// CSS::SendUSSDMessage
// Sending USSD Message
// -----------------------------------------------------------------------------
//

TInt CSS::SendUSSDMessage( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::SendUSSDMessage");
    iSSLog->Log((TDesC8)KPbInit);


    TBool supported( ETrue );
    TInt ret( KErrNotFound );

    TInt dcs;
    TPtrC type;
    TPtrC format;
    TInt flags;

    aItem.GetNextInt( dcs );
    aItem.GetNextString( type );
    aItem.GetNextString( format );
    aItem.GetNextInt( flags );


    RMobileUssdMessaging::TMobileUssdAttributesV1 attributes = RMobileUssdMessaging::TMobileUssdAttributesV1();

    if ( _L("UssdUnknown") == type )
        {
        attributes.iType = RMobileUssdMessaging::EUssdUnknown;
        }
    else if ( _L("UssdMORequest") == type )
        {
        attributes.iType = RMobileUssdMessaging::EUssdMORequest;
        }
    else if ( _L("UssdMOReply") == type )
        {
        attributes.iType = RMobileUssdMessaging::EUssdMOReply;
        }
    else if ( _L("UssdMTNotify") == type )
        {
        attributes.iType = RMobileUssdMessaging::EUssdMTNotify;
        }
    else if ( _L("UssdMTRequest") == type )
        {
        attributes.iType = RMobileUssdMessaging::EUssdMTRequest;
        }
    else if ( _L("UssdMTReply") == type )
        {
        attributes.iType = RMobileUssdMessaging::EUssdMTReply;
        }
    else if ( _L("UssdMOAcknowledgement") == type )
        {
        attributes.iType = RMobileUssdMessaging::EUssdMOAcknowledgement;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected USSD type not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }

    if ( _L("FormatUnspecified") == format )
        {
        attributes.iFormat = RMobileUssdMessaging::EFormatUnspecified;
        }
    else if ( _L("FormatPackedString") == format )
        {
        attributes.iFormat = RMobileUssdMessaging::EFormatPackedString;
        }
    else
        {
        _LIT8(KNotSupp, "SS::Selected USSD format not supported");
        iSSLog->Log((TDesC8)KNotSupp);
        supported = EFalse;
        }


    if ( flags >= 0 && flags <= 7 )
        {
        attributes.iFlags |= flags;
        }
    else
       {
       _LIT8(KNotSupp, "SS::Selected flag not supported");
       iSSLog->Log((TDesC8)KNotSupp);
       supported = EFalse;
       }

    //data coding scheme settings
    attributes.iDcs = dcs;

    RMobileUssdMessaging::TGsmUssdMessageData messageData;

    TInt tmp;

    //add message data
    while( aItem.GetNextInt(tmp) == KErrNone )
        {
        messageData.Append(tmp);
        }

    RMobileUssdMessaging::TMobileUssdAttributesV1Pckg pckg(attributes);

    RMobileUssdMessaging::TMobileUssdAttributesV1Pckg* ptr = &pckg;

    if( supported )
        {
         //Create package
        CMmDataPackage package;
        // Set package data
        package.PackData( &messageData, &ptr  );

        ret = HandleRequestL( EMobileUssdMessagingSendMessage, &package );

        if ( KErrNone == ret )
            {
            _LIT8( KSendOk, "SendUSSDMessageNumber request send ok" );
            iSSLog->Log((TDesC8)KSendOk );
            }
        else
            {
             _LIT8( KSendFailed, "SendUSSDMessage request send failed: %d" );
            iSSLog->Log((TDesC8)KSendFailed, ret );
            }
        }
    else
        {
        ret = KErrNotSupported;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSS::CompleteSendUSSDMessage
// Complete SendUSSDMessage method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteSendUSSDMessage
    (
    TInt aResult, CMmDataPackage* /*aDataPackage*/
    )
    {
    _LIT8( KResult, "CompleteSendUSSDMessage result: %d" );
    iSSLog->Log((TDesC8)KResult, aResult );


    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteSendUSSDMessage Failed: %d" );
        iSSLog->Log((TDesC8)KFail, aResult );
        }

    if( ( !iSSFailTestOngoing ) ||
        ( iHandleSpecifiedRequests ) )
        {
        Signal( aResult );
        }
    }


// -----------------------------------------------------------------------------
// CSS::SSServiceFailed
// -----------------------------------------------------------------------------
//
TInt CSS::SSServiceFailed( CStifItemParser& aItem )
    {

    TBool supported( ETrue );

    TPtrC whereToComplete;

    aItem.GetNextString( whereToComplete );

    if ( whereToComplete == _L("USSDMessHandler") )
      {
      iCompleteFailInUSSDHandler = ETrue;
      }
    else if ( whereToComplete == _L("SupplServMessHandler") )
      {
      iCompleteFailInUSSDHandler = EFalse;
      }
    else
      {
      iSSLog->Log( _L8("Completion place not supported") );
      supported = EFalse;
      }

    if( supported )
        {
        iSSLog->Log(_L8("CSS::SSServiceFailed test started"));
        iSSFailTestOngoing = ETrue;
        return KErrNone;
        }
      else
        {
        return KErrNotSupported;
        }
    }

// -----------------------------------------------------------------------------
// CSS::CompleteSSServiceFailed
// -----------------------------------------------------------------------------
//

void CSS::CompleteSSServiceFailed( TInt aResult )
    {
    iSSLog->Log(_L8("CSS::CompleteSSServiceFailed, err=%d"),aResult);

    if (iSSFailTestOngoing )
        {
        iSSFailTestOngoing = EFalse;
        iSSLog->Log(_L8("CSS::SSServiceFailed test completed"));
        Signal( aResult );
        }
    }


// -----------------------------------------------------------------------------
// CSS::USSDSendRelease
// -----------------------------------------------------------------------------
//
TInt CSS::USSDSendRelease( CStifItemParser& /*aItem*/ )
    {

    TInt ret( KErrNotFound );

    iUSSDSendReleaseTestOngoing = ETrue;

    ret = HandleRequestL( EMobileUssdMessagingSendRelease );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "USSDSendRelease request send ok" );
        iSSLog->Log( (TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "USSDSendRelease request send failed: %d" );
        iSSLog->Log( (TDesC8)KSendFailed, ret );
        }

    iSSLog->Log(_L8("CSS::USSDSendRelease test started"));

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSS::CompleteUSSDSendRelease
// -----------------------------------------------------------------------------
//

void CSS::CompleteUSSDSendRelease( TInt aResult )
    {
    iSSLog->Log(_L8("CSS::CompleteUSSDSendRelease, err=%d"),aResult);

    if( iUSSDSendReleaseTestOngoing )
        {
        iUSSDSendReleaseTestOngoing = EFalse;
        iSSLog->Log( _L8("CSS::USSDSendRelease test completed") );
        Signal( aResult );
        }
    }

// -----------------------------------------------------------------------------
// CSS::SendNetworkServiceRequest
// Sending network service request
// -----------------------------------------------------------------------------
//

TInt CSS::SendNetworkServiceRequest( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "SS::SendNetworkServiceRequest");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );

    TPtrC messageData;
    aItem.GetNextString( messageData );

    TPtrC* messageToSend;

    messageToSend = &messageData;

   //Create package
    CMmDataPackage package;
    // Set package data
    package.PackData( &messageToSend );

    ret = HandleRequestL( EMobilePhoneSendNetworkServiceRequest, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SendNetworkServiceRequest request send ok" );
        iSSLog->Log( (TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "SendNetworkServiceRequest request send failed: %d" );
        iSSLog->Log( (TDesC8)KSendFailed, ret );
        }

  return ret;

  }

// -----------------------------------------------------------------------------
// CSS::CompleteSendNetworkServiceRequest
// Complete SendNetworkServiceRequest
// -----------------------------------------------------------------------------
//
void CSS::CompleteSendNetworkServiceRequest
    (
    TInt aResult, CMmDataPackage* /*aDataPackage*/
    )
    {
    _LIT8( KResult, "CompleteSendNetworkServiceRequest result: %d" );
    iSSLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
      {
      _LIT8( KFail, "CompleteSendNetworkServiceRequest Failed: %d" );
        iSSLog->Log( (TDesC8)KFail, aResult );
      }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSS::SendNetworkServiceRequestNoFdnCheck
// Sending network service request
// -----------------------------------------------------------------------------
//
TInt CSS::SendNetworkServiceRequestNoFdnCheck( CStifItemParser& aItem )
    {
    _LIT8(KPbInit, "CSS::SendNetworkServiceRequestNoFdnCheck");
    iSSLog->Log((TDesC8)KPbInit);

    TInt ret( KErrNotFound );

    TPtrC messageData;
    aItem.GetNextString( messageData );

    TPtrC* messageToSend;

    messageToSend = &messageData;

   //Create package
    CMmDataPackage package;
    // Set package data
    package.PackData( &messageToSend );

    ret = HandleRequestL( EMobilePhoneSendNetworkServiceRequestNoFdnCheck, &package );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SendNetworkServiceRequestNoFdnCheck request send ok" );
        iSSLog->Log( (TDesC8)KSendOk );
        }
    else
        {
         _LIT8( KSendFailed, "SendNetworkServiceRequestNoFdnCheck request send failed: %d" );
        iSSLog->Log( (TDesC8)KSendFailed, ret );
        }

  return ret;

  }

// -----------------------------------------------------------------------------
// CSS::CompleteSendNetworkServiceRequestNoFdnCheck
// Complete CompleteSendNetworkServiceRequestNoFdnCheck
// -----------------------------------------------------------------------------
//
void CSS::CompleteSendNetworkServiceRequestNoFdnCheck
    (
    TInt aResult, CMmDataPackage* /*aDataPackage*/
    )
    {
    _LIT8( KResult, "CompleteSendNetworkServiceRequestNoFdnCheck result: %d" );
    iSSLog->Log((TDesC8)KResult, aResult );

    if( KErrNone != aResult )
      {
      _LIT8( KFail, "CompleteSendNetworkServiceRequestNoFdnCheck Failed: %d" );
        iSSLog->Log( (TDesC8)KFail, aResult );
      }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSS::SSServiceCompleted
// -----------------------------------------------------------------------------
//
TInt CSS::SSServiceCompleted( CStifItemParser& /*aItem*/ )
    {

    iSSLog->Log( _L8("CSS::SSServiceCompleted test started") );
    iSSFailTestOngoing = ETrue;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSS::CompleteSSServiceCompleted
// -----------------------------------------------------------------------------
//
void CSS::CompleteSSServiceCompleted
    (
    TInt aResult                // Completion result value
    )
    {
    iSSLog->Log(_L8("CSS::SSServiceCompleted, err=%d"),aResult);
        if (iSSCompleteTestOngoing )
            {
            iSSFailTestOngoing = EFalse;
            iSSLog->Log( _L8("CSS::SSServiceCompleted test completed") );
            Signal( aResult );
            }
    }

// -----------------------------------------------------------------------------
// CSS::CompleteNotifyCallBarringStatusChange
// Complete NotifyCallBarringStatusChange method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteNotifyCallBarringStatusChange
    (
    TInt aResult,                // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    RMobilePhone::TMobilePhoneCBCondition unpackedCBCondition;

    _LIT8( KResult, "CompleteNotifyCallBarringStatusChange result: %d" );
    iSSLog->Log((TDesC8)KResult, aResult );

    if ( ( iHandleSpecifiedRequests ) && ( KErrNone == aResult ) )
        {
        aDataPackage->UnPackData( unpackedCBCondition );
        _LIT8( KCBCondition,
            "CompleteNotifyCallBarringStatusChange unpackedCBCondition: %d" );
        iSSLog->Log((TDesC8)KCBCondition, unpackedCBCondition );

        if ( iCBCondition != unpackedCBCondition )
            {
            aResult = KErrGeneral;
            }
        }

    if ( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteNotifyCallBarringStatusChange Failed: %d" );
        iSSLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSS::CompleteNotifyCallForwardingStatusChange
// Complete NotifyCallBarringStatusChange method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteNotifyCallForwardingStatusChange
    (
    TInt aResult,                // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    RMobilePhone::TMobilePhoneCFCondition unpackedCFcondition;

    _LIT8( KResult, "CompleteNotifyCallForwardingStatusChange result: %d" );
    iSSLog->Log((TDesC8)KResult, aResult );

    if ( ( iHandleSpecifiedRequests ) && ( KErrNone == aResult ) )
        {
        aDataPackage->UnPackData( unpackedCFcondition );
        _LIT8( KCBCondition,
            "CompleteNotifyCallForwardingStatusChange unpackedCFCondition: %d" );
        iSSLog->Log((TDesC8)KCBCondition, unpackedCFcondition );

        if ( iCFCondition != unpackedCFcondition )
            {
            aResult = KErrGeneral;
            }
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteNotifyCallForwardingStatusChange Failed: %d" );
        iSSLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSS::CompleteNotifyCallWaitingStatusChange
// Complete NotifyCallBarringStatusChange method function.
// -----------------------------------------------------------------------------
//
void CSS::CompleteNotifyCallWaitingStatusChange
    (
    TInt aResult,                // Completion result value
    CMmDataPackage* aDataPackage // pointer to DataPackage
    )
    {
    RMobilePhone::TMobilePhoneCWInfoEntryV1 unpackedCWCondition;

    _LIT8( KResult, "CompleteNotifyCallWaitingStatusChange result: %d" );
    iSSLog->Log((TDesC8)KResult, aResult );

    if ( ( iHandleSpecifiedRequests ) && ( KErrNone == aResult ) )
        {
        aDataPackage->UnPackData( unpackedCWCondition );
        _LIT8( KCBCondition,
            "CompleteNotifyCallWaitingStatusChange unpackedCWCondition: %d" );
        iSSLog->Log((TDesC8)KCBCondition, unpackedCWCondition );

        if ( ( iCWInfoEntry.iStatus != unpackedCWCondition.iStatus ) ||
             ( iCWInfoEntry.iServiceGroup != unpackedCWCondition.iServiceGroup ) )
            {
            aResult = KErrGeneral;
            }
        }

    if( KErrNone != aResult )
        {
        _LIT8( KFail, "CompleteNotifyCallWaitingStatusChange Failed: %d" );
        iSSLog->Log((TDesC8)KFail, aResult );
        }

    Signal( aResult );

    }

// -----------------------------------------------------------------------------
// CSS::CompleteNotifySendNetworkServiceRequest
// Complete NotifySendNetworkServiceRequest
// -----------------------------------------------------------------------------
//
void CSS::CompleteNotifySendNetworkServiceRequest(
        TInt aResult,
        CMmDataPackage* /* aDataPackage */
        )
    {
    _LIT8 ( KCompleteNotifySendNetworkServiceRequest, "CSS::CompleteNotifySendNetworkServiceRequest" );
    Signal( aResult );
    }

// -----------------------------------------------------------------------------
// CSS::SetSendNetworkServiceRequestTestOngoingFlag
// Complete NotifyCallBarringStatusChange method function.
// -----------------------------------------------------------------------------
//
TInt CSS::SetSendNetworkServiceRequestTestOngoingFlag( CStifItemParser& aItem )
    {
    _LIT8 ( KSetSendNetworkServiceRequestTestOngoingFlag, "CSS::SetSendNetworkServiceRequestTestOngoingFlag" );
    iCoreLog->Log( ( TDesC8 )KSetSendNetworkServiceRequestTestOngoingFlag );

    TPtrC string;
    aItem.GetNextString ( string );
    if ( _L("YES") == string )
        {
        iSSSendNetworkServiceRequestTestOngoing = ETrue;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSS::SendSatNotifyCallControlRequest
// -----------------------------------------------------------------------------
//
TInt CSS::SendSatNotifyCallControlRequest( CStifItemParser& /*aItem*/ )
    {

    TInt ret( KErrNotFound );

    ret = HandleRequestL( ESatNotifyCallControlRequest );

    if ( KErrNone == ret )
        {
        _LIT8( KSendOk, "SendSatNotifyCallControlRequest request send ok" );
        iSSLog->Log( (TDesC8)KSendOk );
        }
    else
        {
        _LIT8( KSendFailed, "SendSatNotifyCallControlRequest request send failed: %d" );
        iSSLog->Log( (TDesC8)KSendFailed, ret );
        }

    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


//  End of File
