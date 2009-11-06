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



#ifndef MUSBPNRECEIVEOBSERVER_H
#define MUSBPNRECEIVEOBSERVER_H

// INCLUDES
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

class MUsbPnBufferListener
    {
    public: // New functions

        /**
        * Continue receive if buffer queue has room again.
        * @since Series ?XX ?SeriesXX_version
        * @param TBool aDiscard
        * @return ?description
        */
        virtual void Receive( TBool aDiscard ) = 0;

    };

#endif      // MUSBPNRECEIVEOBSERVER_H

// End of File
