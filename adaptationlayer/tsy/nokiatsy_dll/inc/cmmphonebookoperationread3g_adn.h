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

#ifndef _CMMPHONEBOOK_OPERATION_READ_3G_ADN_H
#define _CMMPHONEBOOK_OPERATION_READ_3G_ADN_H

// INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>
#include "cmmphonebookoperationread.h"

// CONSTANTS
    // None

// MACROS
    // None

// DATA TYPES
    // None

// EXTERNAL DATA STRUCTURES
    // None

// FUNCTION PROTOTYPES
    // None

// CLASS DECLARATION
/**
* CMmPhoneBookOperationInit3G_ADN is used to create and send USIM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to read.
*/
class CMmPhoneBookOperationRead3g_adn : public CMmPhoneBookOperationRead
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return CMmPhoneBookOperationRead3g_adn*: created object
        */
        static CMmPhoneBookOperationRead3g_adn* NewL();

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationRead3g_adn();

    protected:
        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationRead3g_adn();


  private:

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();
        
        // Transmit 

        /**
        * Constructs Data to read entry from USIM ADN Phonebook
        * @param
        * @return TInt: KErrNone or error value.
        */
        TInt UICCHandleData3gADNReadReq(TInt aFileId, TInt aFileSFI);
        
        
        // Receive

        
        /**
        * Handles SimPbResp ISI -message
        * @param  TInt aTagValue
        * @param  TDes8& aFileData
        * @return TInt: KErrNone or error value.
        */
        TInt HandleUICC3gADNRespL(const TInt aStatus, const TDes8& aFileData, const TInt aTransId);
        
    public: // Data
        // None

    protected: // Data
        // None

    private: // Data
        // None


        // List of Pbr Files records
        RArray <TPrimitiveTag> iPBRFileRecordArray;


};

#endif // _CMMPHONEBOOK_OPERATION_READ_H

// End of file
