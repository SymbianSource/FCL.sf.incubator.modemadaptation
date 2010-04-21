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



#ifndef CMODEMATSRV_H
#define CMODEMATSRV_H

#include <e32svr.h>
#include "cmodemathandler.h"  //KMaxDteIdCount
#include "modemattrace.h"

const TUint8 EAtCmd = 0;
const TUint8 EGetNvramStatus = 1;

/*
 * Reference: i_at_modem_fp_3.1.doc (3.5.2)
 * If the unsolicited data needs to be sent to all of the connected DTE sessions,
 * AT Modem server would send the unsolicited data indication to the client with
 * the dte id as 0xFF.  The 0xFF value for the DTE id is reserved for the
 * unsolicited data indication.
*/
const TUint8 KUnsolicitedDataDteId = 0xff;  

const TInt KATModemCommandArgumentIndex = 0;
const TInt KATModemResponseArgumentIndex = 1;
const TInt KATModemUnsolicitedDataIndex = 0;

_LIT8( KEmptyString, "");

class CModemAtSession;
class CAtMessage;

/**
*  CModemAtSrv server core class. CModemAtsrc creates/deletes sessions. Dte-ids are allocated
*  by CmodemAtSrv.
*/
NONSHARABLE_CLASS( CModemAtSrv ) : public CPolicyServer
    {

public:

    /**
     * Two-phased constructor.
     */
    static CModemAtSrv* NewLC();

    /**
     * Destructor.
     */
    ~CModemAtSrv();

    /**
     * When session is closed, this function is called.   
     * @param aSession Reference to the session
     */    
    void ClientClosed( CModemAtSession* aSession );
    
    /**
     * Session count  
     * @return Number of sessions open
     */ 
    TInt SessionCount();

    /**
     * Connect session to the modem  
     * @param CModemAtSession* aSession Pointer to the session
     * @param TATPluginInterface aPluginType Atext or common plugin
     * @return Connection status: KErrNone, KErrAlreadyExists
     */    
    TInt ConnectToModem( CModemAtSession* aSession, TATPluginInterface aPluginType);

    /**
     * HandleSignalInd  
     * @param aDteId Device Terminal ID
     */ 
    void HandleSignalInd( const TUint8 aDteId );
    /**
     * HandleUnsolicitedData 
     * @param aDteId Device Terminal ID
     * @param aData Descriptor data
     */ 
    void HandleUnsolicitedData( const TUint8 aDteId, const TDesC8& aData );
    /**
     * HandleIntermediateDataInd
     * @param aDteId Device Terminal ID
     * @param aPluginType Plugin type
     * @param aResponse Response buffer
     * @param aCommand Command buffer
     */ 
    void HandleIntermediateDataInd( const TUint8 aDteId, const TATPluginInterface aPluginType, const TDesC8& aResponse, const TUint8 aCommand );

    /**
     * HandleATResponse
     * @param aDteId Device Terminal ID
     * @param aResponse Response buffer
     */ 
    void HandleATResponse( const TUint8 aDteId, const TDesC8& aResponse, const TUint8 aCommand );
    
    /**
     * HandleCommandModeChange
     * @param aMode Command mode 0 or data mode 1
     */ 
    void HandleCommandModeChange( TCommandMode aMode );
    
    /**
     * AddToSendFifo
     * @param aPluginType Plugin type
     * @param aMessage RMessage2 container class
     */ 
    void AddToSendFifo( const TATPluginInterface aPluginType, CAtMessage* aMessage );

    /**
     * RemoveFirstFromSendFifo 
     */ 
    void RemoveFirstFromSendFifo();

    /**
     * SendNextFromFifo  
     */ 
    void SendNextFromFifo();
    
    /**
     * CancelFifoRequests
     * @param aSession Session class pointer
     */ 
    void CancelFifoRequests( const CModemAtSession* aSession );
    
    /**
     * RemovePipe
     */ 
    void RemovePipe();

    /**
      *  Sets DteID for this session
      *  @param aDteId DteId
      * @param aConnectionError error
      */    
    void SetDteIdAndConnect( const TUint8 aDteId, const TInt aConnectionError );

private:

    /**
     * Creates a new session to client
     *
     * @param aVersion the version
     * @param aMessage not used
     * @return The new created session
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

    /**
     * Constructor
     *
     * @param aPriority This AO's priority
     */
    CModemAtSrv( TInt aPriority );

    /**
     * Two-Phase constructor
     */
    void ConstructL();
    

private:  // data

    RPointerArray<CModemAtSession> iSessions;       //Sessions from R-interface
    RPointerArray<CAtMessage> iAtMessageArray;
    CModemAtHandler* iHandler;
    TUint8 iDteId;
    };

#endif  // CMODEMATSRV_H

