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



#ifndef RMODEMATCONTROLLER_H
#define RMODEMATCONTROLLER_H

#include <e32base.h>
#include "mmodematobserver.h"

class CAtCommandReq;
class CUnsolicitedDataReq;
class CSignalIndReq;
class CIntermediateDataIndReq;
class CCommandModeReq;

/**
*  RModemAtController
*  This class implements the RModemAtController
*/

NONSHARABLE_CLASS( RModemAtController ) : public RSessionBase 
    {
public:
    /**
     * Default C++ constructor.
     * @param None
     * @return 
     */
    IMPORT_C RModemAtController();
    
    /**
     * Destructor.
     */
    IMPORT_C ~RModemAtController();

    /** 
     *  Connects client to the Modem AT Controller
     *  @param TATPluginInterface aInterface
     *  @param TDesC8& aClientNam
     *  @param MAtControllerObserver* aObserver
     *  @return error from ModemAtController
     */
    
    IMPORT_C TInt Connect( TATPluginInterface aInterface, 
                  TDesC8& aClientName,
                  MAtControllerObserver* aObserver);

    /**
     *  Closes connection to the Modem AT Controller  
     *  @return error status
     */
    IMPORT_C TInt Disconnect();


    /**
     *  Handle AT command
     *  @param aCommand     Reference to the AT-command
     *  @param aResponse    Reference to the response descriptor 
     *  @return Error status
     */
    IMPORT_C TInt HandleATCommand( const TDesC8& aCommand, TDes8& aResponse );

    /**
     *  Cancels Handle AT-command request   
     *  @return KErrNone, if no error 
     */
    IMPORT_C TInt HandleATCommandCancel();
    
    /**
     *  Starts listen unsolicited data
     *  @param aResult Unsolicitedresult
     *  @return KErrNone, if no error
     */
    IMPORT_C TInt ReceiveUnsolicitedResult( TDes8& aResult );

    /**
     *  Cancels receive unsolicited result request. 
     *  @return KErrNone 
     */
    IMPORT_C TInt ReceiveUnsolicitedResultCancel();

    /**
     *  Starts listen Signal indications
     *  @return KErrNone 
     */
    IMPORT_C TInt ReceiveSignalInd( );
   
    /**
     *  Cancels receive signal indications.
     *  @return KErrNone 
     */
    IMPORT_C TInt ReceiveSignalIndCancel( );

    /**
     *  Gets Nvram status from modem
     *  @param aCommand Command to be executed (use "AT&v")
     *  @param aResponse Current status 
     *  @return KErrNone
     */
    IMPORT_C TInt GetNvramStatus( TDesC8& aCommand, TDes8& aResponse );

    /**
     *  Handle command mode
     *  @param aResponse    Reference to the response descriptor 
     *  @return Error status
     */

    IMPORT_C TInt GetCommandMode( );

    /**
     *  Cancels Handle command mode request   
     *  @return KErrNone, if no error 
     */
    IMPORT_C TInt GetCommandModeCancel();

    /**
     *  Remove pipe from AT command handler
     *  @return KErrNone
     */
    IMPORT_C TInt RemovePipe();

    /**
     *  Executes SendReceive-command 
     *  @param aCommand
     *  @param aResponse
     *  @param aStatus
     */
    void SendReceiveATCommand( const TDesC8& aCommand, TDes8& aResponse,
         TRequestStatus& aStatus); 

    /**
     *  Executes SendReceive-command 
     */
    void SendReceiveAtCommandCancel();

    /**
     *  Executes SendReceive-command 
     *  @param aResponse
     *  @param aStatus
     */
    void SendReceiveUnsolicitedResult( TDes8& aResponse,
         TRequestStatus& aStatus); 

    /**
     *  Executes SendReceive-command 
     */
      
    void SendReceiveUnsolicitedResulCancel();

    /**
     *  Executes SendReceive-command 
     *  @param aStatus
     */
    void SendReceiveSignalInd(TRequestStatus& aStatus); //for signal indications
    void SendReceiveSignalIndCancel();

    
    //responses from server, called by request-classes
    
    /**
     *  Called by CAtCommandReq, when response is received from server   
     *  @param aErr, Error status 
     */
    void ATCommandResponseReceived(TInt aErr);

    /**
     *  Called by CUnsolicitedDataReq, when response is received from server
     *  @param aErr, Error status 
     */
        
    void UnsolicitedResultReceived(TInt aErr);
    
    /**
     *  Called by SignalIndReq, when indication is received from server
     *  @param aErr, Error status
     *  @param aMode, See atextcommonbase.h for values
     */
    void SignalIndReceived( TInt aErr );
    
    /**
     * Increases active request count 
     */
    void RequestCountIncreace();
    
    /**
     * Decreases active request count  
     */
    void RequestCountDecreace();
    /**
     *  Executes SendReceive for command mode
     *  @param aResponse
     *  @param aStatus
     */
    
    void SendReceiveCommandMode( TDes8& aResponse, TRequestStatus& aStatus ); 

    /**
     * Cancels Commandmode request
     */
    void SendReceiveCommandModeCancel();
    
    /**
     * Notify observer about commandmode change
     */
    void CommandModeChanged( TInt aErr, TCommandMode aMode );

    /**
     * Remove pipe from server during the disconnect of common plugin
     */
    void SendReceiveRemovePipe();

private:  // data
    TATPluginInterface iConnectedIf;
    MAtControllerObserver* iObserver;
    CAtCommandReq*  iAtRequest;
    CUnsolicitedDataReq* iUnsolicitedDataReq;
    CSignalIndReq* iSignalIndReq;
    CCommandModeReq* iCommandModeReq;
    };

#endif  // RMODEMATCONTROLLER_H
 
