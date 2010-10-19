/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMODEMATHANDLER_H
#define CMODEMATHANDLER_H

#include <e32base.h>
#include "mmodematobserver.h"

// FORWARD DECLARATIONS
class CModemAtSrv;
class RIscApi;
class TIsiReceiveC;
class CModemAtPipeController;


// CLASS DECLARATION

/**
*  CModemAtHandler
*  CModemAtHandler receives AT-commands from CModemATSession andsends AT-command to the AT-modem. 
*  CModemAtHandler communicates with AT modem by ISI-messages. Also pipemessages and name-service messages
*   are sent/received by CModemAtHandler. Pipe- and nameservice messages are forwarded to the CModemAtPipeController. 
*/

class CModemAtHandler : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * @param aServer
     * @return Instance of self
     */
    static CModemAtHandler* NewL( CModemAtSrv& aServer, TInt aChannel );

    /**
     * Destructor.
     */
    ~CModemAtHandler();
    
    /**
     * Sends AT-command to Modem AT-server
     * @param aPluginType AText/Common
     * @param aMessageType 0=normal, 1=getnvram status
     * @param aCmd AT-command
     */
    TInt SendATCommand(
        const TATPluginInterface aPluginType,
        const TUint8 aMessageType,
        const TDesC8& aCmd );

    /**
      *  Connects handler to modem
      *  @param aDteId DteId. Default is zero. If pipe is created, pipehandle will be used.
      *  @return KErrNone or
      *  @return KErrNoMemory
      */

    TInt Connect( TUint8 aDevId, TUint8 aObjId );

    /**
     *  Disconnects handler from modem
     *  @param TUint8 aDteId
     *  @return Disconnect status
     */
    TInt Disconnect();
    
    /**
     *  Check if disconnect is called directly
     *  @return ETrue if disconnect called directly
     */
    TBool IsDisconnected();
    
    /**
     *  HandleCommandModeChange
     *  @param TCommandMode aMode
     */
    void HandleCommandModeChange( TCommandMode aMode );
    
    /**
     *  SendAtModemDataRedirectResultReq
     *  @param TUint aResult
     */
    void SendAtModemDataRedirectResultReq( const TUint aResult );

    /**
     *  SendSignalDetection
     *  @param TUint8 aSignal
     *  @return Symbian error code if error
     */
    TInt SendSignalDetection( const TUint8 aSignal );

    /**
     *  HandleSignalDetectedResp
     *  @param TIsiReceiveC aMessage
     */
    void HandleSignalDetectedResp( const TIsiReceiveC& aMessage );

    /**
     *  RemovePipe
     */
    void RemovePipe();

    /**
     *  IsPluginConnected
     *  @return ETrue if plugin is connected
     */
    TBool IsPluginConnected();

    /**
     *  SetModemAtExistsInCmt
     *  @param TBool aModemAtExistsInCmt
     */
    void SetModemAtExistsInCmt( TBool aModemAtExistsInCmt );
    
    /**
     *  SendAtModemConnectReq
     */
    TInt SendAtModemConnectReq();

    /**
     *  Check if pipe is in data mode
     *  @return ETrue, if pipe is in datamode
     */
    TBool IsPipeInDataMode();

    /**
     *  Check if text message mode is on
     *  @return ETrue, if +CMGS or +CMGS command has been received
     */
    TBool IsTextMessageMode();

    /**
     *  Set text message mode. Modem indications will be ignored during this mode.
     *  @param TBool aTextMode
     */
    void SetTextMessageMode( TBool aTextMode );

    /**
     *  Pipe creation result and pipe handle is informed 
     *  with this function
     *  @param TUint8 aPipeHandle
     *  @param TInt aError
     */
    void PipeCreated( TUint8 aPipeHandle, TInt aError );

     /**
     *  Stores device info so that it's available when
     *  plugin for the device is created 
     *  @param TUint8 aDevId
     *  @param TUint8 aObjId
     */
    void AddDevice( TUint8 aDevId, TUint8 aObjId );

     /**
     *  Remove device info
     *  @param TUint8 aDevId
     *  @param TUint8 aObjId
     */
    void RemoveDevice( TUint8 aDevId, TUint8 aObjId );

    /**
     *  Get Dataport object id
     *  @param TUint8& aObjId
     */
    void GetDataportObjectId( TUint& aObjId );
    
    /**
     *  Get secondary dataport object id (e.g. if usb and bluetooth connect without PNS_NAME_ADD_IND)
     *  @param TUint8& aObjId
     */
    void GetSecondaryDataportObjectId( TUint& aObjId );

    /**
     *  Get ISC channel id used by handler
     *  @return TUint8 channel id
     */
    TInt GetChannelId();


protected:
    // from CActive
    void RunL();
    void DoCancel();

private:
    /**
     * Default C++ constructor.
     */
    CModemAtHandler( CModemAtSrv& aServer, TInt aChannel );

    /**
     *  2nd phase constructor
     */
    void ConstructL();
    /**
     *  Open ISI channel
     */
    void OpenChannelL();

    /**
     *  ISI message handlers
     */
    void HandleATResponse(const TIsiReceiveC& aMessage);
#ifdef __WINSCW__
    void HandleATResponse( );
#endif
    void HandleModemConnectResp( const TIsiReceiveC& aMessage);
    void HandleSignalInd(const TIsiReceiveC& aMessage);
    void HandleUnsolicitedData(const TIsiReceiveC& aMessage);
    void HandleIntermediateDataInd(const TIsiReceiveC& aMessage);
    void HandleRedirectInd(const TIsiReceiveC& aMessage);
    void HandleRedirectResultResp(const TIsiReceiveC& aMessage);
    void SetDisconnected( TBool aIsDisconnected );

private:
    //data
    CModemAtSrv& iServer;

    RIscApi* iIscApi;
    HBufC8* iReceivedMessageBuffer;
    TUint16 iMessageBufferSize;
    TPtr8 iReceivePtr;
        
    TUint16 iReceiveMessageLength;
    TIsiReceiveC*  iReceivedMessage;
    
    TUint iPepObjId;    //controller pep
    TUint iDataportObjId;

    CModemAtPipeController* iPipeController;
#ifdef __WINSCW__
    TInt iEmulatedValue;
    TATPluginInterface iPluginType;
#endif
    TUint8 iLastTransactionId;
    TBool iDisconnected;
    TBool iModemAtExistInCmt;
    TBool iTextMessageMode;
    TUint8 iDteId;
    TInt iChannel;
    };

#endif  // CMODEMATHANDLER_H

