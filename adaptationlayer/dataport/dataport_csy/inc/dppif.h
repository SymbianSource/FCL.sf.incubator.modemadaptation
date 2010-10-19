/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef DPPIF_H
#define DPPIF_H

// INCLUDES
#include "dpobserver.h" // cdpsubject is defined here

// LOCAL CONSTANTS
// none

// DATA TYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  CDpPif pipe interface (PIF) controls pipe state.
*  CDpPif opens and closes the pipe, and handles pipe state related
*  PEP messages from message interface (MIF). CDpPif is also used
*  to send pipe flow control change indication.
*/
class CDpPif : public CActive
    {

    public:  // Enumerations

        enum TDpPipeState
            {
            EDpPipeDisabled = 0,    
            EDpPipeEnabled,
            EDpPipeDisconnected
            };

        enum TDpPifState 
            {
            EDpPifInitial,
            EDpPifReady,
            EDpPifRegister
            };

    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aDataPort: Dataport main class
        */
        CDpPif( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpPif();

    public:  // New methods

        /**
        * This method returns pipe state.
        * @return TDpPipeState: Pipe state
        */
        inline TDpPipeState PipeState();

        /**
        * This method returns pipe handle.
        * @return TUint8: Pipe handle
        */
        inline TUint8 PipeHandle();

        /**
        * This method sets pipe handle.
        * @param aPipeHandle: Pipe handle
        */
        inline void SetPipeHandle( const TUint8 aPipeHandle );

        /**
        * This method sets pipe state.
        * @param aPipeState: Pipe state
        */
        inline void SetPipeState( const TDpPipeState aPipeState );

#ifdef PIPECAMP_DATAPORT_PNS_PEP_STATUS_IND_PHONET_ADDRESS_FROM_PNS_PEP_CTRL_REQ // 20100523_1300
        inline void SetPipeControllerDeviceIdentifier( const TUint8 aDeviceId );
        
        inline void SetPipeControllerObjectIdentifier( const TUint8 aObjectId );
				
				inline TUint8 GetPipeControllerDeviceIdentifier();
				
				inline TUint8 GetPipeControllerObjectIdentifier();
#endif
        /**
        * This method registers DataPort to Name Service.
        * @param aUnit: Port unit number
        */
        TInt InitiatePipe( const TUint aUnit );

        /**
        * This method unregisters DataPort from Name Service.
        * @param aUnit: Port unit number
        */
        void ClosePipeL( const TUint aUnit );

        /**
        * This method returns pif state.
        * @return TDpPifState: Pif state
        */
        inline CDpPif::TDpPifState CDpPif::PifState();

        /**
        * Attach observer to CDpSubject.
        * @param aObserver: Observer
        */
        void Attach( CDpObserver& aObserver );

        /**
        * Detach observer from CDpSubject.
        * @param aObserver: Observer
        */
        void Detach( CDpObserver& aObserver );

        /**
        * Notify all observers of CDpSubject object.
        */
        void Notify();

    public:  // Methods from base classes

        // From CActive

        /**
        * From CActive This method signals PIF with KErrCancel,
        * if PIF is registered state.
        */
        void DoCancel();

        /**
        * From CActive This method changes PIF state after registration.
        */
        void RunL();

        /**
        * From CActive This method handles leave in RunL().
        * This method returns the given error value.
        * @param aError: Leave reason
        * @return TInt: Leave reason is returned in error code
        */
        TInt RunError( TInt aError );

    protected:  // Data

        // Pif subject
        CDpSubject iDpSubject;

        // DataPort
        CDpDataPort& iDataPort;

        // Pipe state
        TDpPipeState iPipeState;

        // Pipe handle
        TUint8 iPipeHandle;
#ifdef PIPECAMP_DATAPORT_PNS_PEP_STATUS_IND_PHONET_ADDRESS_FROM_PNS_PEP_CTRL_REQ // 20100523_1300
        TUint8 iPipeControllerDevId;
        TUint8 iPipeControllerObjId;
#endif        
        // PIF state
        TDpPifState iPifState;

        // Is PEP registered
        TBool iPepRegistered;

    private:  // Data

        //none

    };

#include "dppif.inl" // inline methods

#endif  // DPPIF_H

//  End of File
