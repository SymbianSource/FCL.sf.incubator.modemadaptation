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

#ifndef _CMMPHONEBOOK_OPERATION_CACHE_H
#define _CMMPHONEBOOK_OPERATION_CACHE_H


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

// FORWARD DECLARATION
    // None

// CLASS DECLARATION
/**
* CMmPhoneBookOperationCache is used to create and send GSM-specific PBStore
* ISI messages to PhoNet via PhoNetSender relating to cache.
*/
class CMmPhoneBookOperationCache : public CMmPhoneBookStoreOperationBase
    {
    public:

        /**
        * Two-phased constructor.
        * @param CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler:
        *   Pointer to CMmPhoneBookStoreMessHandler object.
        * @param aIpc IPC number
        * @param const CMmDataPackage* aDataPackage: Packaged data.
        * @return CMmPhoneBookOperationInit*: created object
        */
        static CMmPhoneBookOperationCache* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            TInt aIpc,
            const CMmDataPackage* aDataPackage );

        /**
        * Destructor.
        */
        ~CMmPhoneBookOperationCache();

        /**
        * Calculates transaction id from data package
        * @param aIpc IPC number
        * @param aDataPackage data package
        * @param aTransactionId resulting transaction id
        * @return TInt KErrNotSupported or KErrNone
        */
        static TInt CalculateTransactionId(
            TInt aIpc,
            const CMmDataPackage* aDataPackage,
            TUint8& aTransactionId
            );

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookOperationCache();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        /**
        * Separates different IPC requests for each other.
        * @param TInt aIpc: Identify number of request.
        * @param const CMmDataPackage* aDataPackage: Packaged data.
        * @return TInt: KErrNone or error value.
        */
        TInt CreateReq( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * Handles SimPbResp ISI -message
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @param TBool& aComplete: Indicates if request can remove from
        *        operationlist or not.
        * @return TInt: KErrNone or error value
        */
        TInt HandleSimPbRespL(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Handles SimPbResp ISI -message, Cache.
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @param TBool& aComplete: Indicates if request can remove from
        *        operationlist or not.
        * @return TInt: KErrNone or error value
        */
        TInt HandleSimPbRespLCacheL(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Handles SimPbResp ISI -message, Get info
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @param TBool& aComplete: Indicates if request can remove from
        *        operationlist or not.
        * @return TInt: KErrNone or error value
        */
        TInt HandleSimPbRespGetInfo(
            const TIsiReceiveC& aIsiMessage,
            TBool& aComplete );

        /**
        * Prepares operation to start caching.
        * @param const CMmDataPackage* aDataPackage: data
        * @return TInt: KErrNone or error value
        */
        TInt PrepareCacheReq( const CMmDataPackage* aDataPackage );

        /**
        * Creates and sends ISI message in order to read entry from SIM
        * @param TUint8 aTransactionId : Transaction id
        * @param TInt aLocation : Location index
        * @return TInt : KErrNone or error value
        */
        TInt SimPbReqRead( TUint8 aTransactionId, const TInt aLocation );

        /**
        * Checks if this cacheing operation is prepared to be launched
        * @return TBool ETrue - operation is ready to be launched
        */
        TBool IsPrepared() const;

        /**
        * Cancels cacheing request
        */
        void CancelReq();

        /**
        * Prepares the operation (makes it ready to be launched).
        * @param aIpc IPC request
        * @param aDataPackage parameters to prepare request with
        * @return TInt KErrNotReady on attempt to prepare prepared operation
        * @return TInt KErrArgument on attempt to create malformed request
        */
        TInt PrepareReq(
            TInt aIpc,
            const CMmDataPackage* aDataPackage
            );

        /**
        * Launches activated operation.
        * @return TInt KErrNotReady on attempt to launch not prepared operation;
        * or system-wide error code
        */
        TInt LaunchReq();

        /**
        * Completes the request.
        * @param aErrorCode error code to be completed with
        * @return TInt KErrNone or KErrNotSupported if operation is not prepared
        */
        TInt CompleteReq( TInt aErrorCode );

    public: // Data
        // None

    protected: // Data
        // None

    private: // Data

        // Number of used SDN entries
        TInt iNumOfUsedSDNEntries;

        // Number of used VMBX entries
        TInt iNumOfUsedVMBXEntries;

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        // Flag keeps track if GAS is cashed or not
        TBool iGASCacheIsReady;

        // Flag keeps track if AAS is cashed or not
        TBool iAASCacheIsReady;

#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

    };
#endif // _CMMPHONEBOOK_OPERATION_CACHE_H

// End of File
