/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMMESSAGEROUTER_H
#define CMMMESSAGEROUTER_H

// INCLUDES

//NCP_COMMON_S60_VERSION_SUPPORT flag.
#if ( NCP_COMMON_S60_VERSION_SUPPORT == S60_VERSION_32 )
#include <cmmmessagerouterbase.h>
#else
#include <ctsy/pluginapi/mmessagerouter.h>
#include <ctsy/pluginapi/mmmessagemanagercallback.h>
#endif // NCP_COMMON_S60_VERSION_SUPPORT

#include <iscapi.h>
#include "mmmmesshandlerbase.h"

// CONSTANTS
    //None

// MACROS
    //None

// DATA TYPES
    //None

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
class CMmCallMessHandler;
class CMmPhoNetSender;
class CMmPhoNetReceiver;
class CMmCallMessHandler;
class CMmConferenceCallMessHandler;
class CMmDtmfMessHandler;
class CMmNetMessHandler;
class CMmNetOperatorNameHandler;
class CMmSupplServMessHandler;
class CMmPhoneBookStoreMessHandler;
class CMmENStoreMessHandler;
class CMmPacketServiceMessHandler;
class CMmPacketContextMessHandler;
class CMmPacketQoSMessHandler;
class CMmCustomMessHandler;
class CMmBroadMessHandler;
class CMmUssdMessHandler;
class CMmSmsMessHandler;
class CMmSecurityMessHandler;
class CMmPhoneMessHandler;
class CTsySatMessagingBase;
class CMmUiccMessHandler;

#if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
class CMmMessageManager;
#endif

// CLASS DECLARATION

/**
*  Routes between Tsy objects and Message Handler objects
*/
class CMmMessageRouter :
    #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
    public CMmMessageRouterBase
    #else
    public CBase, public MMessageRouter
    #endif // NCP_COMMON_S60_VERSION_SUPPORT
    {

    // public methods
    public:

        /**
        * Creates a new CMmMessageRouter object
        * @return reference to created CMmMessageRouter type of object
        */
        IMPORT_C static CMmMessageRouter* NewL(
        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        CMmMessageManager* aMessageManager );
        #else
        MmMessageManagerCallback* aMessageManager );
        #endif // NCP_COMMON_S60_VERSION_SUPPORT

        /**
        * Destructor
        */
        virtual ~CMmMessageRouter();

        /**
        * Creates and initialises class attributes
        */
        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        void ConstructL( CMmMessageManager* aMessageManager );
        #else
        void ConstructL( MmMessageManagerCallback* aMessageManager );
        #endif // NCP_COMMON_S60_VERSION_SUPPORT

        /**
        * Completes a request or a notification with no data
        * @param aIpc
        * @param aResult
        */
        void Complete( TInt aIpc, TInt aResult );

        /**
        * Completes a request or a notification with packaged data
        * @param aIpc
        * @param aData
        * @param aResult
        */
        void Complete( TInt aIpc, CMmDataPackage* aData, TInt aResult );

        /**
        * Returns pointer to the message handler for this
        * request, based on IPC number
        */
        MMmMessHandlerBase* RouteRequest( TInt aIpc );

        /**
        * From MMessageRouter. Passes request to the correct message
        * handler
        * @param TInt aIpc: number of the request
        * @param CMmDataPackage* aDataPackage: pointer to data package
        * @return TInt KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

        /**
        * Returns pointer to Phonet sender
        */
        IMPORT_C CMmPhoNetSender* GetPhoNetSender();

        /**
        * Returns pointer to Phonet receiver
        */
        IMPORT_C CMmPhoNetReceiver* GetPhoNetReceiver();

        /**
        * Returns pointer to call message handler
        */
        CMmCallMessHandler* GetCallMessHandler();

        /**
        * Returns pointer to GSM-specific Phone message handler
        */
        CMmPhoneMessHandler* GetPhoneMessHandler();

        /**
        * Returns pointer to Packet Service Message handler
        */
        CMmPacketServiceMessHandler* GetPacketServiceMessHandler();

        /**
        * Returns pointer to GSM-specific Custom API Message handler
        */
        CMmCustomMessHandler* GetCustomMessHandler();

        /**
        * Returns pointer to SMS message handler
        */
        CMmSmsMessHandler* GetSmsMessHandler();

        /**
        * Returns pointer to SS message handler
        */
        CMmSupplServMessHandler* GetSupplServHandler();

        /**
        * Returns pointer to phonebookstore message handler
        */
        CMmPhoneBookStoreMessHandler* PhoneBookStoreMessHandler();

        /**
        * Returns pointer to NET message handler
        */
        CMmNetMessHandler* GetNetMessHandler();

    // protected methods
    protected:

    // private methods
    private:

        /**
        * Default constructor
        */
        CMmMessageRouter();

    // public attributes
    public:
        
		// pointer to SAT Messaging
		CTsySatMessagingBase* iSatMessaging;

    // protected attributes
    protected:

        #if (NCP_COMMON_S60_VERSION_SUPPORT==S60_VERSION_32)
        // pointer to message manager
        CMmMessageManager* iMessageManager;
        #else
        MmMessageManagerCallback* iMessageManager;
        #endif //NCP_COMMON_S60_VERSION_SUPPORT

        // pointer to GSM-specific Phone message handler
        CMmPhoneMessHandler* iMmPhoneMessHandler;

        // pointer to Call message handler
        CMmCallMessHandler* iMmCallMessHandler;

        // pointer to Conference Call message handler
        CMmConferenceCallMessHandler* iMmConferenceCallMessHandler;

        // pointer to Dtmf message handler
        CMmDtmfMessHandler* iMmDtmfMessHandler;

        // pointer to Net message handler
        CMmNetMessHandler* iMmNetMessHandler;

        // pointer to USSD message handler
        CMmUssdMessHandler* iMmUssdMessHandler;

        // pointer to Supplementary Services message handler
        CMmSupplServMessHandler* iMmSupplServMessHandler;

        // pointer to PhoneBook store message handler
        CMmPhoneBookStoreMessHandler* iMmPhoneBookStoreMessHandler;

        // pointer to emergency number store message handler
        CMmENStoreMessHandler* iMmENStoreMessHandler;

        // pointer to Phonet/ISA interface
        RIscApi iPhoNet;

        // pointer to Phonet sender
        CMmPhoNetSender* iPhoNetSender;

        // pointer to Phonet receiver
        CMmPhoNetReceiver* iPhoNetReceiver;

        // pointer to Packet Service Message handler
        CMmPacketServiceMessHandler* iMmPacketServiceMessHandler;

        // pointer to Packet Context Message handler
        CMmPacketContextMessHandler* iMmPacketContextMessHandler;

        // pointer to Packet QoS Message handler
        CMmPacketQoSMessHandler* iMmPacketQoSMessHandler;

        // pointer to Custom Tsy GSM extension Message handler
        CMmCustomMessHandler* iMmCustomMessHandler;

        //pointer to the Broadcast messagehandler
        CMmBroadMessHandler* iMmBroadMessHandler;

        //pointer to the SMS message handler
        CMmSmsMessHandler* iMmSmsMessHandler;

        //pointer to security message handler
        CMmSecurityMessHandler* iMmSecurityMessHandler;

        // Pointer to UICC message handler
        CMmUiccMessHandler* iMmUiccMessHandler;

    // private attributes
    private:

    };

#endif // CMMMESSAGEROUTER_H

// End of file
