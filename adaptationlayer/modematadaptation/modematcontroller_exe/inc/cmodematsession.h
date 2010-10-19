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



#ifndef CMODEMATSESSION_H
#define CMODEMATSESSION_H

#include <e32svr.h>

class CModemAtSrv;
class RModemAtController;

// CLASS DECLARATION

/**
*  CModemAtSession receives and completes client's requests. CModemAtSession forwards
*  requests to the CModemAtSrv.
*/

NONSHARABLE_CLASS( CModemAtSession ) : public CSession2
    {
    
public:

    /**
     * Two-phased constructor.
     * @param aServer
     * @return Instance of self
     */
    static CModemAtSession* NewL( CModemAtSrv& aServer, const TVersion& aVersion );

   /**
    * Destructor.
    */
    ~CModemAtSession();

    /**
     *  Called by CModemAtHandler, when Unsolicitated data ind is received  
     */
    void UnsolicitedData( const TDesC8& aData );

    /**
     *  Called by CModemAtHandler, when signal ind is received  
     * @param aCompleteCode
     */
    void SignalIndReceived( const TInt aCompleteCode );

    /**
     * Returns DteId of current session
     *  @return DteID 
     */    
    TUint8 GetDteId();

    /**
      *  Sets DteID for this session
      *  @param aDteId DteId
      */    
    void SetDteId( const TUint8 aDteId );

    /**
     * Returns name of current session  
     * @return Session name 
     */    
    TDesC8& GetName();

    /**
     * Returns plugin type (common/atext) 
     * @return Plugintype 
     */    
    TATPluginInterface GetPluginType();

    /**
     * Modem-Connected-callback. Called when modem is connected.
     * @param aDteId DteId
     * @param aErr Error code
     * @return void
     */
    void ModemConnected( const TInt aErr );
   
    /**
     *  CommandModeChanged. Called when pipe handle indicates that mode is changed.
     *  @param aMode TCommandMode aMode
     *  @return void
     */
    void CommandModeChanged( TCommandMode aMode ) ;
    
    /**
     *  IsUnsolicitedDataReqActive
     *  @return ETrue if request is active, otherwise EFalse
     */
    TBool IsUnsolicitedDataReqActive();

    /**
     *  IsSignalIndReqActive
     *  @return ETrue if request is active, otherwise EFalse
     */
    TBool IsSignalIndReqActive();

    /**
     *  IsCommandModeReqActive
     *  @return ETrue if request is active, otherwise EFalse
     */
    TBool IsCommandModeReqActive();

    /**
     *  IsConnectReqActive
     *  @return ETrue if request is active, otherwise EFalse
     */
    TBool IsConnectReqActive();

     /**
     *  sets handler for this session
     *  @param aHandler pointer to handler, which handles messages 
     *                  for this session
     *  @return void
     */
    void SetHandler( CModemAtHandler* aHandler );
    
    /**
     *  gets handler for this session
     *  @return pointer to handler, which handles messages for this session
     */
    CModemAtHandler* GetHandler();

private:

    // from base class CSession2    
    void ServiceL( const RMessage2& aMessage );
    void DoServiceL( const RMessage2& aMessage );

    /**
     *  Checks if there is existing request and completes it.
     *  @param aMessage Message to be checked.
     *  @return ETrue if there was request already pending. 
     */
	  TBool CheckAndCompleteExistingRequest( const RMessage2& aMessage );

       
private:  

    /**
     * Default C++ constructor.
     * @param aServer   Reference to the CModemAtsrv
     * @param aVersion  Version of client
     * @param aHandler  Reference to the CModemAtHandler
     */
    CModemAtSession( CModemAtSrv& aServer, const TVersion& aVersion );

    // data
    CModemAtSrv& iServer;
    RMessage2 iConnectReq;
    RMessage2 iSignalIndReq;
    RMessage2 iUnsolicitedDataReq;
    RMessage2 iCommandModeReq;

    TVersion iVersion;
    TUint8 iDteId;
    HBufC8* iClientName;
    TATPluginInterface iPluginType;
    TPtr8 iClientNamePtr;
    TBuf8<KMaxBufferLength> iTemporaryUnsolicitedData;
    CModemAtHandler* iHandler;
    };

#endif  // CMODEMATSESSION_H


