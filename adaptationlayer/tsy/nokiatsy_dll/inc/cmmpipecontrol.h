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



#ifndef CMMPIPECONTROL_H
#define CMMPIPECONTROL_H

// INCLUDES
#include <iscnokiadefinitions.h>
#include "cmmphonetsender.h"
#include "cmmphonetreceiver.h" // for mmmmessagereceiver
#include "nokiatsy_internal_variation.h"
#ifdef INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
#include "pn_const_for_uicc.h"
#else /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <pn_const.h>
#endif /* INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING */
#include <pipeisi.h>
#include <pipe_sharedisi.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "nokiatsy_internal_variation.h"

// CONSTANTS
const TUint8 KFirstPepType = PN_PEP_TYPE_COMMON; // First PEP type
const TUint8 KSecondPepType = PN_PEP_TYPE_COMMON; // Second PEP type
const TUint8 KFirstPepDevice = PN_DEV_OWN;      // Device Id of first PEP
const TUint8 KSecondPepDevice = PN_DEV_MODEM;    // Device Id of second PEP
const TUint8 KSecondPepObject = PN_OBJ_PEP_GPDS; // Object Id of second PEP

const TUint8 KUnknownTransID  = 0x00;

const TUint8 KInvalidPipeHandle = 0xFF;

// Maximum number of simultaneous PipeHandle supported by this phone.
const TInt KMmMaxNumberOfPipeHandles  = KMmMaxNumberOfContexts;

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

// Channel ID's
// Proxy Id's are taken from IscNokiaDefinitions.h file
const TUint8 KMmPacketContextProxy1_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy2_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy3_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy4_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy5_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy6_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy7_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy8_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy9_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy10_DOS = EIscNokiaMmTsy;
const TUint8 KMmPacketContextProxy11_DOS = EIscNokiaMmTsy;

#else

const TUint8 KMmPacketContextProxy1_DOS = EIscNokiaNifPep0;
const TUint8 KMmPacketContextProxy2_DOS = EIscNokiaNifPep1;
const TUint8 KMmPacketContextProxy3_DOS = EIscNokiaNifPep2;
const TUint8 KMmPacketContextProxy4_DOS = EIscNokiaNifPep3;
const TUint8 KMmPacketContextProxy5_DOS = EIscNokiaNifPep4;
const TUint8 KMmPacketContextProxy6_DOS = EIscNokiaNifPep5;
const TUint8 KMmPacketContextProxy7_DOS = EIscNokiaNifPep6;
const TUint8 KMmPacketContextProxy8_DOS = EIscNokiaNifPep7;
const TUint8 KMmPacketContextProxy9_DOS = EIscNokiaNifPep8;
const TUint8 KMmPacketContextProxy10_DOS = EIscNokiaNifPep9;
const TUint8 KMmPacketContextProxy11_DOS = EIscNokiaNifPep10;

#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

// Dummy transaction id
const TInt KDummyTrId = 0x0F;

// Last byte index
const TInt KLastByteIndex = 3;

// MACROS
    // None

// DATA TYPES
    // None

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
class TIsiReceiveC;
class CMmDataPackage;
class CMmPacketContextMessHandler;
class CMmPacketContextMesshandlerList;

// CLASS DECLARATION
// CMmPipeControl is used to create and send GSM-specific pipe control
// ISI messages to PhoNet via PhoNetSender. It also receives
// GSM-specific pipe control ISI messages from Phonet via PhoNetReceiver.
class CMmPipeControl : public CBase,
    public MMmMessageReceiver
    {
    public: // Constructors and destructor.

        /**
        * NewL method is used to create a new instance of class.
        * @param CMmPhoNetSender*: pointer to phonet sender
        * @param CMmPhoNetReceiver*: pointer to phonet receiver
        * @param CMmPacketContextMessHandler*: pointer to context message handler
        * @param CMmPacketContextMesshandlerList*: pointer to context list
        * @return CMmPipeControl*: pointer to the message handler
        */
        static CMmPipeControl* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmPacketContextMessHandler* aMmPacketContextMessHandler,
            CMmPacketContextMesshandlerList* aMmPacketContextMesshandlerList );

        /**
        * Destructor.
        */
        ~CMmPipeControl();

    public: // New classes

        // TPipeOperationInfo
        class TPipeOperationInfo
            {
            public:
                TUint8 iPipeHandle;
                TUint8 iTransId;
            };

    public: // Functions from base classes

        /**
        * Isimsg received by PhonetReceiver.
        * @param TIsiReceiveC, reference to the received message.
        */
        void ReceiveMessageL( const TIsiReceiveC& aIsiMessage );

    public: // New functions

        /**
        * Creates PnsPipeCreateReq ISI message and sends it to Phonet.
        * @param aTransId: transaction Id
        * @param aPipeState: pipe state
        * @param aFirstPepObject: First PEP object Id
        * @return error value: Symbian error code
        */
        TInt PnsPipeCreateReq(
            const TUint8 aTransId,
            const TUint8 aPipeState,
            const TUint8 aFirstPepObject ) const;

        /**
        * Creates PnsPipeEnableReq ISI message and sends it to Phonet.
        * @param aPipeHandle: pipe to be enabled
        * @param aTransId: transaction Id
        * @return error value: Symbian error code
        */
        TInt PnsPipeEnableReq(
            TUint8 aPipeHandle,
            TUint8 aTransId ) const;

        /**
        * Creates PnsPipeResetReq ISI message and sends it to Phonet.
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        * @param aStateAfterReset: pipe state after reset
        * @return error value: Symbian error code
        */
        TInt PnsPipeResetReq(
            const TUint8 aTransId,
            const TUint8 aPipeHandle,
            const TUint8 aStateAfterReset );

        /**
        * Creates PnsPipeRemoveReq ISI message and sends it to Phonet.
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        * @return error value: Symbian error code
        */
        TInt PnsPipeRemoveReq(
            const TUint8 aTransId,
            const TUint8 aPipeHandle );

        /**
        * Creates PnsPipeRedirectReq ISI message and sends it to Phonet.
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        * @param aOldPepDevId: Device ID to remove from
        * @param aOldPepObjId: Object ID to remove from
        * @param aReplacementPepDevId: Device ID to redirect to
        * @param aReplacementPepObjId: Object ID to redirect to
        * @return error value: Symbian error code
        */
        TInt PnsPipeRedirectReq(
            TUint8 aTransId,
            TUint8 aPipeHandle,
            TUint8 aOldPepDevId,
            TUint8 aOldPepObjId,
            TUint8 aReplacementPepDevId,
            TUint8 aReplacementPepObjId);

        /**
        * Creates PnsPepCtrlReq ISI message and sends it to Phonet.
        * @param aDevId: device Id
        * @param aObjId: object Id
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        * @param aControlId: control id
        * @param aStatus: status code
        * @return error value: Symbian error code
        */
        TInt PnsPepCtrlReq(
            TUint8 aDevId,
            TUint8 aObjId,
            TUint8 aTransId,
            TUint8 aPipeHandle,
            TUint8 aControlId,
            TUint8 aStatus,
            TUint8 aChannelId );

     public: // Functions from base classes
        // None

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /**
        * C++ default constructor.
        */
        CMmPipeControl();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * PnsPipeCreate response.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPipeCreateResp( const TIsiReceiveC &aIsiMessage );

        /**
        * PnsPipeEnable response.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPipeEnableResp( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPipeReset response.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPipeResetResp( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPipeRemove response.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPipeRemoveResp( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPipeRedirect response.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPipeRedirectResp( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPepCtrl response.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPepCtrlResp( const TIsiReceiveC& aIsiMessage );


        /**
        * Initialize lists.
        */
        void InitLists();

        /**
        * Set Pipe Handle.
        * @param aPipeHandle: pipe handle
        */
        void SetPipeHandle( const TUint8 aPipeHandle );

        /**
        * Set Transaction Id.
        * @param aPipeHandle: pipe handle
        * @param aTransId: transaction Id
        */
        void SetTransactionId(
            const TUint8 aPipeHandle,
            const TUint8 aTransId );

        /**
        * Return Transaction Id.
        * @param aPipeHandle: pipe handle
        * @return TUint8, Transaction Id
        */
        TUint8 TransactionId( const TUint8 aPipeHandle ) const;

        /**
        * Reset Pipe Handle.
        * @param aPipeHandle: pipe handle
        */
        void ResetPipeHandle( const TUint8 aPipeHandle );

        /**
        * If PipeHandle found returns True otherwise returns False.
        * @param aPipeHandle: pipe handle
        */
        TBool IsOperationCalled( const TUint8 aPipeHandle ) const;

#ifdef DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
        /**
        * PnsPepConnect request.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPepConnectReq( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPepDisconnect request.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPepDisconnectReq( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPepReset request.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPepResetReq( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPepEnable request.
        * @param aIsiMessage, reference to the received message.
        */
        void PnsPepEnableReq( const TIsiReceiveC& aIsiMessage );

        /**
        * PnsPepConnect response.
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        */
        void PnsPepConnectResp(
            const TUint8 aTransId,
            const TUint8 aPipeHandle );

        /**
        * PnsPepDisconnect response.
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        */
        void PnsPepDisconnectResp(
            const TUint8 aTransId,
            const TUint8 aPipeHandle );

        /**
        * PnsPepReset response.
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        */
        void PnsPepResetResp(
            const TUint8 aTransId,
            const TUint8 aPipeHandle );

        /**
        * PnsPepEnable response.
        * @param aTransId: transaction Id
        * @param aPipeHandle: pipe handle
        */
        void PnsPepEnableResp(
            const TUint8 aTransId,
            const TUint8 aPipeHandle );
#endif // DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS

    private: // Functions from base classes
        // None

    protected: // Data
        // None

    private: // Data

        // A pointer to the context messhandler.
        CMmPacketContextMessHandler* iContextMessHandler;

        // Pointer to contextlist
        CMmPacketContextMesshandlerList* iContextList;

        // A pointer to the PhonetSender.
        CMmPhoNetSender* iPhoNetSender;

        // Pipe Operation Array contains TPipeOperation elements.
        TPipeOperationInfo iPipeOperationArray[KMmMaxNumberOfPipeHandles];

        // Device Id of second PEP.
        TUint8 iSecondPepDeviceId;

        // Object Id of second PEP.
        TUint8 iSecondPepObjectId;

    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };

#endif // CMMPIPECONTROL_H  

// End of File
