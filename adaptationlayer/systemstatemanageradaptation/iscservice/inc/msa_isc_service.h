/*
* ==============================================================================
*  Name:           msa_isc_service.h
*  Part of:        System adaptation
*  Description:    Declaration of MSAIscService class. MSAIscService class
*                  defines an interface to System Adaptation plug-in clients.
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:59:13 2009 %
*
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
* Declaration of MSAIscService class
*
*/


#ifndef MSA_ISC_SERVICE_H
#define MSA_ISC_SERVICE_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  MSAIscService
*  Interface class to System adaptation Common Isc service classes
*
*/
class MSAIscService
    {

    public: // New functions
        
        /**
        * Indicates System adaptation client about received isc message
        * @param aData              Reference to received data
        * @return none
        */
        virtual void ReceiveIsiMessageL( TDesC8& aData ) = 0;

        /**
        * Indicates System adaptation client about isc channel status change
        * @param aErrorCode         Isc channel error code
        * @return none
        */
        virtual void ProcessConnectionClosedL( TInt aErrorCode ) = 0;
    };

#endif      // MSA_ISC_SERVICE_H

// End of File
