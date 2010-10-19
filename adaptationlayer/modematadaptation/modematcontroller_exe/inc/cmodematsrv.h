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
const TUint8 KMaxDteCount( 3 );

/*
 * Reference: i_at_modem_fp_3.1.doc (3.5.2)
 * If the unsolicited data needs to be sent to all of the connected DTE sessions,
 * AT Modem server would send the unsolicited data indication to the client with
 * the dte id as 0xFF.  The 0xFF value for the DTE id is reserved for the
 * unsolicited data indication.
*/
const TUint8 KUnsolicitedDataDteId = 0xff;  
const TUint8 KInitialValue = 0xff;
const TInt KATModemCommandArgumentIndex = 0;
const TInt KATModemResponseArgumentIndex = 1;
const TInt KATModemUnsolicitedDataIndex = 0;

_LIT8( KEmptyString, "");

#define KMaxBufferLength 1024

class CModemAtSession;
class CAtMessage;


class TDteInfo
{
public:

    TUint8 iDevId;
    TUint8 iObjId;
    CModemAtSession* iSession;
};


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
    TInt ConnectToModemL( CModemAtSession* aSession, TATPluginInterface aPluginType);

    /**
     * HandleSignalInd handles AT_MODEM_SIGNAL_IND and notifying ATV, ATQ and ATE changes.
     * @param aDteId Device Terminal ID
     * @param aCompleteCode Change in V, Q or E is completed to common plugin.
     */
    void HandleSignalInd( const TUint8 aDteId, const TInt aCompleteCode );
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
    void HandleATResponse( 
        const TUint8 aDteId, 
        CModemAtHandler* aHandler,
        const TDesC8& aResponse, 
        const TUint8 aCommand );
    
    /**
     * HandleCommandModeChange
     * @param aDteId dte id which mode is changed
     * @param aMode Command mode 0 or data mode 1
     */ 
    void HandleCommandModeChange( TUint8 aDteId, TCommandMode aMode );
    
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
     * @param aHandler pointer to message handler
     */ 
    void RemovePipe( CModemAtHandler* aHandler );

    /**
     *  Sets DteID for this session
     *  @param aDteId DteId
     *  @param aConnectionError error
     */ 
    void SetDteIdAndConnect( const TUint8 aDteId, const TInt aConnectionError );

    /**
     *  Check if pipe is in data mode
     *  @param CModemAtHandler* aHandler
     *  @return True, if pipe is in datamode
     */
    TBool IsPipeInDataMode( CModemAtHandler* aHandler );

    /**
     *  Check if text message mode is on
     *  @param aHandler pointer to message handler
     *  @return ETrue, if +CMGS or +CMGS command has been received
     */
    TBool IsTextMessageMode( CModemAtHandler* aHandler );

    /**
     *  Peek if AT command is for sending text messages or if it's reply
     *  needs to be notified to ATEXT. This will reduce ISI message overhead, otherwise
     *  AT&V command would be needed every time when V, Q or E setting is changed.
     *  @param CModemAtHandler* aHandler
     *  @param TDes8& aCmd
     */
    void PeekAtCommand( CModemAtHandler* aHandler, TDes8& aCmd );

    /**
     *  Check if echo has been notified to common plug-in.
     *  @return ETrue or EFalse.
     */
    TBool IsEchoNotified( const TUint8 aEchoValue );

    /**
     *  Check if V, Q or E command is ongoing, to handle AT_MODEM_SIGNAL_IND correctly.
     *  @return ETrue or EFalse.
     */
    TBool IsSettingQueryOngoing();

     /**
     *  Check if there is unsolicited data available in buffer, before there are active requests.
     *  @return ETrue or EFalse.
     */
    TBool IsUnsolicitedDataInBuffer();

    /**
     *  Get unsolicited data available in buffer, before there are active requests.
     *  @return ETrue or EFalse.
     */
    void GetUnsolicitedDataInBuffer( TDes8& aData );

    /**
     *  Reset unsolicited data in temporary buffer.
     */    
    void ResetUnsolicitedDataInBuffer();

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
     *  calculates number of different DTE's connected
     *  @return number of dte's conntected
     */
    TUint8 GetDteCount();

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

    /**
     * Gets free ISC channel id
     *
     * @param aChannel free channel id is inserted to this param
     * @return TInt KErrNone if free channel found, otherwise KErrNotFound
     */
    TInt GetFreeChannelId( TUint8& aChannel );
    

private:  // data

    RPointerArray<CModemAtSession> iSessions;       //Sessions from R-interface
    RPointerArray<CAtMessage> iAtMessageArray;
    CModemAtHandler* iHandler;
    TUint8 iDteId;

    TUint8 iEcho;
    TUint8 iEchoNotified;
    TBool  iEchoQueryIsOngoing;

    TUint8 iVerbose;
    TUint8 iVerboseNotified;
    TBool  iVerboseQueryIsOngoing;

    TUint8 iQuiet;
    TUint8 iQuietNotified;
    TBool  iQuietQueryIsOngoing;
    
    TUint8 iCarriageReturn;
    TBool  iCarriageReturnFound;

    TBuf8<KMaxBufferLength> iTemporaryUnsolicitedData;
    RArray<TDteInfo> iDteInfo;
    TBuf8<KMaxDteCount> iFreeChannels;
  
    };

#endif  // CMODEMATSRV_H

