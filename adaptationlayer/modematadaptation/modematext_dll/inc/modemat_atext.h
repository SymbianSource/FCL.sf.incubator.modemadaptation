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


#ifndef MODEMAT_ATEXT_IMPL_H
#define MODEMAT_ATEXT_IMPL_H

#include <atextpluginbase.h>
#include "rmodematcontroller.h"

const TInt KNameLength(256);

NONSHARABLE_CLASS(CModemATExt) : public CATExtPluginBase, public MAtControllerObserver
  	{
public:
        
    /**
    * Two-phased constructor.
    */
    static CModemATExt* NewL();
    
    /**
    * Destructor.
    */
    ~CModemATExt();

public:
 	
 	//from atextpluginbase
    /**
       * Reports the support status of an AT command. This is a synchronous API
       * and the extension plugin must set aEditorCmd parameter before this
       * function returns.
       *
       * @param aCmd The AT command. Its format may vary depending on the
       *             specification. E.g. in BT HFP case, the command may contain
       *             a character carriage return (<cr>) in the end.
       * @return ETrue if the command is supported; EFalse otherwise.
       */
     
    TBool IsCommandSupported( const TDesC8& aCmd );
 	
   /**
     * Handles an AT command. Cancelling of the pending request is done by
     * HandleCommandCancel(). The implementation in the extension plugin should
     * be asynchronous.
     *
     * The extension plugin which accepts this command is responsible to supply
     * the result codes and response and to format result codes properly, e.g.
     * in BT HFP case, the format should be <cr><lf><result code><cr><lf>
     *
     * After an extension plugin has handled or decided to reject the given AT
     * command, it must inform ATEXT by HandleCommandCompleted() with a proper
     * error code.
     *
     * @since S60 5.0
     * @param aCmd The AT command to be handled. Its format may vary depending
     *             on the specification. E.g. in BT HFP case, the command may
     *             contain a character carriage return (<cr>) in the end.
     * @param aReply When passed in, contains the built in answer filled by
     *               ATEXT if it is not empty; when command handling completes
     *               successfully, contains the result codes and responses to
     *               this command; Its ownership always belongs to ATEXT, plugin
     *               may reallocate its space when needed.
     * @param aReplyNeeded Reply needed if ETrue, no reply otherwise. If EFalse,
     *                     the aReply must not contain the reply, otherwise it
     *                     must contain verbose or numeric reply (ATV0/1) or an
     *                     empty string reply (with ATQ).
     * @return None
     */
   	void HandleCommand( const TDesC8& aCmd, RBuf8& aReply, TBool aReplyNeeded );


   /**
     * Cancels a pending HandleCommand request.
     *
     * @since S60 5.0
     * @return None
     */

   	void HandleCommandCancel();
  
   /**
    * Receives unsolicited results. Cancelling of the pending request is done
    * by ReceiveUnsolicitedResultCancel(). The implementation in the extension
    * plugin should be asynchronous.
    *
    * @since S60 5.0
    * @return None
    */
   void ReceiveUnsolicitedResult();

   /**
    * Cancels a pending ReceiveUnsolicitedResult request.
    *
    * @since S60 5.0
    * @return None
    */
   void ReceiveUnsolicitedResultCancel();
   
   /**
     * Handles editor mode input.
     *
     * @since S60 5.0
     * @param aInput The input to process after IsCommandSupported() has
     *               reported about a supported command with aEditorCmd=ETrue
     * @param aReply Reply to return when end of editor mode found
     * @return ETrue if end of editor mode (ctrl-z), EFalse otherwise
     */
   
   TBool HandleEditorModeInput( const TDesC8& aInput,
                                            RBuf8& aReply,
                                            TBool& aEchoOn );

   /**
     * Reports NVRAM status change to the plugins.
     *
     * @since S60 5.0
     * @param aNvram New NVRAM status. Each call of this function is a result
     *               of DUN extracting the form notified by
     *               CATExtCommonBase::SendNvramStatusChange(). Each of the
     *               settings from SendNvramStatusChange() is separated to
     *               one call of ReportNvramStatusChange().
     * @return None
     */
  	void ReportNvramStatusChange( const TDesC8& aNvram );
   
    /**
     * Gets the next part of reply initially set by HandleCommandComplete().
     * Length of aNextReply must be equal or less than KDefaultCmdBufLength.
     *
     * @since S60 5.0
     * @param aNextReply Next reply
     * @return Symbian error code on error, KErrNone otherwise
     */
	TInt GetNextPartOfReply( RBuf8& aNextReply );
	
    /**
     * Reports connection identifier name to the extension plugin.
     *
     * @since S60 5.0
     * @param aName Connection identifier name
     * @return None
     */
    void ReportConnectionName( const TDesC8& aName );
   
   
   /**
     * Next reply part's length.
     * The value must be equal or less than KDefaultCmdBufLength.
     * When the reply from this method is zero, ATEXT stops calling
     * GetNextPartOfReply().
     *
     * @since S60 5.0
     * @return Next reply part's length if zero or positive
     */
   
    TInt NextReplyPartLength();
    
    /**
     * Reports about external handle command error condition.
     * This is for cases when for example DUN decided the reply contained an
     * error condition but the plugin is still handling the command internally.
     * Example: in command line "AT+TEST;ATDT1234" was given. "AT+TEST" returns
     * "OK" and "ATDT" returns "CONNECT". Because "OK" and "CONNECT" are
     * different reply types the condition is "ERROR" and DUN ends processing.
     * This solution keeps the pointer to the last AT command handling plugin
     * inside ATEXT and calls this function there to report the error.
     * It is to be noted that HandleCommandCancel() is not sufficient to stop
     * the processing as the command handling has already finished.
     *
     * @since S60 5.0
     * @return None
     */
    void ReportExternalHandleCommandError();

    
	 //from RModemAtController 
	 
	/**
     * AT-command is compeleted. Called by modematcontroller.
     * @param aErr Error value of completed command
     */
   	 	 
     void HandleATCommandCompleted( TInt aErr ) ;
     
    /**
     * Unsolicited data is received. Called by modematcontroller.
     * @param aErr Error value of unsolicited data
     */
     void HandleUnsolicitedResultReceived( TInt aErr ) ;
     
     /**
     * Signal indication is received. Called by modematcontroller.
     * @param aErr Error value of signal indication
     */
     void HandleSignalIndication( TInt aErr ) ;

     /**
     * Command mode change. Called by modematcontroller.
     * @param aErr Error value of command mode request
     */
     void HandleCommandModeChanged( TInt aErr, TCommandMode aMode );
     
private:

    CModemATExt();
    //Constructor
    void ConstructL();

	//Returns AT command by index
    TPtrC8 GetAtCommand( TInt aNumber );

    
private:    // Data
    
    RModemAtController iRModemAt;
    TBuf8<KNameLength> iName;
    HBufC8* iReplyBuffer;
    TPtr8 iReplyPtr;
    TPtr8 iUnsolicitedPtr;
    HBufC8* iUnsolicitedBuffer;
    RBuf8* iReply;
    TBool iReplyNeeded;
    HBufC8* iCommandBuf;
    };

#endif      //MODEMAT_ATEXT_IMPL_H
            
// End of File
