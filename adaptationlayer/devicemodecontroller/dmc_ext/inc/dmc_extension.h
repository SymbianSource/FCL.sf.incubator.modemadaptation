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
*     DMC extension class declaration.
*
*/


#ifndef DMC_EXTENSION_H
#define DMC_EXTENSION_H

#ifndef DMC_INTERNAL_INCLUDE
#error Illegal usage of dmc_extension.h, It is only for DMC internal use !
#endif // DMC_INTERNAL_INCLUDE

// INCLUDES
#include <kpower.h>    // DPowerHandler

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class DDmcExtension;
class DIsaKernelAPI;
class TDfcQue;
class TDfc;


// CLASS DECLARATION
/**
*  DDmcExtension
*/
NONSHARABLE_CLASS(DDmcExtension) : public DPowerHandler
    {
    public:  // Constructors
        /**
        * C++ default constructor.
        */
        DDmcExtension();

        /**
        * DMC initialization function
        *
        * @param  None
        * @return none
        */
        void Init();

    public: // from DPowerHandler
        /**
        * Called by power FW when requesting power up
        *
        * @param  None
        * @return None
        */        
        void PowerUp();

        /**
        * Called by power FW when requesting power down
        *
        * @param aTargetState    Target state to powerdown.
        * @return None
        */        
        void PowerDown(TPowerState aTargetState);


    public: // New functions

        /**
        * Isa Access Wrapper in DMC extension.
        *
        * Allocates a message block from Isa Access Driver
        *
        * @param  Size of the buffer
        * @return A reference to the allocate buffer.
        */        
        TDes8& AllocateMsgBlock(const TUint32 aSize) const;
        
        /**
        * Isa Access Wrapper  in DMC extension.
        *
        * Sends a message and deallocates the block given as parameter.
        * 
        * @param    aMsg, reference to message to be send.
        * @return   An error code
        *               KErrNone, send succesfully
        *               KErrBadDescriptor, descriptor length too small.
        *               KErrUnderflow, ISI message length either bigger than descriptor
        *                              length or bigger than maximum ISI message length
        *                              or smaller than minimum ISI message length.
        *               KErrOverFlow, send queue overflow, client must resend.
        *               KErrNotReady, modem sw down, send failed, start listen state change,
        *               when ok, possible to resend.
        */
        TInt Send(TDes8& aMsg);

        /**
        * Isa Access Wrapper in DMC extension.
        *
        * @param     aIndications, Indication subscription list.
        * @return    TInt, an error code.
        *                  KErrNone, subscribe succesfully
        *                  KErrBadDescriptor, Descriptor length too small or not according to subscribing list items.
        *                  KErrUnderflow,     Subscribing list item too long.
        *                  KErrOverFlow,      Send queue overflow, client must resend.
        *                  KErrNotReady,      Modem sw down, send failed, start listen state change,
        *                                     when ok, possible to resend.
        */
        TInt SubscribeIndications(TDes8& aIndications);

        /**
        * Completes the power down
        *
        * @param  None
        * @return None
        */
        void CompletePowerDown();

    private: // new functions

        /**
        * Dfc function for handling completion of the channel open.
        *
        * @param aPtr     Pointer to DDmc object.
        * @return         None
        */
        static void ChannelOpenedDfc(TAny* aPtr);

        /**
        * Dfc function for handling received messages.
        *
        * @param  aPtr     Pointer to DDmc object.
        * @return None
        */
        static void MsgReceivedDfc(TAny* aPtr);

        /**
        * Dfc function for handling channel state change.
        *
        * @param     aPtr pointer to DDmc object
        * @return    None
        */
        static void StateChangedDfc(TAny* aPtr);

        /**
        * DFC function for completing the power down request.
        *
        * @param  a pointer to sender object
        * @return none
        */
        static void PowerDownDfc(TAny* aPtr);

        /**
        * Handles the rest of the initialization of the communication
        * Isa Access Driver.
        *
        * @param  None
        * @return None
        */
        void CompleteIadInit();

        /**
        * Allocates a buffer for message and starts to receive ISI messages.
        *
        * @param  None
        * @return None
        */
        void ReceiveMessage();

    private: // data members

        /**
        * A pointer to the DFC queue DMC owns.
        */
        TDfcQue* iDmcDfcQueuePtr;

    private: // Isa access related

        /**
        * IAD API
        * DMC owns this pointer.
        */
        DIsaKernelAPI* iIsaKernelApiPtr;

        /**
        * Buffer in which received messages are stored.
        * DMC owns this pointer.
        */
        TDes8* iReceiveMsgPtr;

        /** 
        * Request status for isakernelif asynchronous open.
        */
        TInt iIsaKernelAPIOpenStatus;

        /**
         * Request status for isakernelif asynchronous receive.
         */
        TInt iIsaKernelAPIReceiveStatus;

        /**
        * Request status for isakernelif asynchronous state change.
        */
        TInt iIsaKernelModemStatus;

        /**
        * Dfc that is executed when isakernelIf channel is opened.
        * DMC owns this pointer.
        */
        TDfc* iChannelOpenedDfcPtr;

        /**
        * Dfc that is executed when a message is received.
        * DMC owns this pointer.
        */
        TDfc* iMsgReceivedDfcPtr;

        /**
        * Dfc that is executed when connection status changes.
        * DMC owns this pointer.
        */
        TDfc* iStateChangedDfcPtr;

        /**
        * Dfc that is executed when connection status changes.
        * DMC owns this pointer.
        */
        TDfc* iPowerDownDfcPtr;

    public: // DMC related
        /**
        * A pointer to an extension.
        * Must be public because a pointer assigned 
        * in DECLARE_STANDARD_EXTENSION(), which is
        * not part of the DDmcExtension class.
        * DMC owns this pointer.
        */
        static DDmcExtension* iDmcExtPtr;

    };
#endif  // DMC_EXTENSION_H

//  End of File
