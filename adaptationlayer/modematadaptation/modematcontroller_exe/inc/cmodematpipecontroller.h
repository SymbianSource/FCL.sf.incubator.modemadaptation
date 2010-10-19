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


#ifndef CMODEMATPIPECONTROLLER_H
#define CMODEMATPIPECONTROLLER_H

#include <e32base.h>
#include "cmodemathandler.h"
const TUint8 KInvalidPipeHandle = 0xff;


const TUint8 PEP_COMM_IND_ID_ESCAPE = 0x07;// Hard coded PEP constants to be included when dataport exports (DP is COMM PEP)!!
const TUint8 PEP_COMM_CTRL_ID_ESCAPE = 0x05;
const TUint8 PEP_COMM_SIGNAL_DTR_ON = 0x05;
const TUint8 PEP_COMM_SIGNAL_DTR_OFF = 0x06;
const TUint8 PEP_COMM_SIGNAL_DCD_ON = 0x07;
const TUint8 PEP_COMM_SIGNAL_DCD_OFF = 0x08;
const TUint8 PEP_COMM_SIGNAL_DSR_ON = 0x01;
const TUint8 PEP_COMM_SIGNAL_DSR_OFF = 0x02;
const TUint8 PEP_COMM_IND_ID_V24_CHANGE = 0x04;
const TUint8 PEP_COMM_FLOW_CTRL_SW = 0x01;
const TUint8 PEP_COMM_FLOW_CTRL_HW = 0x02;
// const TUint8 PEP_COMM_FLOW_CTRL_BOTH = 0x03;  // Note: AT_MODEM_FLOW_CONTROL_BOTH is not supported
const TUint8 PEP_COMM_FLOW_CTRL_NONE = 0x04;
const TUint8 PEP_COMM_FLOW_CTRL_EXTRACT = 0x05;
const TUint8 PEP_COMM_FLOW_CTRL_NO_EXTRACT = 0x06;
const TUint8 PEP_COMM_CTRL_ID_FLOW_CTRL = 0x02;
// Escape time for PEP_CTRL_REQ with PEP_COMM_CTRL_ID_ESCAPE
// Time is given as 20 milliseconds. So real escape time is:
// 50 * 20 milliseconds = 1000 milliseconds
const TUint8 KEscapeTime = 50; 

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

    TUint8 pipeHandle;
    TUint8 firstObjId;
    TUint8 firstDevId;
    TUint8 secondObjId;
    TUint8 secondDevId;
    EPipeState pipeState;
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
     */
    void RemovePipe();
    
    /**
     * Redirects pipe
     * @param TUint8 aDteId Connection DteId   
     * @param TUint8 aNewDevId New PEP
     * @param TUint8 aNewObjId New PEP
     */
    void RedirectPipe( const TUint8 aNewDevId, const TUint8 aNewObjId );

    /**
     * Queries AT MODEM object id. 
     */
    void QueryModemAtFromNameService();

    /**
     * GetPipeState
     * @return pipe state
     */
    TPipeInfo::EPipeState GetPipeState();

    /**
     * Sends PNS_PIPE_ENABLE_REQ
     */
    void SendEnablePipeReq();

   /**
    * Sends PNS_PEP_CTRL_REQ to notify dataport
    * @param TUint8 aControlId
    * @param TDesC8& aData
    */
    void SendPepCtrlReq( const TUint8 aControlId, TDesC8& aData );

    /**
     * Sends PNS_PIPE_CREATE_REQ
     * @param TUint8 aDevId New PEP
     * @param TUint8 aObjId New PEP
     */
    void SendCreatePipeMessage(const TUint8 aDevId, const TUint8 aObjId);
    
    /**
     * Sends PNS_PEP_STATUS_IND + PEP_COMM_IND_ID_V24_CHANGE
     * @param TUint8 aPepCommSignal DCD or DSR signal ON or OFF
	 *
	 *
     */
    void SendPepStatusIndSignal( const TUint8 aPepCommSignal );

private:
    /**
     * Default C++ constructor.
     * @param RIscApi& aIscApi
     * @param TUint& aObjId
     * @param CModemAtHandler& aHandler
     */
    CModemAtPipeController( RIscApi& aIscApi, TUint& aObjId, CModemAtHandler& aHandler );

    /**
     *  ConstructL
     */

    void ConstructL();

    /**
     * Handles MatchDataportName 
     * @param TUint aName
     */
    TBool MatchDataportName( TUint aName );

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
     * Sends PNS_PIPE_REMOVE_REQ
     * @param TUint8 aPipeHandle Pipehandle
     */
    void SendRemovePipeReq( const TUint8 aPipeHandle );
    

    /**
     * ChangePipeState
     * @param TPipeInfo::EPipeState aState
     */
    void ChangePipeState( TPipeInfo::EPipeState aState );
    
    /**
     * HandlePepCtrlResp
     * @param TIsiReceiveC& aReceivedMessage Message data 
     */
    void HandlePepCtrlResp( const TIsiReceiveC& aReceivedMessage );

private:
    
    RIscApi& iIscApi;
    TUint iModemAtObjId;            //Modem AT Controller Obj-id
    TUint iModemAtDevId;            //Modem AT Controller device-id
    TUint iDpDevId;             //Dataport device-id
    TUint iDpObjId;             //Dataport obj-id
    TPipeInfo iPipe;            //Pipehandle, pipe status and pipe id's
    CModemAtHandler& iAtHandler;    //forwards AT-commands to the AT MODEM
    CActiveSchedulerWait* iSchedulerWait;
    };

#endif  // CMODEMATPIPECONTROLLER_H

