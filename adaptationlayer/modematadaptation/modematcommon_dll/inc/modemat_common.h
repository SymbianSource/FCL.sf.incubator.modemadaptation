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



#ifndef MODEMAT_COMMON_IMPL_H
#define MODEMAT_COMMON_IMPL_H

#include <atextcommonbase.h>
#include "rmodematcontroller.h"

const TInt KMediaNameSize(256);

NONSHARABLE_CLASS(CModemAtCommon) : public CATExtCommonBase, public MAtControllerObserver
    {
public:
    /**
    * Two-phased constructor.
    */
    static CModemAtCommon* NewL();

    /**
    * Destructor.
    */
    ~CModemAtCommon();

private:
    // From CAtExtCommonBase
    /**
    * Reports connection identifier name to the extension plugin.
    *
    * @since S60 5.0
    * @param aName Connection identifier name
    * @return None
    */
    void ReportConnectionName( const TDesC8& aName );
    
    /**
    * Gets current mode status. This is a synchronous version of
    * ReceiveModeStatusChange() and the extension plugin must set aMode
    * parameter to the current mode status before this function returns.
    *
    * @since S60 5.0
    * @param aMask Mask for current mode
    * @param aMode Current mode (see explanation in this file)
    * @return Symbian error code on error, KErrNone otherwise
    */
    TInt GetMode( TUint aMask, TUint& aMode );
    
    /**
    * Receives mode status change. Cancelling of the pending request is
    * done by CancelReceiveModeStatusChange(). The implementation in the
    * extension plugin should be asynchronous.
    *
    * @since S60 5.0
    * @return Symbian error code on error, KErrNone otherwise
    */
    TInt ReceiveModeStatusChange();
    
    /**
    * Cancels a pending mode receiving request.
    *
    * @since S60 5.0
    * @return None
    */
    void CancelReceiveModeStatusChange();
    
    /**
    * Gets current NVRAM status. This is a synchronous version of
    * ReceiveNvramStatusChange() and the extension plugin must set aNvram
    * parameter to the current NVRAM status before this function returns.
    *
    * @since S60 5.0
    * @param aNvram Current NVRAM status. The current settings
    *               (delimited by "|") must differ from the Hayes
    *               defaults.
    * @return Symbian error code on error, KErrNone otherwise
    */
    TInt GetNvramStatus( RBuf8& aNvram );
    
   /**
    * Receives NVRAM status change. Cancelling of the pending request is done
    * by CancelReceiveNvramStatusChange(). The implementation in the extension
    * plugin should be asynchronous.
    *
    * @since S60 5.0
    * @return Symbian error code on error, KErrNone otherwise
    */
    TInt ReceiveNvramStatusChange();
    
    /**
    * Cancels a pending ReceiveNvramStatusChange() NVRAM status change receive
    * request.
    *
    * @since S60 5.0
    * @return None
    */
    void CancelReceiveNvramStatusChange();
    
    // From MAtControllerObserver
   /**
    * AT-command is compeleted. Called by modematcontroller.
    * @param aErr Error value of completed command
    */
    void HandleATCommandCompleted( TInt aErr );
   
   /**
    * Unsolicited data is received. Called by modematcontroller.
    * @param aErr Error value of unsolicited data
    */
    void HandleUnsolicitedResultReceived( TInt aErr );

    /**
     * Signal indication is received. Called by modematcontroller.
     * @param aErr Error value of signal indication
     */
    void HandleSignalIndication( TInt aErr );

    /**
      * Command mode change is received. Called by modematcontroller.
      * @param aErr Error value of command mode
      */
    void HandleCommandModeChanged( TInt aErr, TCommandMode aMode = EIgnore );



private:
    //Constructor
    CModemAtCommon();
    void ConstructL();
    //parses echo, quiet and verbosemodes + mode characters
    void ParseNvramStatus();
    //helpers to parse values
    TInt ParseValue(const TDesC8& aReg, TInt aPos, TUint8& aValue);
    TUint GetChangedMode();
    TBool HasNvramStatusChanged();
    //for debugging
    void PrintModes();

private:  // Data
    TBuf8<KMediaNameSize> iMediaName;
    TUint8 iReceiveModeStatusChange;
    TUint8 iReceiveNvramStatusChange;

    HBufC8* iNvramStatus;       //buffer for nvram status
    RModemAtController iRModemAt;  //interface to modematcontroller
    TUint8 iEchoMode;           //current echo-mode value
    TUint8 iQuietMode;          //current quiet-mode value
    TUint8 iVerboseMode;        //current verbose-mode value
    TUint8 iCarriageChar;       //current carriage char
    TUint8 iLineFeedChar;       //current linefeed char
    TUint8 iBackspaceChar;      //current backspace char
    TCommandMode iCommandMode;  //current command-mode value
    };

#endif      // MODEMAT_COMMON_IMPL_H

// End of File
