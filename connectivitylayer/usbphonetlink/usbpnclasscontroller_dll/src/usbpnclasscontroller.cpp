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
#include "usbpnclasscontroller.h"
#include "usbpndefinitions.h"
#include "usbpntrace.h"

#include "osttracedefinitions.h"    // For tracing
#ifdef OST_TRACE_COMPILER_IN_USE
#include "usbpnclasscontrollerTraces.h"
#endif

#include <usb_std.h>        // For TUsbDescriptor

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

#ifdef _DEBUG
_LIT(KTraceName, "USBPNClassController");
#endif

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------
// CUsbPNClassController::NewL
// ---------------------------------------------------------
CUsbPNClassController* CUsbPNClassController::NewL(MUsbClassControllerNotify& aOwner)
    {
    OstTrace1( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_NEWL_ENTRY, "CUsbPNClassController::NewL;aOwner=%x", ( TUint )&( aOwner ) );
    C_TRACE( ( _T("CUsbPNClassController::NewL(0x%x)" ), &aOwner ) );

    CUsbPNClassController* self = new (ELeave) CUsbPNClassController(aOwner);

    OstTrace1( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_NEWL_EXIT, "CUsbPNClassController::NewL;self=%x", ( TUint ) self );
    C_TRACE( ( _T("CUsbPNClassController::NewL - return 0x%x" ), self ) );
    return self;
    }

// ---------------------------------------------------------
// CUsbPNClassController::CUsbPNClassController
// ---------------------------------------------------------
CUsbPNClassController::CUsbPNClassController(MUsbClassControllerNotify& aOwner)
    : CUsbClassControllerPlugIn(aOwner, 1)
    {
    OstTrace1( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_CUSBPNCLASSCONTROLLER_ENTRY, "CUsbPNClassController::CUsbPNClassController;aOwner=%x", ( TUint )&( aOwner ) );
    C_TRACE( ( _T("CUsbPNClassController::CUsbPNClassController(0x%x)" ), &aOwner ) );

    iState = EUsbServiceIdle;

    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_CUSBPNCLASSCONTROLLER_EXIT, "CUsbPNClassController::CUsbPNClassController - return void" );
    C_TRACE( ( _T("CUsbPNClassController::CUsbPNClassController - return void" ) ) );
    }

// ---------------------------------------------------------
// CUsbPNClassController::~CUsbPNClassController
// ---------------------------------------------------------
CUsbPNClassController::~CUsbPNClassController()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_CUSBPNCLASSCONTROLLER_DESTRUCTOR_ENTRY, "CUsbPNClassController::~CUsbPNClassController" );
    C_TRACE( ( _T("CUsbPNClassController::~CUsbPNClassController()" ) ) );

    Cancel();

    if(iState == EUsbServiceStarted)
        {
        iClient.Detach();
        }

    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_CUSBPNCLASSCONTROLLER_DESTRUCTOR_EXIT, "CUsbPNClassController::~CUsbPNClassController - return void" );
    C_TRACE( ( _T("CUsbPNClassController::~CUsbPNClassController - return void" ) ) );
    }

// ---------------------------------------------------------
// CUsbPNClassController::Start
// ---------------------------------------------------------
void CUsbPNClassController::Start(TRequestStatus& aStatus)
    {
    OstTrace1( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_START_ENTRY, "CUsbPNClassController::Start;aStatus=%x", ( TUint )&( aStatus ) );
    C_TRACE( ( _T("CUsbPNClassController::Start(0x%x)" ), &aStatus ) );

    //Show component build information in traces
    BUILD_TRACE( &KTraceName );

    TRequestStatus* reportStatus = &aStatus;
    TInt err( KErrNotReady );

    if(iState == EUsbServiceIdle)
        {
        iState = EUsbServiceStarting;

        // Connect to server client to start server.
        TRAP( err, iClient.ConnectL() );
        if(!err)
            {
            iState = EUsbServiceStarted;
            }
        }

    User::RequestComplete(reportStatus, err);

    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_START_EXIT, "CUsbPNClassController::Start - return void" );
    C_TRACE( ( _T("CUsbPNClassController::Start - return void" ) ) );
    }

// ---------------------------------------------------------
// CUsbPNClassController::Stop
// ---------------------------------------------------------
void CUsbPNClassController::Stop(TRequestStatus& aStatus)
    {
    OstTrace1( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_STOP_ENTRY, "CUsbPNClassController::Stop;aStatus=%x", ( TUint )&( aStatus ) );
    C_TRACE( ( _T("CUsbPNClassController::Stop(0x%x)" ), &aStatus ) );
    TRequestStatus* ReportStatus = &aStatus;

    if(iState == EUsbServiceStarted)
        {
        iState = EUsbServiceStopping;

        // Close connection to client API to shutdown server.
        iClient.Detach();

        iState = EUsbServiceIdle;
        }
    User::RequestComplete(ReportStatus, KErrNone);

    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_STOP_EXIT, "CUsbPNClassController::Stop - return void" );
    C_TRACE( ( _T("CUsbPNClassController::Stop - return void" ) ) );
    }

// ---------------------------------------------------------
// CUsbPNClassController::GetDescriptorInfo
// ---------------------------------------------------------

void CUsbPNClassController::GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const
    {
    OstTrace1( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_GETDESCRIPTORINFO_ENTRY, "CUsbPNClassController::GetDescriptorInfo;aDescriptorInfo=%x", ( TUint )&( aDescriptorInfo ) );
    C_TRACE( ( _T("CUsbPNClassController::GetDescriptorInfo(0x%x)" ), &aDescriptorInfo ) );

    aDescriptorInfo.iNumInterfaces = KPnNumInterfaces;
    aDescriptorInfo.iLength = KPnDescriptorLength;

    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_GETDESCRIPTORINFO_EXIT, "CUsbPNClassController::GetDescriptorInfo - return void" );
    C_TRACE( ( _T("CUsbPNClassController::GetDescriptorInfo - return void" ) ) );
    }

// ---------------------------------------------------------
// CUsbPNClassController::RunL
// ---------------------------------------------------------
void CUsbPNClassController::RunL()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_RUNL_ENTRY, "CUsbPNClassController::RunL" );
    C_TRACE( ( _T("CUsbPNClassController::RunL()" ) ) );

    __ASSERT_DEBUG(EFalse, User::Invariant());

    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_RUNL_EXIT, "CUsbPNClassController::RunL - return void" );
    C_TRACE( ( _T("CUsbPNClassController::RunL - return void" ) ) );
    }

// ---------------------------------------------------------
// CUsbPNClassController::DoCancel
// ---------------------------------------------------------
void CUsbPNClassController::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_DOCANCEL_ENTRY, "CUsbPNClassController::DoCancel" );
    C_TRACE( ( _T("CUsbPNClassController::DoCancel()" ) ) );

    __ASSERT_DEBUG(EFalse, User::Invariant());

    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_DOCANCEL_EXIT, "CUsbPNClassController::DoCancel - return void" );
    C_TRACE( ( _T("CUsbPNClassController::DoCancel - return void" ) ) );
    }

// ---------------------------------------------------------
// CUsbPNClassController::RunError
// ---------------------------------------------------------
TInt CUsbPNClassController::RunError(TInt /*aError*/)
    {
    OstTrace0( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_RUNERROR_ENTRY, "CUsbPNClassController::RunError" );
    C_TRACE( ( _T("CUsbPNClassController::RunError()" )) );

    __ASSERT_DEBUG(EFalse, User::Invariant());

    OstTrace1( TRACE_NORMAL, CUSBPNCLASSCONTROLLER_RUNERROR_EXIT, "CUsbPNClassController::RunError - return 0x%x", KErrNone );
    C_TRACE( ( _T("CUsbPNClassController::RunError - return 0x%x" ), KErrNone ) );
    return KErrNone;
    }

