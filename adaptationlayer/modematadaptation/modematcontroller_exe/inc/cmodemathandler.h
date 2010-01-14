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



#ifndef CMODEMATHANDLER_H
#define CMODEMATHANDLER_H

#include <e32base.h>
#include "mmodematobserver.h"

const TInt KMaxDteIdCount(10);
const TInt KPluginCount(2); //atext & common plugin
const TInt KMaxNumOfTransactionIds(256);

// FORWARD DECLARATIONS
class CModemAtSrv;
class RIscApi;
class TIsiReceiveC;
class MMtiWrapperService;
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
    static CModemAtHandler* NewL( CModemAtSrv& aServer );

    /**
     * Destructor.
     */
    ~CModemAtHandler();
    
    /**
     * Sends AT-command to Modem AT-server
     * @param aDteId 
     * @param aPluginType AText/Common
     * @param aMessageType 0=normal, 1=getnvram status
     * @param aCmd AT-command
     */
    TInt SendATCommand(
        const TUint8 aDteId,
        const TATPluginInterface aPluginType,
        const TUint8 aMessageType,
        const TDesC8& aCmd );

    /**
      *  Connects Handler to modem with specific dteid.
      *  @param TUint8 aDteId
      *  @return KErrNone or
	  *  @return KErrNoMemory
      */

    TInt Connect( const TUint8 aDteId );

    /**
     *  Disconnects handler from modem
     *  @param TUint8 aDteId
     *  @return Disconnect status
     */
    TInt Disconnect( const  TUint8 aDteId );
    
    /**
     *  Check if disconnect is called directly
     *  @return ETrue if disconnect called directly
     */
    TBool IsDisconnected();
    
    /**
     *  HandleCommandModeChange
     *  @param TUint8 aDteId
     *  @param TCommandMode aMode
     */
    void HandleCommandModeChange( TInt aDteid, TCommandMode aMode );
    
    /**
     *  SendAtModemDataRedirectResultReq
     *  @param TUint8 aDteId
     *  @param TUint aResult
     */
    void SendAtModemDataRedirectResultReq( const TUint8 aDteId, const TUint aResult);

    /**
     *  SendEscapeSignalDetection
     *  @param TUint8 aDteId
     *  @return Symbian error code if error
     */
    TInt SendEscapeSignalDetection( const TUint8 aDteId );

    /**
     *  HandleSignalDetectedResp
     *  @param TIsiReceiveC aMessage
     */
    void HandleSignalDetectedResp( const TIsiReceiveC& aMessage );

    /**
     *  RemovePipe
     *  @param TUint8 aDteId
     */
    void RemovePipe( const TUint8 aDteId );

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

protected:
    // from CActive
    void RunL();
    void DoCancel();

private:
    /**
     * Default C++ constructor.
     */
    CModemAtHandler( CModemAtSrv& aServer );

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
    
    CModemAtPipeController* iPipeController;
#ifdef __WINSCW__
    TInt iEmulatedValue;
    TUint8 iDteId;
    TUint8 iChannel;
    TATPluginInterface iPluginType;
#endif
    TUint8 iLastTransactionId[KMaxNumOfTransactionIds];
    TBool iDisconnected;
    TBool iModemAtExistInCmt;
    };

#endif  // CMODEMATHANDLER_H

