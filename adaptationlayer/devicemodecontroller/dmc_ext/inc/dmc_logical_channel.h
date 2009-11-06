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
*     DMC Logical Channel class declaration.
*
*/


#ifndef DMC_LOGICAL_CHANNEL_H
#define DMC_LOGICAL_CHANNEL_H

#ifndef DMC_INTERNAL_INCLUDE
#error Illegal usage of dmc_logical_channel.h, It is only for DMC internal use !
#endif // DMC_INTERNAL_INCLUDE

// INCLUDES
#include <kernel.h>    // DLogicalChannel

//  CONSTANTS  
//  MACROS  
//  DATA TYPES 
//  EXTERNAL DATA STRUCTURES
//  FUNCTION PROTOTYPES  
//  FORWARD DECLARATIONS
class DDmcLogicalDevice;
class DThread;

//  CLASS DEFINITIONS 

/**
* DDmcLogicalChannel
*/
NONSHARABLE_CLASS(DDmcLogicalChannel) : public DLogicalChannel
    {
    public: // Constructor / destructor
        /**
        * C++ default constructor.
        */
        DDmcLogicalChannel(DLogicalDevice* aLogicalDevicePtr);

        /**
        * C++ default destructor.
        */
        ~DDmcLogicalChannel();

    public: // from DLogicalChannelBase
        /**
        * Second stage constructor called by the kernel's device driver framework.
        * This is called in the context of the user thread (client) which requested
        * the creation of a Logical Channel (E.g. through a call to RBusLogicalChannel::DoCreate)
        * The thread is in a critical section.

        @param aUnit     The unit argument supplied by the client to RBusLogicalChannel::DoCreate
        @param aInfo     The info argument supplied by the client to RBusLogicalChannel::DoCreate
        @param aVer      The version argument supplied by the client to RBusLogicalChannel::DoCreate

        @return KErrNone or standard error code.
        */
        TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);

    public: // from DLogicalChannel
        /**
         * @param  aMsg     The message to process.
         *                  The iValue member of this distinguishes the message type:
         *                  iValue==ECloseMsg, channel close message
         *                  iValue==KMaxTInt, a 'DoCancel' message
         *                  iValue>=0, a 'DoControl' message with function number equal to iValue
         *                  iValue<0, a 'DoRequest' message with function number equal to ~iValue
         * return  void 
         */
        void HandleMsg(TMessageBase* aMsg);

        /**
        * Send an event to a user side client.
        *
        * @param   aEvent     A type of the event.
        * @return  None
        */
        void SendEvent(const RDmc::TEvent& aEvent);

    private:
        /**
        * Handles a synchronous request.
        * @param   aFunction   A number identifying specific functionality.
        * @param   *a1         A parameter from the user side.
        * @return  KErrNone if successful, otherwise one of the system wide error codes.
        */
        TInt DoControl(const TInt aFunction, TAny* a1);

        /**
        * Handles an asynchronous request.
        * @param   aReqId      A request number.
        * @param   aStatus     Request status, completed with KErrNone when data has arrived.
        * @param   *a1         A parameter from the user side.
        * @return  KErrNone if successful, otherwise one of the system wide error codes.
        */
        TInt DoRequest(const TInt aReqId, TRequestStatus* const aStatus, TAny* const a1);

    private: // Data members

        /**
        * A pointer to the logical device class.
        */
        DDmcLogicalDevice* iDmcLogicalDevPtr; 

        /**
        * A pointer to a client thread.
        */
        DThread* iClientThreadPtr;

        /**
        * A pointer to a client request status.
        */
        TRequestStatus* iClientStatusPtr;

        /**
        * A pointer where client event data is written to.
        */
        RDmc::TEvent* iClientEventBufferPtr;
    }; 
     
#endif  // DMC_LOGICAL_CHANNEL_H

// End of File
