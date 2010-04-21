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

        /**
        * Create request to Start initialization for Phonebook
        * @param aIpc
        * @param aDataPackage
        * @param aTransId
        * @return TInt: KErrNone or error value.
        */
        TInt UICCCreateReq(TInt aIpc, const CMmDataPackage* aDataPackage, TUint8 aTransId);

        /**
        * Create request to Start initialization for 3G ADN Phonebook
        * @param aTransId
        * @return TInt: KErrNone or error value.
        */

        TInt UICCInitializeReq3GADN( TUint8 aTransId);

        /**
        * Create request to Start initialization for Type1 , Type2 and Type3 Files in PBR
        * @param aparams
        * @param aFiletype
        * @return TInt: KErrNone or error value.
        */
        TInt CreateReqFetchTypeFile(
                TUiccReadLinearFixed &aparams,
                const TUint8 aFileType);
        

        /**
        * Handles response for PhoneBook Init 
        * @param aFileData
        * @param TInt aTraId
        * @param TInt aStatus
        * @param TDesC8& aFileData
        * @return TInt: KErrNone or error value.
        */

        TBool HandleUICCPbRespL( TInt aStatus,
                TUint8 aDetails,
                const TDesC8 &aFileData,
                TInt aTransId );

        /**
        * Handles response for 3GADN PhoneBook Init 
        * @param aFileData
        * @param TInt aTraId
        * @param TInt aStatus
        * @param TDesC8& aFileData
        * @return TInt: KErrNone or error value.
        */

        TInt HandleUICCPbResp3GADN(const TDesC8 &aFileData, TInt aStatus, TUint8 aTransId );
        
        /**
        * Handles FileData of Type 1 file info for ADN 3G initialization
        * @param TDesC8& aFileData
        * @param TInt aStatus
        * @return TInt: KErrNone or error value.
        */

        void HandleType1FileResp(const TDesC8& aFileData, TInt aStatus);

        /**
        * Handles FileData of Type 2 file info for ADN 3G initialization
        * @param TDesC8& aFileData
        * @param TInt aStatus
        * @return TInt: KErrNone or error value.
        */

        void HandleType2FileResp(const TDesC8& aFileData, TInt aStatus);
        
        /**
        * Handles FileData of Type 3 file info for ADN 3G initialization
        * @param TDesC8& aFileData
        * @param TInt aStatus
        * @return TInt: KErrNone or error value.
        */

        void HandleType3FileResp(const TDesC8& aFileData, TInt aStatus);
        
        /**
        * Check for 2 Files have same File id in PBR TLV Data
        * @return TInt: KErrNone or error value.
        */
        TInt CheckForFileId();

        /**
        * Check for no of recors for Type 1 File SHould be same as EF_ADN file
        * @param aNoOfRecords
        * @return TInt: KErrNone or error value.
        */

        TInt CheckForNoOfRecords(TInt aNoOfRecords);

        /**
        * Check same File id fot Type and Type 2 Files
        * @return TInt: KErrNone or error value.
        */

        TInt CheckForSameType1AndType2Files();

        /**
        * Check for fixed recordlegth EF have correct Recordlength
        * @param aNoOfRecords
        * @return TInt: KErrNone or error value.
        */

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

