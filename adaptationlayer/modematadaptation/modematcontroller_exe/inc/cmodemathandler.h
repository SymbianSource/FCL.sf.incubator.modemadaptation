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
    TBool IsDisconnected();
    void HandleCommandModeChange( TInt aDteid, TCommandMode aMode );
    
    void SendAtModemDataRedirectResultReq( const TUint8 aDteId, const TUint aResult);
    TInt SendEscapeSignalDetection( const TUint8 aDteId );
    void HandleSignalDetectedResp( const TIsiReceiveC& aMessage );
    void RemovePipe( const TUint8 aDteId );
    TBool IsPluginConnected();

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
 
    void OpenChannelL();
    void HandleATResponse(const TIsiReceiveC& aMessage);

#ifdef __WINSCW__
    void HandleATResponse( );
#endif
    void HandleModemConnectResp( const TIsiReceiveC& aMessage);
    void HandleSignalInd(const TIsiReceiveC& aMessage);
    void HandleUnsolicitedData(const TIsiReceiveC& aMessage);

    TInt GetMinSizeDividendByFour( TInt aSize);
   
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
    
    };

#endif  // CMODEMATHANDLER_H

