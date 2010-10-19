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

#ifndef _CMMPHONEBOOK_OPERATION_INIT_H
#define _CMMPHONEBOOK_OPERATION_INIT_H

// INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>
#include "cmmphonebookstoreoperationbase.h"
#include "muiccoperationbase.h"



// CONSTANTS
    // Nonea

// MACROS
    // None

// DATA TYPES
    // None

// EXTERNAL DATA STRUCTURES
    enum TPBIniPhases
    {
    EPBInitPhaseADN = 0,
    EPBInitPhaseFDN,
    EPBInitPhaseSDN,
    EPBInitPhaseMBDN,
    EPBInitPhaseMSISDN,
    EPBInitPhaseVMBX,
    EPBInitPhase_File_Info,
    EPBInitPhase_FIle_Data,
    EPBInitPhase_Complete,
    EPBInitPhase_3GADN_PBR,
    EPBInitPhase_3GADN_Type1,
    EPBInitPhase_3GADN_Type2,
    EPBInitPhase_3GADN_Type3,
    EPBIniPhase_3GADNDone,
    EPBIniPhase_PBInitialized,
    EPBIniPhase_Unknown,
    EPBIniPhase_Internal
    };

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATION
class CMmPhoneBookOperationInit3G_adn;


// CLASS DECLARATION
/**
* CMmPhoneBookOperationInit is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to init.
*/

class CMmPhoneBookOperationInit : public CMmPhoneBookStoreOperationBase
    {
    public:  // Constructors and destructor
        

        /**
        * Two-phased constructor.
        * @param CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler:
        *        pointer to CMmPhoneBookStoreMessHandler class
        * @param const CMmDataPackage* aDataPackage: data
        * @return CMmPhoneBookOperationInit*: created object
        */
        static CMmPhoneBookOperationInit* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aUiccMessHandler,
            const CMmDataPackage* aDataPackage);

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationInit();
        

        /**
        * Get next Available phonebook in SST table to initalize
        * @param Tint aPbook
        * @return TInt: Phonebook value.
        */
        TUint8 GetNextAvailablePbIcc(TUint8 aPbook);

        /**
        * Get next Available phonebook in UST table to initalize
        * @param Tint aPbook
        * @return TInt: Phonebook value.
        */
        TUint8 GetNextAvailablePbUicc(TUint8 aPbook);

        /**
        * Creates phonebook initialize request for UICC card Type
        * @return TInt: KErrNone or error value.
        */
        TInt UICCInitializeReq(TUint8 aTransId);
        
        /**
        * Get the next phonebook init Phase
        * @param current init phase 
        * @return None
        */
        void GetNextPhoneBookInitPhase(TUint8 & aInitPhase);


        
        

    protected:
        
        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationInit();
        
        /**
        * Handle init response for phase 3
        * @param const TDes8& aFileData: UICC Message
        * @return TInt: KErrNone or error value.
        */
        TInt HandlePBRespL(
            const TDesC8& aFileData, TInt aStatus, TUint8 aTransId );




    private:

        
        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL( );

        TInt UICCCreateReq( TInt aIpc, const CMmDataPackage* aDataPackage, TUint8 aTransId );

        

        /**
        * Creates phonebook initialize request
        * @return TInt: KErrNone or error value.
        */
        TInt CreateDataToFetchFile(TUiccReadLinearFixed &aParams,const TUint8 aServiceType, TUint8 aRecordNo);


        
// RECEIVE        
        
        
        /**
        * Handles aFileData
        * @param TInt aTraId
        * @param TInt aStatus
        * @param TDesC8& aFileData
        * @return TInt: KErrNone or error value.
        */
        TBool HandleUICCPbRespL(
            TInt aStatus,
            TUint8 aDetails,
            const TDesC8 &aFileData,
            TInt aTransId);



        /**
        * Handle FileData of FDN File Response
        * @param const TDes8& aFileData: UICC Message
        * @return TInt: KErrNone or error value.
        */
        TInt HandleFileResp(const TDesC8 &aFileData, TInt aStatus);        

        /**
        * Handle FileData of FDN File Response
        * @param const TDes8& aFileData: UICC Message
        * @return TInt: KErrNone or error value.
        */
        TInt HandleEXTFileResp(const TDesC8 &aFileData, TInt aStatus);        

        /**
        * Handle FileData of MBI File Response
        * @param const TDes8& aFileData: UICC Message
        * @return TInt: KErrNone or error value.
        */
        void HandleMBIFileResp( const TDesC8 &aFileData , TInt aStatus );

        
        
    public: // Data
        // None



    protected: // Data
        // Attribute to hold the information about storage
        CStorageInfoData *iPBStoreInfoData;

        // Keep track on current ini -phase

        TUint8 iIniPhase;
        // Keep track about Service type 
        TUint8 iServiceType;
        // Storage info
        TInt iNumOfPBRRecords;
        // Indicates is internal init ongoing or not.
        TBool iInternalInit;
        // Attribute to check what kind of read is ongoing
        TTypeOfFileToBeRead iTypeOfReading ;
        // Attribute to Store MBI recordlength
        TUint8 iMbiRecLen;

        
        // Check for ADN is Initilized or not 
        
        TBool iADNPbInitilized;

    private: // Data

        
        
        // Indicate the Card type
        TUint8 iCardType;
        
    };

#endif // _CMMPHONEBOOK_OPERATION_INIT_H

// End of File

