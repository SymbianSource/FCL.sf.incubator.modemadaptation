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

#ifndef CMMPHONEBOOKOPERATIONWRITE_H
#define CMMPHONEBOOKOPERATIONWRITE_H

//  INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>

#include "cmmphonebookstoreoperationbase.h"

//  CONSTANTS
    //none

//  MACROS
    //none

//  DATA TYPES
    //none

//  EXTERNAL DATA STRUCTURES

//  FUNCTION PROTOTYPES
    //none

// CLASS DECLARATION
/**
* CMmPhoneBookOperationWrite is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender relating to write.
*/
class CMmPhoneBookOperationWrite
    : public CMmPhoneBookStoreOperationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmPhoneBookOperationWrite* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            const CMmDataPackage* aDataPackage );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationWrite();

        /**
        * This method created entry point to correct operation
        *
        * @param aPhonebookType PhoneBook type
        * @param aIpc IPC
        * @return Pointer to operation.
        */
        static CMmPhoneBookOperationWrite* Build(
            TName aPhonebookType,
            TInt aIpc );

  private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationWrite();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        /**
        * Separates different IPC requests for each other.
        *
        * @param aIpc Identify number of request.
        * @param aDataPackage Packaged data.
        * @return KErrNone or error value.
        */
        TInt CreateReq(
            TInt aIpc,
            const CMmDataPackage* aDataPackage );

        /**
        * Handles SimPbResp ISI -message
        *
        * @param aIsiMessage
        * @param aComplete Indicates if request can remove from
        *        operationlist or not.
        * @return KErrNone or error value.
        */
        TInt HandleSimPbRespL(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Handles HandleWriteResp ISI -message
        *
        * @param aIsiMessage
        * @param aComplete Indicates if request can remove from
        *        operationlist or not.
        * @return KErrNone or error value.
        */
        TInt HandleWriteResp(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Handling write response at delete phase
        *
        * @param aIsiMessage
        * @param aComplete Indicates if request can remove from
        *        operationlist or not.
        * @return KErrNone or error value.
        */
        TInt HandleWriteDeleteL(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Creates and sends ISI message in order to wite an entry to SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @param aDataToWrite Entry to write
        * @return KErrNone or error value
        */
        TInt SimPbReqWriteL(
            TUint8 aTraId,
            TInt16 aIndex,
            CPhoneBookStoreEntry& aDataToWrite );

        /**
        * Creates and sends ISI message in order to wite an entry to SIM
        *
        * @param aTraId Transaction id
        * @param aIndex Location index
        * @return KErrNone or error value
        */
        TInt SimPbReqWriteDelete(
            TUint8 aTraId,
            const TInt16 aIndex );

        /**
        * Compose 3G data if available
        *
        * @param aDataToWrite Entry
        * @param aDataPtr Reference to data
        * @param aErrVal error value
        * @return KErrNone or error value
        */
        TInt Compose3GDataL(
            CPhoneBookStoreEntry& aDataToWrite,
            TDes8& aDataPtr
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            ,TInt& aErrVal
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
             );

        /**
        * Count sublock length
        *
        * @param aDataToWrite Entry to write
        * @param aNumOfSubblocks Number of subblocks
        * @param aMessageLength MessageLength
        * @return None
        */
        void CountSublocksAndMessLength(
            CPhoneBookStoreEntry& aDataToWrite,
            TUint8& aNumOfSubblocks,
            TInt& aMessageLength );

        /**
        * Create SIM_DATA_SIZE_READ_REQ message.
        *
        * @param aTraId Transaction Id
        * @param aComplete
        * @return KErrNone or error value.
        */
        TInt BuildSimDataSizeReadReqData(
            TUint8 aTraId,
            TBool& aComplete );

        /**
        * Handling error situation
        *
        * @param aIsiMessage ISI message
        * @param aStatus Status from ISI message
        * @param aSbStartOffSet Offset to data
        * @return KErrNone or error value.
        */
        TInt ErrorStatusHandling(
            const TIsiReceiveC& aIsiMessage,
            TUint8 aStatus,
            TUint& aSbStartOffSet );

        /**
        * Add tSIM_SB_NAME_NUMBER
        *
        * @param aSubBlockId Subblock Id
        * @param aLocation Location
        * @param aName Reference to data name
        * @param aNumber Reference to data number
        * @param aDataToAppend Reference to data
        * @return None
        */
        void AddSimSbNameNumberSubBlock(
            TUint16& aSubBlockId,
            TInt16& aLocation,
            TDesC& aName,
            TDesC& aNumber,
            TDes8& aDataToAppend );

        /**
        * Add tSIM_SB_TEXT_NUMBER
        *
        * @param aSubBlockId Reference to subblock data
        * @param aLocation Location
        * @param aText Reference to text data
        * @param aDataToAppend Reference to data
        * @return None
        */
        void AddSimSbTextSubBlock(
            const TUint16& aSubBlockId,
            TInt16& aLocation,
            TDesC& aText,
            TDes8& aDataToAppend );

        /**
        * Add tSIM_SB_NUMBER_NUMBER
        *
        * @param aLocation Location
        * @param aNumber Reference to number
        * @param aAasRecordNumber AAS number
        * @param aDataToAppend Reference to data
        * @return None
        */
        void AddSimSbNumberSubBlock( const TUint16& aSubBlockId,
            TInt16& aLocation,
            TDesC& aNumber,

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            TUint8 aAasRecordNumber,
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
            TDes8& aDataToAppend );
#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        /**
        * Add tSIM_SB_GROUP_NUMBER
        *
        * @param aSubBlockId
        * @param aGrpLocation
        * @param aGasLocations
        * @param aDataToAppend:
        * @return None
        */
        void AddSimSbGroupSubBlock( const TUint16& aSubBlockId,
            TInt16& aGrpLocation,
            CArrayFixFlat<TInt>* aGasLocations,
            TDes8& aDataToAppend );

        /**
        * Creates and sends ISI message in order to write alphastring to SIM.
        *
        * @param None
        * @return KErrNone or error value
        */
        TInt SimWriteAlphaStringReq();

        /**
        * Receives SIM_PB_RESP(SIM_PB_WRITE) ISI messages from phonet receiver
        *
        * @param aIsiMessage Reference to the received message.
        * @return KErrNone or error code
        */
        TInt SimWriteAlphaStringRespL( const TIsiReceiveC& aIsiMessage );
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS


    public:     // Data
        // None

    protected:  // Data
        // none

    private:    // Data

        // Attribute to hold the write enty while delete -request is on
        CPhoneBookStoreEntry* iPhoneBookEntry;

        // Keep track on current write -phase
        TPBWritePhases iCurrentWritePhase;
};

#endif // CMMPHONEBOOKOPERATIONWRITE_H

// End of file
