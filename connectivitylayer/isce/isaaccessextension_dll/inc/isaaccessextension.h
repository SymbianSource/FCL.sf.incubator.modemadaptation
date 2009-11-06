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



#ifndef INC_DISAACCESSEXTENSION_H
#define INC_DISAACCESSEXTENSION_H

#include "internalapi.h"        // For MIAD2ISTApi, MChannel2IADApi

class DRouter;
class TDfcQueue;

enum TIADDfcThread
{
    EIADLddDfcQueue = 0,
    EIADExtensionDfcQueue,
    EIADSizeOfDfcThreadList
};


/*
* Rules:
*         
*        All functions must be called in kernel thread context: TODO: check-macro for this.
*
*
*/
NONSHARABLE_CLASS( DIsaAccessExtension ) : public DBase
    {
    
  public:
  
    DIsaAccessExtension();
    
    ~DIsaAccessExtension();
    
    IMPORT_C static MChannel2IADApi* GetChannel2IADApi();

    IMPORT_C static TDfcQue* GetDFCThread( TIADDfcThread aDfcThread );

    static DRouter* GetRouter(){ return iRouter; };

  private:
  
      // owned
    static DRouter*     iRouter;
    // Keeps list of DFC queue's created in this extension
    static TDfcQue*     iDfcQueueList[ EIADSizeOfDfcThreadList ];
     
    };



#endif /* INC_DISAACCESSEXTENSION_H */
