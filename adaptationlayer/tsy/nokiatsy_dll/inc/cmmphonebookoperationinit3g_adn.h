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

#ifndef _CMMPHONEBOOK_OPERATION_INIT_3G_ADN_H
#define _CMMPHONEBOOK_OPERATION_INIT_3G_ADN_H

// INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>
#include "cmmphonebookoperationinit.h"
#include "muiccoperationbase.h"



// CONSTANTS
    // Nonea

// MACROS
    // None

// DATA TYPES
    // None

// EXTERNAL DATA STRUCTURES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATION
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
class CAlphaString;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS


// CLASS DECLARATION
/**
* CMmPhoneBookOperationInit3G_ADN is used to create and send USIM-specific
* ADN Phone book Init.
*/

class CMmPhoneBookOperationInit3G_adn : public CMmPhoneBookOperationInit
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler:
        *        pointer to CMmPhoneBookStoreMessHandler class
        * @param const CMmDataPackage* aDataPackage: data
        * @return CMmPhoneBookOperationInit*: created object
        */
        static CMmPhoneBookOperationInit3G_adn* NewL(    
                CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
                CMmUiccMessHandler* aUiccMessHandler,
                const CMmDataPackage* aDataPackage // Data
                );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationInit3G_adn();


    protected:
        
        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationInit3G_adn();


    private:


        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        TInt UICCCreateReq(TInt aIpc, const CMmDataPackage* aDataPackage);
        TInt UICCInitializeReq3GADN();

        TInt CreateReqFetchTypeFile(
                TUiccReadLinearFixed &aparams,
                const TUint8 aFileType);
        
        TInt HandleUICCPbRespL(TBool &aComplete,TInt aStatus,const TDesC8 &aFileData,TInt aTransId);
        TInt HandleUICCPbResp3GADN(const TDesC8 &aFileData, TInt aStatus);

        TInt FetchType1FileFromPBR(const TDesC8 &aFileData);
        TInt FetchType2FileFromPBR(const TDesC8 &aFileData);
        TInt FetchType3FileFromPBR(const TDesC8 &aFileData);

        
        void HandleType1FileResp(const TDesC8& aFileData, TInt aStatus);
        void HandleType2FileResp(const TDesC8& aFileData, TInt aStatus);
        void HandleType3FileResp(const TDesC8& aFileData, TInt aStatus);
        
        TInt CheckForFileId();
        TInt CheckForNoOfRecords(TInt aNoOfRecords);
        TInt CheckForSameType1AndType2Files();
        TInt CheckRecordSizeForFixedLength(TInt aFileType, TInt aFileId, TInt aRecordLength);

        

    private: // Data

        
        // keep Track for Curre6gnt Array data to be accessed
        TUint8 iCurrentArrayCounter;

        // List of Type1 Pbr Files
        RArray <TPrimitiveTag> iType1FileArray;
        
        // List of Type2 Pbr Files
        RArray <TPrimitiveTag> iType2FileArray;
        
        // List of Type3 Pbr Files
        RArray <TPrimitiveTag> iType3FileArray;

        // no of ANR files type1 per ADN phonebook
        TInt iType1ANRFiles;
        
        // no of ANR files type2 Per ADN phonebook
        TInt iType2ANRFiles;
        
        TInt iType1EmailFiles;
        
        TInt iType2EmailFiles;
        
        TInt iType1SNEFiles;
        TInt iType2SNEFiles;
        
    };

#endif // _CMMPHONEBOOK_OPERATION_INIT_3G_ADN_H

// End of File

