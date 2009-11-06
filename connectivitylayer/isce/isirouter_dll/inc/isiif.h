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



#ifndef __ISIIFKERNEL_H___
#define __ISIIFKERNEL_H___


// Includes
#include <klib.h>           // For DBase
#include "misiif.h"         // For MISIIf


// Forward declarations
class DISIKernelChannel;


/*
*  Implementation for ISI kernel interface.
*/
NONSHARABLE_CLASS( DISIIf ) : public DBase, public MISIIf
    {

    public:

        /*
        * Constructor.
        * Throws fault if memory not available.
        */
        static DISIIf* NewF( const TInt32 aUID, TUint8& aObjId );

        /*
        * Destructor.
        */
        ~DISIIf();

        // From MIsiIf start

        /*
        * See comments from MISIIf
        */
        TDes8& AllocateMsgBlock( const TUint16 aSize );
        
        /*
        * See comments from MISIIf
        */
        void DeallocateMsgBlock( TDes8& aMsgBlock );
        
        /*
        * See comments from MISIIf
        */
        void Receive( TRequestStatus& aRxStatus, TDes8*& aRxMsg, const TDfc& aRxCompletedDfc );
        
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

        // From MIsiIf end

    private:

        /**
        * Private constructor.
        * One reason for this is class size BC.
        */
        DISIIf( const TInt32 aUID3, TUint8& aObjId );

    private:

        // Owned
        // Kernel channel dedicated for this.
        // Synchronization done in kernel channel.
        DISIKernelChannel* iKernelChannel;

    };

#endif // __ISIIFKERNEL_H__

// End of File.
