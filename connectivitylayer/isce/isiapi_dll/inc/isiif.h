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


#ifndef _ISIIFUSER_H_
#define _ISIIFUSER_H_

#include <e32cmn.h>     // For RBusLogicalChannel
#include <misiif.h>     // For MISIIf

/*
*  Implementation for ISI user interface.
*/
NONSHARABLE_CLASS( RISIIf ) : public RBusLogicalChannel, public MISIIf
    {

    public:

        /*
        * Constructor.
        * Panics if memory not available.
        */
        static RISIIf* NewF( const TInt32 aUID, TUint8& aObjId );
        
        /*
        * Destructor
        */
        ~RISIIf();
        
        // From MISIIf start
        
        /*
        * See comments from MISIIf
        */
        void Receive( TRequestStatus& aRxStatus, TDes8& aRxMsg, TUint16& aNeededBufLen );

        /*
        * See comments from MISIIf
        */
        void ReceiveCancel();

        /*
        * See comments from MISIIf
        */
        void Release();

        /*
        * See comments from MISIIf
        */        
        TInt Send( const TDesC8& aTxMsg );
        
        // From MISIIf end

    private:

        /**
        * Private constructor.
        * One reason for this is class size BC.
        */
        RISIIf( const TInt32 aUID, TUint8& aObjId );
                
    };

#endif      // __ISIIFUSER_H__

// End of File
