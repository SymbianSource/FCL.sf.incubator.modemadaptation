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

#ifndef CMMPHONEBOOKOPERATIONLIST_H
#define CMMPHONEBOOKOPERATIONLIST_H

//  INCLUDES
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <e32base.h>

#include "cmmphonebookstoreoperationbase.h"

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
    //none

// CLASS DECLARATION
class CMmMessageManager;
class CMmPhoneBookStoreMessHandler;
//class CMmUiccMessHandler;

/**
* CMmPhoneBookStoreOperationList is used to keep list of different request(s)
* (operation(s))
*/
class CMmPhoneBookStoreOperationList
    : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMmPhoneBookStoreOperationList* NewL(
            CMmPhoneBookStoreMessHandler* aMmPhoneBookStoreMessHandler,
            CMmUiccMessHandler* aMmUiccMessHandler);

        /**
        * Destructor.
        */
        ~CMmPhoneBookStoreOperationList();

        /**
        * Creates entry point to correct operation.
        *
        * @param aDataPackage Packaged data
        * @param aIpc Identify number of request.
        * @return Pointer to operation.
        */
        CMmPhoneBookStoreOperationBase* BuildL(
            const CMmDataPackage* aDataPackage,
            TInt aIpc );

        /**
        * Get pointer to correct operation uing identify number.
        *
        * @param aTrans Identify number of request.
        * @param aSearchFrom pointer to entry to start search from
        * @return Pointer to operation.
        */
        CMmPhoneBookStoreOperationBase* Find(
            TUint8 aTrans,
            const CMmPhoneBookStoreOperationBase* aSearchFrom=NULL );

        /**
        * Remove correct operation from operation list.
        *
        * @param aTrans Transaction Id
        * @return None
        */
        void RemoveOperationFromList( TUint8 aTrans );

        /**
        * Removes last operation from operation list.
        * @return TInt KErrNone or KErrNotFound in case list is empty
        */
        TInt RemoveLastOperationFromList();

        /**
        * Calculates transaction Id from IPC number and
        * data package
        * @param aIpc IPC number
        * @param aDataPackage data package
        * @param aTransactionId resulting transaction id
        * @return TInt KErrNone or KErrNotSupported
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
         CMmPhoneBookStoreOperationList();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

    public:     // Data
        // none

    protected:  // Data
        // none

    private: //data
        // Pointer to CMmPhoneBookStoreMessHandler.
        CMmPhoneBookStoreMessHandler* iMmPhoneBookStoreMessHandler;
        
        // Pointer to CMmUiccMessHandler
        CMmUiccMessHandler* iUiccMessHandler;
        
        //CMmUiccMessHandler* iMmUiccMessHandler;
        // Array for saving operation(s)
        RPointerArray<CMmPhoneBookStoreOperationBase> iPtrOperationArray;
};

#endif // CMMPHONEBOOKOPERATIONLIST_H

// End of file.
