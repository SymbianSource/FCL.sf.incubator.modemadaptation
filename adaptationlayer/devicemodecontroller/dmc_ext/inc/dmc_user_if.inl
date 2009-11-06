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
*     Inline implementation of class DMC.
*
*/


// INCLUDE FILES
// EXTERNAL DATA STRUCTURES 
// EXTERNAL FUNCTION PROTOTYPES 
// FORWARD DECLARATIONS
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES  
// LOCAL FUNCTION PROTOTYPES  


// ==================== LOCAL FUNCTIONS ====================
// ================= OTHER EXPORTED FUNCTIONS ==============

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RDmc::Open
// -----------------------------------------------------------------------------
inline TInt RDmc::Open()
    {
#ifdef __WINS__

    TInt ret(KErrNotSupported );

#else // __WINS__

    TInt err = User::LoadLogicalDevice(KDmcDrvName);

    DMC_USER_TRACE((_L("DMC:R_USER_API: Open() - User::LoadLogicalDevice, err: %d"), err));

    DMC_USER_TRACE_ASSERT_RESET((err != KErrNone  || err != KErrAlreadyExists),
                                 _L("DMC: R_USER_API: Open() - LoadLogicalDevice"),
                                 err);

    TInt ret = DoCreate(KDmcDrvName,
                        (TVersion(RDmc::EMajorVersionNumber,
                                  RDmc::EMinorVersionNumber,
                                  RDmc::EBuildVersionNumber)),
                        KNullUnit,
                        NULL,
                        NULL);

    DMC_USER_TRACE((_L("DMC:R_USER_API: Open() - DoCreate, ret: %d"), ret));
#endif // __WINS__

    DMC_USER_TRACE((_L("DMC:R_USER_API: Open() - ret: %d"), ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// RDmc::Close
// -----------------------------------------------------------------------------
inline void RDmc::Close()
    {
#ifdef __WINS__

#else // __WINS__
    RHandleBase::Close();
    DMC_USER_TRACE((_L("DMC:R_USER_API: Close() - RHandleBase::Close")));

    TInt err = User::FreeLogicalDevice(KDmcDrvName);

    DMC_USER_TRACE((_L("DMC:R_USER_API: Close() - User::FreeLogicalDevice, err: %d"), err));
    DMC_USER_TRACE_ASSERT_RESET((err != KErrNone  || err != KErrInUse),
                                _L("DMC: R_USER_API: Close() - FreeLogicalDevice"),
                                err);
#endif // __WINS__
    }


// -----------------------------------------------------------------------------
// RDmc::GetStartUpMode
// -----------------------------------------------------------------------------
inline TInt RDmc::GetStartUpMode(TTargetStartupMode& aTargetStartUpMode)
    {
#ifdef __WINS__

    TInt ret(KErrNotSupported);

#else // __WINS__

    TInt ret = DoControl(RDmc::EGetTargetStartUpMode, &aTargetStartUpMode);

#endif // __WINS__

    DMC_USER_TRACE((_L("DMC:R_USER_API: GetStartUpMode() - ret: %d, aTargetStartUpMode: %d"), 
                        ret, aTargetStartUpMode));

    return ret;
    }


// -----------------------------------------------------------------------------
// RDmc::PowerOff
// -----------------------------------------------------------------------------
inline TInt RDmc::PowerOff()
    {
#ifdef __WINS__

    TInt ret(KErrNotSupported);

#else // __WINS__

    TInt ret = DoControl(EPowerOff);

#endif // __WINS__

    DMC_USER_TRACE((_L("DMC:R_USER_API: PowerOff() - ret: %d"), ret));

    return ret;
    }


// -----------------------------------------------------------------------------
// RDmc::GenerateReset
// -----------------------------------------------------------------------------
inline TInt RDmc::GenerateReset(TTargetStartupMode& aTargetStartUpMode)
    {
#ifdef __WINS__

    TInt ret(KErrNotSupported);

#else // __WINS__

    TInt ret = DoControl(RDmc::EGenerateReset, &aTargetStartUpMode);

#endif // __WINS__

    DMC_USER_TRACE((_L("DMC:R_USER_API: GenerateReset() - ret: %d, aTargetStartUpMode: %d"), 
                        ret, aTargetStartUpMode));

    return ret;
    }


// -----------------------------------------------------------------------------
// RDmc::IsHiddenReset
// -----------------------------------------------------------------------------
inline TInt RDmc::IsHiddenReset(TBool& aHiddenStatus)
    {
#ifdef __WINS__

    TInt ret(KErrNotSupported);

#else // __WINS__

    TInt ret = DoControl(RDmc::EIsHiddenReset, &aHiddenStatus);

#endif // __WINS__

    DMC_USER_TRACE((_L("DMC:R_USER_API: GenerateReset() - ret: %d, aTargetStartUpMode: %d"), 
                        ret, aHiddenStatus));

    return ret;
    }


// -----------------------------------------------------------------------------
// RDmc::ReceiveEvents
// -----------------------------------------------------------------------------
inline void RDmc::ReceiveEvents(TRequestStatus& aStatus, TEvent& aEvent)
    {
#ifdef __WINS__


#else // __WINS__

    aStatus = KRequestPending;
    
    DMC_USER_TRACE((_L("DMC:R_USER_API: ReceiveEvents() - aStatus.Int(): 0x%x"), aStatus.Int()));
    
    DoRequest(RDmc::EReceiveEvents, aStatus, &aEvent);

#endif // __WINS__

    DMC_USER_TRACE((_L("DMC:R_USER_API: ReceiveEvents() - aEvent: %d"), aEvent));
    }

//  End of File
