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


#ifndef CMODEMATPIPECONTROLLER_H
#define CMODEMATPIPECONTROLLER_H

#include <e32base.h>
#include "cmodemathandler.h" //KMaxDteIdCount


class TPipeInfo
{
public:

    enum EPipeState 
        {
        EPipeCreated = 0,
        EPipeRedirected,
        EPipeEnabled,
        EPipeDisabled,
        EPipeRemoving,
        EPipeNoPipe
        };
        
    TInt iHandle;
    TUint8 iFirstObjId;
    TUint8 iFirstDevId;
    TUint8 iSecondObjId;
    TUint8 iSecondDevId;
    EPipeState iPipeState;
};

/**
*   CModemAtPipeController receives ISI-messages from CModemAtHandler. CModemAtPipeController creates, 
*   redirects and removes pipes.  
*/


class CModemAtPipeController : public CBase 
    {
public:
    static CModemAtPipeController* NewL( RIscApi& aIscApi, TUint& aObjId, CModemAtHandler& aHandler );

    ~CModemAtPipeController();
#ifndef NCP_COMMON_BRIDGE_FAMILY
    /*
     * Handles messages from PN_NAME
     * @param TIsiReceiveC& aReceivedMessage Message data
     */
    void HandleNameServiceMessage( const TIsiReceiveC& aReceivedMessage );
    
    /*
     * Handles messages from PN_
     * @param TIsiReceiveC& aReceivedMessage Message data
     */
        
    void HandlePipeServiceMessage( const TIsiReceiveC& aReceivedMessage );
    
    /**
     * Removes pipe.
     * @param TUint8 aDteId Connection DteId   
     */
    void RemovePipe( const TUint8 aDteId);
    
    /**
      * Redirects pipe
      * @param TUint8 aDteId Connection DteId   
      * @param TUint8 aNewDevId New PEP
      * @param TUint8 aNewObjId New PEP
      */
    
    void RedirectPipe( const TUint8 aDteId, const TUint8 aNewDevId, const TUint8 aNewObjId );

    /**
      * Links dteId to created Pipe
      * @param TUint8 aDteId Connection DteId   
      */
    void LinkDteIdToPipe( const TUint8 aDteId );
    
    /**
     * Queries AT MODEM object id. 
     */
    void SendTaskIdQuery();
#endif
private:
    /**
     * Default C++ constructor.
     * @param None
     * @return
     */
    CModemAtPipeController( RIscApi& aIscApi, TUint& aObjId, CModemAtHandler& aHandler );

    /**
     *  Used to send At command to modem
     *  @param
     *  @return Send status
     */

    void ConstructL();

#ifndef NCP_COMMON_BRIDGE_FAMILY
    /**
     * Handles MatchDataportName 
     * @param TUint aName
     */
    TUint MatchDataportName( TUint aName );

    /**
     * Handles PNS_NAME_ADD_IND 
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandleNameAddInd( const TIsiReceiveC& aReceivedMesssage );
   
    /**
     * Handles PNS_NAME_REMOVE_IND 
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandleNameRemoveInd( const TIsiReceiveC& aReceivedMesssage );

    /**
     * Handles PNS_NAME_QUERY_RESP 
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandleNameQueryResp( const TIsiReceiveC& aReceivedMesssage );


    /**
     * Handles PNS_PIPE_CREATE_RESP 
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandlePipeCreateResp( const TIsiReceiveC& aReceivedMessage );

    /**
     * Handles PNS_PIPE_REDIRECT_RESP 
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandlePipeRedirectResp( const TIsiReceiveC& aReceivedMessage );
    
    /**
     * Handles PNS_PIPE_REMOVE_RESP 
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandlePipeRemoveResp( const TIsiReceiveC& aReceivedMessage );
    
    /**
     * Handles PNS_PIPE_ENABLE_RESP 
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandlePipeEnabledResp( const TIsiReceiveC& aReceivedMessage );

    /**
     * Handles PNS_PEP_STATUS_IND
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandlePepStatusInd( const TIsiReceiveC& aReceivedMessage );
 
    /**
     * Sends PNS_PIPE_CREATE_REQ
     * @param TUint8 aDevId New PEP
     * @param TUint8 aObjId New PEP
     */
    void SendCreatePipeMessage(const TUint8 aDevId, const TUint8 aObjId);
    
    /**
     * Sends PNS_PIPE_REMOVE_REQ
     * @param TUint8 aPipeHandle Pipehandle
     */
    void SendRemovePipeReq( const TUint8 aPipeHandle );
    
    /**
     * Sends PNS_PIPE_ENABLE_REQ
     * @param TUint8 aPipeHandle Pipehandle
     */
    void SendEnablePipeReq( const TUint8 aPipeHandle);
    /**
     * Finds dteid of pipe
     * @param const TInt aHandle pipe handle
     */
    TInt FindDteId( const TInt aHandle );

    /**
     * ChangePipeState
     * @param const TInt aDteId
     * @param TPipeInfo::EPipeState aState
     */
    void ChangePipeState( const TInt aDteId, TPipeInfo::EPipeState aState );
#endif
private:
    
    RIscApi& iIscApi;
    TUint iModemAtObjId;            //Modem AT Controller Obj-id
    TUint iModemAtDevId;            //Modem AT Controller device-id
    TInt  iPipeHandle;              //handle of created pipe 
    TUint iDataportDevId;           //Dataport device-id
    TUint iDataportObjId;           //Dataport obj-id
    TUint iCellmoDevId;             //AT Modem device-id
    TUint iCellmoObjId;             //AT Modem obj-id
    TPipeInfo iPipeTable[KMaxDteIdCount];   //created pipes
    CModemAtHandler& iAtHandler;    //forwards AT-commands to the AT MODEM
    CActiveSchedulerWait* iSchedulerWait;
    TUint8 iDteId;                  // temporary dte id for creating the connection
    };

#endif  // CMODEMATPIPECONTROLLER_H

