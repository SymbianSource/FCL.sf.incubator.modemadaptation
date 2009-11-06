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
* Description:   Specific class for SS tests
*
*/




// INCLUDE FILES
#include <stiftestinterface.h>
#include "ss.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSS::CSS
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSS::CSS(
    CTestModuleIf& aTestModuleIf ):
        CCore( aTestModuleIf )
    {
    // Initial values
    }

// -----------------------------------------------------------------------------
// CSS::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSS::ConstructL()
    {
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
    BaseConstructL();
#else
    BaseConstructL( this );
#endif

    iSSLog = CStifLogger::NewL( KSSLogPath,
                          KSSLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    InitializeIscControlL(KIscControlTestCaseFileCSS);

    _LIT8(KSS, "CSS: ConstructL");
    iSSLog->Log((TDesC8)KSS );

    iSSSendNetworkServiceRequestTestOngoing = EFalse;
    }

// -----------------------------------------------------------------------------
// CSS::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSS* CSS::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CSS* self = new (ELeave) CSS( aTestModuleIf );

    CleanupStack::PushL((CScriptBase*)self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CSS::~CSS()
    {

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iSSLog;

    }

// -----------------------------------------------------------------------------
// CSS::RouteCompletion
// Completes a request or notification via the specific Tsy object, based on
// IPC number and data package
// -----------------------------------------------------------------------------
//
EXPORT_C void CSS::RouteCompletion
        (
        TInt aIpc,
        CMmDataPackage* aDataPackage,
        TInt aResult
        )
    {

    // Process the ISI msg according to IPC requested.
    _LIT8(KResult, "CSS::RouteCompletion result: %d IPC: %d");
    iSSLog->Log((TDesC8)KResult, aResult, aIpc );
    if ( !iHandleSpecifiedRequests )
        {
        switch(aIpc)
          {
          case EMobilePhoneGetBarringStatusPhase1:
            {
            CompleteGetCallBarringStatus( aResult, aDataPackage );
            break;
            }
          case EMobilePhoneGetCallForwardingStatusPhase1:
            {
            if ( iSSCompleteTestOngoing ) //for SS service completed test
              {
              CompleteSSServiceCompleted( aResult );
              }
            else if ( iSSFailTestOngoing )
              {
              if ( !iCompleteFailInUSSDHandler ) //for USSD error completion
                {
                CompleteSSServiceFailed( aResult );
                }
              }
            else
              {
              CompleteGetCallForwardingStatus( aResult, aDataPackage );
              }
            break;

            }
          case EMobilePhoneGetWaitingStatusPhase1:
            {
            CompleteGetCallWaitingStatus( aResult, aDataPackage );
            break;
            }
          case EMobilePhoneGetIdentityServiceStatus:
            {
            //PrintStatusInfo( aDataPackage, aIpc );
            CompleteGetIdentityServiceStatus( aResult, aDataPackage );
            break;
            }
          case EMobilePhoneSetCallBarringStatus:
            {
            CompleteSetCallBarringStatus( aResult );
            break;
            }
          case EMobilePhoneSetCallForwardingStatus:
            {
            CompleteSetCallForwardingStatus( aResult );
            break;
            }
          case EMobilePhoneSetCallWaitingStatus:
            {
            CompleteSetCallWaitingStatus( aResult );
            break;
            }
          case EMobilePhoneSetSSPassword:
            {
            CompleteSetSupplementaryServicesPassword( aResult );
            break;
            }
          case EMmTsyGetCallForwardingNumberIPC:
            {
            CompleteGetCallForwardingNumber( aResult, aDataPackage );
            break;
            }
          case EMobileUssdMessagingSendMessage:
            {
            if ( !iSSFailTestOngoing ) //for USSD message sending
              {
              CompleteSendUSSDMessage( aResult, aDataPackage );
              }
            else if ( iSSFailTestOngoing )
              {
              if ( iCompleteFailInUSSDHandler ) //for USSD error completion
                {
                CompleteSSServiceFailed( aResult );
                }
              }
            break;
            }
          case EMobileUssdMessagingNotifyNetworkRelease:
            {
            CompleteUSSDSendRelease( aResult );
            break;
            }
          case EMobilePhoneSendNetworkServiceRequest:
            {
            CompleteSendNetworkServiceRequest( aResult, aDataPackage );
            break;
            }
          case EMobilePhoneSetIccMessageWaitingIndicators:
              {
            CompleteSetIccMessageWaitingIndicators( aResult );
              break;
              }
          case EMobilePhoneGetIccMessageWaitingIndicators:
              {
            CompleteGetIccMessageWaitingIndicators( aResult, aDataPackage );
              break;
              }
          case ECustomGetIccCallForwardingStatusIPC:
              {
              CompleteGetIccCallForwardingStatus( aResult, aDataPackage );
              break;
              }
            default:
            {
              _LIT8(KDefault, "CSS::RouteCompletion default");
            iSSLog->Log((TDesC8)KDefault );
            break;
            }
          }
        }
    else
        {
        switch(aIpc)
            {
            case EMobilePhoneGetBarringStatusPhase1:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetBarringStatusPhase1" ) ) )
                    {
                    CompleteGetCallBarringStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneGetCallForwardingStatusPhase1:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetCallForwardingStatusPhase1" ) ) )
                    {
                    CompleteGetCallForwardingStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneGetWaitingStatusPhase1:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetWaitingStatusPhase1" ) ) )
                    {
                    CompleteGetCallWaitingStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneGetIdentityServiceStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetIdentityServiceStatus" ) ) )
                    {
                    CompleteGetIdentityServiceStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneNotifyCallBarringStatusChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyCallBarringStatusChange" ) ) )
                    {
                    CompleteNotifyCallBarringStatusChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneNotifyCallForwardingStatusChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyCallForwardingStatusChange" ) ) )
                    {
                    CompleteNotifyCallForwardingStatusChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneSetCallForwardingStatus:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneSetCallForwardingStatus" ) ) )
                    {
                    CompleteSetCallForwardingStatus( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneNotifyCallWaitingStatusChange:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneNotifyCallWaitingStatusChange" ) ) )
                    {
                    CompleteNotifyCallWaitingStatusChange( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobilePhoneSetSSPassword:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneSetSSPassword" ) ) )
                    {
                    CompleteSetSupplementaryServicesPassword( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case EMobileUssdMessagingSendMessage:
                  {
                  if ( 0 == iSavedIPCNameForComparison.Compare
                         ( _L( "EMobileUssdMessagingSendMessage" ) ) )
                      {
                      CompleteSendUSSDMessage( aResult, aDataPackage );
                      CCore::GetNextExpectedCompleteIPC();
                      }
                  break;
                  }

            case EMobilePhoneGetIccMessageWaitingIndicators:
                  {
                  if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneGetIccMessageWaitingIndicators" ) ) )
                      {
                      CompleteGetIccMessageWaitingIndicators( aResult, aDataPackage );
                      CCore::GetNextExpectedCompleteIPC();
                      }
                  break;
                  }

            case EMobilePhoneSetIccMessageWaitingIndicators:
                  {
                  if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobilePhoneSetIccMessageWaitingIndicators" ) ) )
                      {
                      CompleteSetIccMessageWaitingIndicators( aResult );
                      CCore::GetNextExpectedCompleteIPC();
                      }
                  break;
                  }

            case EMobilePhoneSendNetworkServiceRequest:
              {
              if ( 0 == iSavedIPCNameForComparison.Compare
                      ( _L( "EMobilePhoneSendNetworkServiceRequest" ) ) )
                     {
                     if ( ! iSSSendNetworkServiceRequestTestOngoing )
                         {
                         CompleteSendNetworkServiceRequest( aResult, aDataPackage );
                         }
                     CCore::GetNextExpectedCompleteIPC();
                     }
              break;
              }

            case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
              {
              if ( 0 == iSavedIPCNameForComparison.Compare
                      ( _L( "EMobilePhoneSendNetworkServiceRequestNoFdnCheck" ) ) )
                     {
                     if ( ! iSSSendNetworkServiceRequestTestOngoing )
                         {
                         CompleteSendNetworkServiceRequestNoFdnCheck( aResult,aDataPackage );
                         }
                     CCore::GetNextExpectedCompleteIPC();
                     }
              break;
              }

            #if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
            case EMobilePhoneNotifySendNetworkServiceRequest:
                {
                CompleteNotifySendNetworkServiceRequest( aResult,aDataPackage );
                break;
              }
            #else
            case EMobilePhoneNotifyAllSendNetworkServiceRequest:
                {
                CompleteNotifySendNetworkServiceRequest( aResult,aDataPackage );
                break;
                }
            #endif // NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32

            case EMobileUssdMessagingNotifyNetworkRelease:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "EMobileUssdMessagingNotifyNetworkRelease" ) ) )
                    {
                    CompleteUSSDSendRelease( aResult );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }

            case ECustomGetIccCallForwardingStatusIPC:
                {
                if ( 0 == iSavedIPCNameForComparison.Compare
                       ( _L( "ECustomGetIccCallForwardingStatusIPC" ) ) )
                    {
                    CompleteGetIccCallForwardingStatus( aResult, aDataPackage );
                    CCore::GetNextExpectedCompleteIPC();
                    }
                break;
                }


            default:
                {
                _LIT8(KDefault, "CSS::RouteCompletion default");
                iSSLog->Log((TDesC8)KDefault );
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

    return ( CScriptBase* ) CSS::NewL( aTestModuleIf );

    }

//  End of File
