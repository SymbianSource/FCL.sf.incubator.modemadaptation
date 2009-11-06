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
*     DMC Logical Device class declaration.
*
*/


#ifndef DMC_LOGICAL_DEVICE_H
#define DMC_LOGICAL_DEVICE_H

#ifndef DMC_INTERNAL_INCLUDE
#error Illegal usage of dmc_logical_device.h, It is only for DMC internal use !
#endif // DMC_INTERNAL_INCLUDE

// INCLUDES
#include <kernel.h>        // DLogicalDevice

//  CONSTANTS  
//  MACROS  
//  DATA TYPES 
//  EXTERNAL DATA STRUCTURES
//  FUNCTION PROTOTYPES  
//  FORWARD DECLARATIONS
//  CLASS DEFINITIONS 


/**
* DDmcLogicalDevice
*/
NONSHARABLE_CLASS(DDmcLogicalDevice) : public DLogicalDevice
    {
    public:
        /**
        * C++ default constructor.
        */
        DDmcLogicalDevice();
        
        /**
        * C++ default destructor.
        */
        ~DDmcLogicalDevice();

    public: // from DLogicalDevice        
        /**
        * Second stage constructor, completes the installation
        * of this the device.
        *
        * @param   None
        * @return  KErrNone or standard error code 
        */
        TInt Install();

        /**
        * Returns device capabilities.
        * @param   aDes, On return, contains information describing the capabilities
        *                of the device.
        * @return  none
        */
        void GetCaps(TDes8& aDes) const;

        /**
        * Creates a logical channel
        *
        * @param     aChannel, Set to point to the created Logical Channel.
        * @return    KErrNone or standard error code 
        */
        TInt Create(DLogicalChannelBase*& aChannel);

    public: // New functions
        /**
        * Returns a target startup mode in which a device is started.
        *
        * @param     &aTargetStartUpmode    A reference in which a target startup mode
        *                                   is written.
        * @return    KErrNone or one of the system wide error
        */
        TInt GetTargetStartUpMode(RDmc::TTargetStartupMode &aTargetStartUpmode) const;

        /**
        * Generates a reset.
        *
        * @param     aTargetStartUpmode    A mode in which a device is reset.
        * @return    KErrNone or KErrLocked
        */
        TInt GenerateReset(const RDmc::TTargetStartupMode aTargetStartUpmode) const;

        /**
        * Power OFF the device
        *
        * @param     None
        * @return    KErrNone or KErrLocked
        */
        TInt PowerOff() const;

        /**
        * Power OFF the device
        *
        * @param     aHiddenStatus    ETrue if the reset is hidden, otherwise EFalse.
        * @return    KErrNone or one of the system wide error
        */
        TInt IsHiddenReset(TBool& aHiddenStatus) const;

        /**
        * Subscribe events
        *
        * Subscribes modem or PC SW orginated reset and power off events.
        *
        * @param     aLogChnlPtr   Pointer to logical channel.
        * @return    None
        */
        void SubscribeEvents(const DDmcLogicalChannel* const aLogChnlPtr);

        /**
        * Unsubscribe events
        * 
        * Unsubscribe power off and reset events.
        *
        * @param     aLogChnlPtr   Pointer to logical channel.
        * @return    None
        */
        void UnsubscribeEvents(const DDmcLogicalChannel* const aLogChnlPtr);

        /**
        * Returs a pointer to iDmcLogDeviceDfcQueuePtr;
        *
        * @param     None
        * @return    TDynamicDfcQue*    A pointer to the DMC Logical Device owned 
        *                               dynamic dfc queue.
        */
        TDynamicDfcQue* GetDynamicDfcQueue() const;

    private: // New functions
        /**
        * Dfc function for receiving events.
        *
        * @param aPtr     Pointer to DDmc logical deveice object.
        * @return         None
        */
        static void ReceiveEventsDfc(TAny* Ptr);

    private: // Data members
        /**
        * A pointer table to a DMC logical channels,
        * needed for delivering events to a user side clients.
        */
        RPointerArray <DDmcLogicalChannel> iLogChannelsPtr;
        
        /**
        * A pointer to a dynamic dfc queue owned by the DMC logical device.
        */
        TDynamicDfcQue* iDmcLogDeviceDfcQueuePtr;

        /**
        * A pointer to DMC user / kernel Dfc-queue.
        * This class owns this pointer.
        * 
        */
        TDfc* iUserEventDfcPtr;
    };

#endif // DMC_LOGICAL_DEVICE_H

//  End of File
