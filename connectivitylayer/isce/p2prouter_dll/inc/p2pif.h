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



#ifndef __P2PIF_H__
#define __P2PIF_H__

// Includes
#include <klib.h>           // For DBase
#include <mp2pif.h>         // For MP2PIf

// Forward declarations
class DP2PKernelChannel;

/**
*  Implementation for P2P kernel interface.
*/
NONSHARABLE_CLASS( DP2PIf ) : public DBase, public MP2PIf
{

    public:

        /*
        * Constructor.
        * Throws fault if memory not available.
        */
        static DP2PIf* NewF();

        /*
        * Destructor.
        */
        ~DP2PIf();

        // From MP2PIf start

        /*
        * See comments from MP2PIf
        */
        TDes8& AllocateBlock( const TUint16 aSize );

        /*
        * See comments from MP2PIf
        */
        void Close();

        /*
        * See comments from MP2PIf
        */
        virtual void ConnectionLost( TInt& aStatus, const TDfc& aConnectionResetedCompletedDfc );

        /*
        * See comments from MP2PIf
        */
        virtual void ConnectionLostCancel();

        /*
        * See comments from MP2PIf
        */
        void DeallocateBlock( TDes8& aDataBlock );

        /*
        * See comments from MP2PIf
        */
        void Open( const TP2PProtocol aP2PProtocolId, TInt& aDfcStatus,TDfc& aOpenCompleteDfc );

        /*
        * See comments from MP2PIf
        */
        void OpenCancel();

        /*
        * See comments from MP2PIf
        */
        void Receive( TDes8*& aRxDataBlock, TInt& aStatus, const TDfc& aReceiveCompletedDfc );

        /*
        * See comments from MP2PIf
        */
        void ReceiveCancel();

        /*
        * See comments from MP2PIf
        */
        void Release();

        /*
        * See comments from MP2PIf
        */
        TInt Send( TDes8& aData );

        // From MP2PIf end



    private:

        /**
         * Private constructor.
         * One reason for this is class size BC.
         */
        DP2PIf();

    private:

        // Owned
        // Kernel channel dedicated for this.
        // Synchronization done in kernel channel.
        DP2PKernelChannel*  iKernelChannel;

    };

#endif // __P2PIF_H__

// End of File.

