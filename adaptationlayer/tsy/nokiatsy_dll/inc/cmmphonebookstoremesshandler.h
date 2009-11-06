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

#ifndef CMMPHONEBOOKSTOREMESSHANDLER_H
#define CMMPHONEBOOKSTOREMESSHANDLER_H

//  INCLUDES
#include <ctsy/serviceapi/cmmgsmphonestorageutility.h>
#include "mmmmesshandlerbase.h"

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
#include <ctsy/rmmcustomapi.h>
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

#include "cmmphonetsender.h"
#include "cmmphonetreceiver.h"
#include "nokiatsy_internal_variation.h"

//UICC
#include "muiccoperationbase.h"
#include "cmmuiccmesshandler.h"

//  CONSTANTS
    //none

//  MACROS
    //none

//  DATA TYPES
    //none

//  EXTERNAL DATA STRUCTURES

//  FUNCTION PROTOTYPES
    //none

//  FORWARD DECLARATIONS
class TIsiReceiveC;
class CMmPhoneBookStoreOperationBase;
class CMmPhoneBookStoreOperationList;
class CMmMessageRouter;
//class CMmUiccMessHandler;

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
class CMmPhonebookAlphaString;
#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

// CLASS DECLARATION
/**
* CMmPhoneBookStoreMessHandler is used to create and send GSM-specific
* PBStore ISI messages to PhoNet via PhoNetSender. It also receives
* GSM-specific PBStore ISI messages.
*/
class CMmPhoneBookStoreMessHandler
    : public CBase, 
      public MMmMessHandlerBase, 
      public MMmMessageReceiver,
      public MUiccOperationBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmPhoneBookStoreMessHandler* NewL(
            CMmMessageRouter* aMessageRouter,
            CMmUiccMessHandler* aUiccMessHandler);

        /**
        * Destructor.
        */
        ~CMmPhoneBookStoreMessHandler();

        /**
        * From CMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        *
        * @param aIpc Identify number of request.
        * @param aDataPackage data
        * @return KErrNone or error code
        */
        TInt ExtFuncL(
            TInt aIpc,
            const CMmDataPackage* aDataPackage );

        
        /**
        * Handles a received message by calling the specific
        * message handling method.
        *
        * @param Transaction id Reference to the received message transaction ID.
        * @param Status
        * @param aFileData reference to Data received in message
        * @return KErrNone or error code
        */
        TInt ProcessUiccMsg( TInt aTransactionId, TInt aStatus, const TDesC8 &aFileData );
        

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
        /**
        * Gets pointer to list of Group Alpha String
        *
        * @return Pointer to CMmPhoNetSender object.
        */
        CArrayPtrSeg<CMmPhonebookAlphaString>* Gas();

        /**
        * Update list of Group Alpha String.
        *
        * @param aAas New list.
        * @return None
        */
        void SetGas( CArrayPtrSeg<CMmPhonebookAlphaString>* aGas );

        /**
        * Gets pointer to list of Additional Alpha String
        *
        * @param None
        * @return Pointer to CMmPhoNetSender object.
        */
        CArrayPtrSeg<CMmPhonebookAlphaString>* Aas();

        /**
        * Update list of Additional Alpha String
        *
        * @param aAas New list.
        * @return None
        */
        void SetAas( CArrayPtrSeg<CMmPhonebookAlphaString>* aAas );

        /**
        * Gets number of GAS
        *
        * @param None
        * @return Number of GAS.
        */
        TInt MaxNumberOfGAS();

        /**
        * Sets number of GAS.
        *
        * @param aMaxNumberOfGAS Number of GAS
        * @return None
        */
        void SetMaxNumberOfGAS( TInt aMaxNumberOfGAS );

        /**
        * Gets number on AAS entries.
        *
        * @param
        * @return Number of AAS entries.
        */
        TInt MaxNumOfAasEntries();

        /**
        * Sets maximum number os AAS entries
        *
        * @param aMaxNumOfAasEntries maximum number os AAS entries
        * @return None
        */
        void SetMaxNumOfAasEntries( TInt aMaxNumOfAasEntries );

#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        /**
        * Gets MBDN phonebook flag.
        *
        * @param None
        * @return Flag if MBDN phonebook is ready or not.
        */
        TBool MBDNPhonebook();

        /**
        * Sets MBDN phonebook flag.
        *
        * @param aBoolean Flag of MBDN phonebook.
        * @return None
        */
        void SetMBDNPhonebook( TBool aBoolean );

        /**
        * Sets number of FDN info.
        *
        * @param aNumber Number of FDN info.
        * @return None
        */
        void SetNumberOfFdnInfoResps( TUint8 aNumber );

        /**
        * Gets pointer to CMmMessageRouter class.
        *
        * @param None
        * @return Pointer to CMmMessageRouter object.
        */
        CMmMessageRouter* MessageRouter();

        
        /**
        * Gets pointer to CMmUiccMessHandler class
        *
        * @param None
        * @return Pointer to CMmUiccMessHandler object.
        */
        CMmUiccMessHandler* UiccMessHandler();

        /**
        * Gets pointer to CMmPhoNetSender class.
        *
        * @param None
        * @return Pointer to CMmPhoNetSender object.
        */
        CMmPhoNetSender* PhoNetSender();
        
        /**
        * Gets pointer to CMmUiccMesshandler class.
        *
        * @param None
        * @return Pointer to CMmMessageRouter object.
        */
        //CMmUiccMesshandler* UiccMessHandler();
        

    protected:
        // None

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        CMmPhoneBookStoreMessHandler();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL(
            CMmPhoneBookStoreMessHandler* mmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aUiccMessHandler);

        /**
        * Gets ISI message from PhoNetReceiver related to SIM_IND
        *
        * @param TIsiReceiveC& aIsiMessage: Received ISI message
        * @return None
        */
        void SimInd( const TIsiReceiveC& aIsiMessage );

    public:     // Data
        // None

    protected:  //Data
        // None

    private:    // Data

        // Pointer to the Phonet sender
        CMmPhoNetSender*  iPhoNetSender;

        //Pointer to the Message router
        CMmMessageRouter* iMessageRouter;

        // Pointer to the UICC Messhandler
        CMmUiccMessHandler* iMmUiccMessHandler;
        
        // Array for storing objects of operations.
        CMmPhoneBookStoreOperationList* iOperationlist;

        // Number of FDN info responses
        TUint8 iNumberOfFdnInfoResps;
        
        // to store CardType 
        TUint8 iCardType;
        

#ifdef INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS

        // Keeps track of readed location of AAS/GAS
        TUint16 iCount;

        // Holds information for AAS and GAS
        RMmCustomAPI::TAlphaStringParams iAlphaStringParams;

        // Max number of GAS
        TInt iMaxNumberOfGAS;

        // Keeps track of max number of AAS
        TInt iMaxNumOfAasEntries;

        // Array for storing aas texts
        CArrayPtrSeg<CMmPhonebookAlphaString>* iAas;

        // Array for storing GAS texts
        CArrayPtrSeg<CMmPhonebookAlphaString>* iGas;

#endif // INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
};

#endif // CMMPHONEBOOKSTOREMESSHANDLER_H

// End of File
