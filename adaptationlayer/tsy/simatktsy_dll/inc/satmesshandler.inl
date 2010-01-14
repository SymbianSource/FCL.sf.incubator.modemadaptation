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



#ifndef SATMESSHANDLER_INL
#define SATMESSHANDLER_INL

//  INCLUDES
#include "satmesshandler.h"
#include <net_modemisi.h> // net server


// -----------------------------------------------------------------------------
// CSatMessHandler::TimeZone
// Returns time zone value received from Network.
// -----------------------------------------------------------------------------
//
inline TUint8 CSatMessHandler::TimeZone() const
    {
    return iTimeZone;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::ImeiAvailable
// Return ETrue if Imei has been successfullly read, EFalse if not.
// -----------------------------------------------------------------------------
//
inline TBool CSatMessHandler::ImeiAvailable() const
    {
    return iImeiAvailable;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::ImeiSvAvailable
// Return ETrue if ImeiSv has been successfullly read, EFalse if not.
// -----------------------------------------------------------------------------
//
inline TBool CSatMessHandler::ImeiSvAvailable() const
    {
    return iImeiSvAvailable;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::ServiceAvailable
// Returns ETrue if ME is in Service, otherwise EFalse
// -----------------------------------------------------------------------------
//
inline TBool CSatMessHandler::ServiceAvailable() const
    {
    return iLocInfo.iRegStatus == NET_SERVICE ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::GetPollingOffStatus
// Returns polling off status: true after a Polling Off,
// and false after a polling interval pcmd
// -----------------------------------------------------------------------------
//
inline TBool CSatMessHandler::PollingOff() const
    {
    return iPollingOff;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SetPollingOffStatus
// Sets polling off status: true after a Polling Off,
// and false after a polling interval pcmd
// -----------------------------------------------------------------------------
//
inline void CSatMessHandler::SetPollingOff
        (
        const TBool aPollingOff
        )
    {
    iPollingOff = aPollingOff;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::SsStatus
// Returns status indicating if Ss or Ussd transaction ongoing
// -----------------------------------------------------------------------------
//
inline CSatMessHandler::TSsStatus CSatMessHandler::SsStatus() const
    {
    return iSsStatus;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::CurrentAccessTechnology
// Returns currently used access technology type
// -----------------------------------------------------------------------------
//
inline TUint8 CSatMessHandler::CurrentAccessTechnology() const
    {
    return iCurrentAccTech;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::OldPollInterval
// Returns old polling intervqal status: true if Old Polling is set on,
// and false otherwise
// -----------------------------------------------------------------------------
//
inline TBool CSatMessHandler::OldPollInterval() const
    {
    return iOldPollIntervalSupportedInPp;
    }

// -----------------------------------------------------------------------------
// CSatMessHandler::EnsSupported
// Returns current status of INFO_PP_ENHANCED_NETWORK_SELECTION PP flag
// -----------------------------------------------------------------------------
//
#ifdef INFO_PP_ENHANCED_NETWORK_SELECTION
inline TBool CSatMessHandler::EnsSupported() const
    {
    return iEnsSupportedInPp;
    }
#endif // INFO_PP_ENHANCED_NETWORK_SELECTION

#endif    // SATMESSHANDLER_INL

// End of File
