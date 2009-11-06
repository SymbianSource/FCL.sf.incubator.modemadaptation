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



// ---------------------------------------------------------
// CDpDataPort::CreateTransactionId
// This method creates a transaction ID for pipe message.
// Transaction ID is previous transaction ID + 1.
// When transaction ID gets over 255, it turns to 0 again.
// ---------------------------------------------------------
//
inline TUint8 CDpDataPort::CreateTransactionId()
    {
    return TUint8( iTransactionId++ % 256 );
    }

// ---------------------------------------------------------
// CDpDataPort::pMif
// This method returns pointer to message interface class.
// ---------------------------------------------------------
//
inline CDpMif* CDpDataPort::pMif()
    {
    return iMif;
    }

// ---------------------------------------------------------
// CDpDataPort::pPif
// This method returns pointer to pipe interface class.
// ---------------------------------------------------------
//
inline CDpPif* CDpDataPort::pPif()
    {
    return iPifDcs;
    }

 // ---------------------------------------------------------
// CDpDataPort::PortUnit
// This method returns port unit value.
// ---------------------------------------------------------
//
inline TUint CDpDataPort::PortUnit()
    {
    return iUnit;
    }

// End of File
