/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
            const CMmDataPackage* aDataPackage,
            TInt aIpc );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationRead();

        /**
        * Sends Request To Get Read and Write Size
        * for Msisdn PB
        * @param TUint8 aTransId : TransactionId
        * @return TInt: KErrNone or error value.
        */
        TInt USimReadWriteSizeReq( TUint8 aTransId );

        /**
        * Construct UICC Req to read Entry
        * @param
        * @return TInt: KErrNone or error value.
        */
        TInt USimPbReqRead( TInt aRecordNo, TUint8 aTransId );

        /**
        * Appends Correct record numner to request
        * @param aParams
        * @return TInt: KErrNone or error value.
        */
        TInt AddParamToReadReq( TUiccReadLinearFixed& aParams );

        /**
        * Handles SimPbResp ISI -message
        * @param  aStatus
        * @param aTransId
        * @param aFileData
        * @param pbFileId
        * @param fileIdExt
        * @parma arrayIndex
        * @return TInt: KErrNone or error value.
        */
        TBool  USimPbReadRespL( TInt aStatus, 
                TUint8 aTransId,
                const TDesC8 &aFileData );

        /**
        * Handles response for Msisdn read Write size request
        * @param  TDesC8 &aFileData :  Response Data
        * @return TInt: KErrNone or error value.
        */
        TBool USimReadWriteSizeResp( const TDesC8 &aFileData, 
                                    TInt aStatus );

  private:

      /**
      * Separates different IPC requests for each other.
      * @param TInt aIpc: Identify number of request.
      * @param const CMmDataPackage* aDataPackage: Packaged data.
      * @return TInt: KErrNone or error value.
      */
      TInt UICCCreateReq( TInt aIpc, const CMmDataPackage* aDataPackage, TUint8 aTransId );


      /**
      * Handles read resp for main Entry
      * @param  aFileData
      * @param aTransId
      * @param aEmptyEntry  : Entry is EMpty or not
      * @param aEntryStore : ENtry is store in internal list or not
      * @return TInt: KErrNone or error value.
      */
      void HandleReadResp( 
              const TDesC8 &aFileData, 
              TUint8 aTransId, 
              TInt &aEmptyEntry, 
              TBool &aEntryStore );

      /**
      * Handles read resp for main Entry when Entry is present
      * @param  aFileData
      * @param aTransId
      * @param aEntryStore : ENtry is store in internal list or not
      * @return TInt: KErrNone or error value.
      */
      void HandleEntryPresentResp( 
              const TDesC8 &aFileData, 
              TUint8 aTransId, 
              TBool &aEntryStore );

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
      * Handles EXT file read resp
      * @param aFIleData 
      * @param aTransId
      * @return TInt: KErrNone or error value.
      */
      void HandleExtReadResp( const TDesC8 &aFileData, 
                              TUint8 aTransId,
                              TBool &aEntryStored );


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
        TInt UICCHandleDataADNReadReq( TUiccReadLinearFixed& aParams, TUint16 aFileID, TUint8 aFileSFI );


        // Receive


        /**
        * Handles SimPbResp ISI -message
        * @param  TInt aTagValue
        * @param  TDes8& aFileData
        * @return TInt: KErrNone or error value.
        */
        TInt UICCHandleDataADNReadResp( const TDesC8& aFileData);

        /**
        * Store Entry to internal List and CacheArray
        * @param  TBool &aEntryStored
        */
        void StoreEntryToListL( TBool &aEntryStored );

        /**
        * Store Own Number Entry to internal and compelte
        * @param  TInt aRet
        */
        void StoreAndCompleteOwnNumber( TInt aRet, TInt aEmptyEntry );

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

        // Attribute to Store Entry
        TPBEntry* iStoreEntry;

        // Saved IPC for complete
        TInt iSavedIPCForComplete;
        
        // Attribute store fileid
        TUint16 iFileId;
        
        // Attribute to store Ext File 
        TUint16 iExtFileId;
        
        // Attribute to store Phonebook conf array index
        TUint8 iArrayIndex;

    private: // Data

        // Attribute to check what kind of read is ongoing
        TTypeOfFileToBeRead iTypeOfReading ;

        TBool iExtensionRead ;
        
        



};

#endif // _CMMPHONEBOOK_OPERATION_READ_H

// End of file
