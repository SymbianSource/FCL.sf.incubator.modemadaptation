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

#ifndef _CMMPHONEBOOK_OPERATION_READ_H
#define _CMMPHONEBOOK_OPERATION_READ_H

// INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>
#include "cmmphonebookstoreoperationbase.h"

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
* CMmPhoneBookOperationInit is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to read.
*/
class CMmPhoneBookOperationRead : public CMmPhoneBookStoreOperationBase
    {
    public:  // Constructors and destructor
        
        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationRead();


        /**
        * Two-phased constructor.
        * @return CMmPhoneBookOperationRead*: created object
        */
        static CMmPhoneBookOperationRead* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aUiccMessHandler,
            const CMmDataPackage* aDataPackage );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationRead();

        /**
        * This method created entry point to correct operation
        * @param TName aPhonebookType:
        * @param TInt aIpc: Identify number of request.
        * @return CMmPhoneBookStoreOperation*: pointer to operation.
        */
        static CMmPhoneBookOperationRead* Build(
            TName aPhonebookType,
            TInt aIpc );



virtual TInt UICCHandleData3gADNReadReq( TInt /*aFileID*/, TInt /*aFileSFI*/)
{
        TFLOGSTRING("TSY: CMmPhoneBookOperationInit::CreateReq - Return KErrNotSupported");
        return KErrNotSupported;
}


virtual TInt HandleUICC3gADNRespL(const TInt /*aStatus*/, const TDes8& /*aFileData*/, const TInt /*aTransId*/)
{
        TFLOGSTRING("TSY: CMmPhoneBookOperationInit::CreateReq - Return KErrNotSupported");
        return KErrNotSupported;
}
  private:


        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        // Transmit

        /**
        * Separates different IPC requests for each other.
        * @param TInt aIpc: Identify number of request.
        * @param const CMmDataPackage* aDataPackage: Packaged data.
        * @return TInt: KErrNone or error value.
        */
        TInt UICCCreateReq( TInt aIpc, const CMmDataPackage* aDataPackage, TUint8 aTransId );

        /**
        * Constructs an ISI-message to read entry from SIM
        * @param
        * @return TInt: KErrNone or error value.
        */
        TInt USimPbReqRead( TInt aRecordNo, TUint8 aTransId );

        /**
        * Constructs Data to read entry from USIM ADN Phonebook
        * @param
        * @return TInt: KErrNone or error value.
        */
        TInt UICCHandleDataADNReadReq( TUiccReadLinearFixed& aParams, TUint16 aFileID, TUint8 aFileSFI );


        // Receive

        /**
        * Handles SimPbResp ISI -message
        * @param  TIsiReceiveC& aIsiMessage
        * @param TBool& aComplete: Indicates if request can remove from
        *        operationlist or not.
        * @return TInt: KErrNone or error value.
        */
        TBool HandleUICCPbRespL(
                TInt aStatus,
                TUint8 aDetails,
                const TDesC8 &aFileData,
                TInt aTransId);



        /**
        * Handles SimPbResp ISI -message
        * @param  TInt aTagValue
        * @param  TDes8& aFileData
        * @return TInt: KErrNone or error value.
        */
        TInt UICCHandleDataADNReadResp( const TDesC8& aFileData);


    public: // Data
        // None

    protected: // Data
        // Attribute to hold the information how many entries left to read
        TInt iNumOfEntriesToRead;

        // Attribute to hold the information how many entries written into
        // array
        TInt iNumOfEntriesFilled;

        // To Store the information about first valid Entry Search
        TBool iLocationSearch;

    private: // Data

        // Attribute to check what kind of read is ongoing
        TTypeOfFileToBeRead iTypeOfReading ;

        TBool iExtensionRead ;
        
        // Attribute to Store Entry
        TPBEntry* iStoreEntry;
        
        // Saved IPC for complete
        TInt iSavedIPCForComplete;



};

#endif // _CMMPHONEBOOK_OPERATION_READ_H

// End of file
