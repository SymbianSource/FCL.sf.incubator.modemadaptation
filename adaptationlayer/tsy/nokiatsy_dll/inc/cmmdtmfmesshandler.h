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



#ifndef CMMDTMFMESSHANDLER_H
#define CMMDTMFMESSHANDLER_H

//  INCLUDES
#include <e32base.h>  // Symbian base types

#include <tisi.h>
#include "mmmmesshandlerbase.h"
#include "cmmphonetreceiver.h"

// CONSTANTS
const TUint16 KMaxDataBufferSize( 256 );

// MACROS
    //none

// DATA TYPES
    //none

//  EXTERNAL DATA STRUCTURES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class CMmPhoNetSender;
class CMmDataPackage;
class TIsiReceiveC;
class CMmMessageRouter;

// CLASS DECLARATION

/**
* Used for creating and sending DTMF ISI messages to
* PhoNet via PhoNetSender. It also receives DTMF
* ISI messages from PhoNetReceiver.
*  @lib nokiatsy.lib
*  @since Series60_2.6
*/
class CMmDtmfMessHandler
        : public CBase, public MMmMessHandlerBase,  public MMmMessageReceiver
    {
    public: // types

        /**
        * DTMF types used in Nokia OS
        */
        enum TNOSDtmfType
            {
            EDtmfTypeUnknown = 0, // unknown DTMF type
            EDtmfTypeDigit, // the currently processed DTMF is one digit
            EDtmfTypeString // the currently processed DTMF is a string
            };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aPhoNetSender pointer to the Phonet sender
        * @param aPhoNetReceiver pointer to thephonet receiver
        * @param aMessageRouter pointer to the message router
        * @return created message handler object
        */
        static CMmDtmfMessHandler* NewL(
            CMmPhoNetSender* aPhoNetSender,
            CMmPhoNetReceiver* aPhoNetReceiver,
            CMmMessageRouter* aMessageRouter );

        /**
        * Destructor.
        */
        ~CMmDtmfMessHandler();

    public: // Functions from base classes

        /**
        * From MMmMessHandlerBase. This method is the single entry point for
        * requests coming from the Symbian OS layer to this message handler
        * @param aIpc IPC number of the request
        * @return TInt KErrNone or error code
        */
        TInt ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage );

    public: // New functions

        /**
        * Handles a received message by calling the specific
        * message handling method.
        * @param TIsiReceiveC, reference to the received message.
        * @return void
        */
        void ReceiveMessageL( const TIsiReceiveC &aIsiMessage );

        /**
        * Handles errors comes from PhoNetReceiver RunError
        * @param const TIsiReceiveC &aIsiMessage: The received ISI message
        * @param TInt aError: Error code
        */
        void HandleError( const TIsiReceiveC &aIsiMessage, TInt aError );

        /**
        * Creates CallDtmfSendReq ISI message and sends it to Phonet.
        * @param TUint8 aTransactionId: unique TransactionId number
        * @param const TDesC* aDtmfDigit: DTMF digit or string to be sent
        * @param TNOSDtmfType aDtmfType: string/digit
        * @return TInt, success/failure value
        */
        TInt CallDtmfSendReqL( TUint8 aTransactionId,
            const TDesC* aDtmfDigit, TNOSDtmfType aDtmfType );

        /**
        * CMmDtmfMessHandler::SendPostAddressL
        * @param const TDesC* aDtmfString: DTMF character(s) to be sent
        */
        void CMmDtmfMessHandler::SendPostAddressL(
            const TDesC* aDtmfString //dtmf character(s) to be sent
            );

    private:

        /**
        * C++ default constructor.
        */
        CMmDtmfMessHandler();

        /**
        * 2nd phase constructor
        */
        void ConstructL();

        /**
        * Breaks received CallDtmfSendResp ISI message.
        * @param TIsiReceiveC, reference to the received message.
        * @return None
        */
        void CallDtmfSendResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates CallDtmfStopReq ISI message and sends it to Phonet.
        * @param TUint8 aTransactionId: unique TransactionId number
        * @return TInt, success/failure value
        */
        TInt CallDtmfStopReq( TUint8 aTransactionId );

        /**
        * Breaks received CallDtmfStopResp ISI message.
        * @param TIsiReceiveC, reference to the received message.
        * @return None
        */
        void CallDtmfStopResp( const TIsiReceiveC &aIsiMessage );

        /**
        * Breaks received CallDtmfStatusInd ISI message.
        * @param CIsiMsg, reference to the received message.
        * @return None
        */
        void CallDtmfStatusInd( const TIsiReceiveC &aIsiMessage );

        /**
        * Creates dtmf send request sub block
        * @param const TDesC* aDtmfString: DTMF string
        * @param const TIsiSendC &aIsiMessage: IsiMsg to be build
        * @param TInt aCurrentMsgOffset
        * @param TUint8 aNumOfSbInMessage
        * @return Indicates should the request be sent or not
        */
        TBool GetCallDtmfSendReqSubBlockL( const TDesC* aDtmfString,
                                          TIsiSend &aIsiMessage,
                                          TUint &aCurrentMsgOffset,
                                          TUint8 &aNumOfSbInMessage );

        /**
        * Handles the dtmf status indication message
        * CIsiMsg* aIsiMsg: The received ISI message
        * @param CIsiMsg* aIsiMsg , the received message.
        * @return None
        */
        void CallDtmfToneInd( const TIsiReceiveC &aIsiMessage );


    private:    // Data

        //Pointer to the PhonetSender
        CMmPhoNetSender* iPhoNetSender;

        //Pointer to the Message Router
        CMmMessageRouter* iMessageRouter;

        //To distinguish between string and digit
        TNOSDtmfType iDtmfType;

        //The Dtmf digit that is active at the moment
        TUint16 iDtmfDigitSent;

        //buffer for storing DTMF string after 'w' mark in the DTMF string
        TBuf<KMaxDataBufferSize> iDTMFStringAfterW;

        // Set if DTMF string includes character 'W'
        TBool iCharWFound;

        // True if DTMF send is already started
        TBool iDtmfSendOngoing;

    };

#endif // CMMDTMFMESSHANDLER_H

// End of File
