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



/** @trxdefs.h
@brief Unique identifiers of transceivers. Part of IST API.
@publishedDeviceAbstraction
*/

#ifndef __TRXDEFS_H__
#define __TRXDEFS_H__

// Enumerations

/**
Unique identifiers for each transceiver.
*/
enum TTrxId
    {
    ETrxHSI             = 0x00,
    ETrxSharedMemory,
    ETrxPartnerOS,
    ETrxTest,           // Only for testing purposes
    ETrxTest2,          // Only for testing purposes
    EAmountOfTrxs,      // Not used by transceivers
    };

#endif /* __TRXDEFS_H__ */
