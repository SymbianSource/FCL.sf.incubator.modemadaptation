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



/** @mmuxtrxif.h
@brief Abstract interface class. Part of IST API.
@publishedDeviceAbstraction
*/

#ifndef __MMUXTRXIF_H__
#define __MMUXTRXIF_H__

// Include files
#include <e32def.h>         // For TInt
#include <e32cmn.h>         // For TDes8

// Macros
/*
A equals to asserted value b equals unique identifier of transceiver and c equals to TTransceiverFault codes. 
*/
#define TRX_ASSERT_FAULT_MACRO( a, b, c ) if( !( a ) ){ Kern::Fault( "mmuxtrxif:", ( ( TUint32 ) ( ( TUint32 ) b | ( TUint32 ) c << 16 ) ) ); }

/**
@brief Abstract interface class to use transceiver services.

Interface implemented by transceiver and used from multiplexer.

Class is used to transmit data from multiplexer to transceiver.
*/
class MMuxTrxIf
    {

    public:

        /**
        Data transmission priorities.
        */
        enum TDataTransmitPriority
            {
            EPriorityNormal     = 5,
            EPriorityHigh       = 10,
            };

        /**
        Interface functions fault values.
        */
        enum TTransceiverFault
           {
           ENotThreadContext    = 1,
           ETransmissionFailed,
           };

        /**
        Transmits data from multiplexer to transceiver.
        Transceiver is responsible of transmitting the data over the interconnection.
        If data is needed to be queued, data with highest priority shall be transmitted first over the interconnection.
        Transmission over the interconnection shall be done in reliable and ordered manner.
        Transceiver is responsible of deallocating the data block after transmission with MTrxMuxIf::DeallocateBlock.
        Transceiver is recommended to transmit data over the interconnection in its own thread context to avoid blocking calling thread for too long.
        If not called in kernel thread context fault ENotThreadContext thrown with TRX_ASSERT_FAULT_MACRO.
        If data transmission reliability or ordering failed fault ETransmissionFailed thrown with TRX_ASSERT_FAULT_MACRO.
        Execution:          Synchronous
        Re-entrant:         No
        Can block:          Yes
        Panic mode:         Kern::Fault
        Memory allocation:  No
        Execution time:     Quick
        SMP safe:           Yes
        @param aData data to be transmitted
        @param aPriority priority of the data to be transmitted
        @param aMuxingHeader multiplexing protocol header formulated by multiplexer
        @return void
        @pre Called always in kernel thread context
        @pre No fastmutex held
        @post Calling thread not blocked
        */
        virtual void Transmit( TDes8& aData, const TDataTransmitPriority aPriority, const TUint32 aMuxingHeader ) = 0;

    };

#endif // __MMUXTRXIF_H__
