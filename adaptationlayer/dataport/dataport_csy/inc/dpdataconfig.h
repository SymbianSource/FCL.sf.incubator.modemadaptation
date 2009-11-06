/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef DPDATACONFIG_H
#define DPDATACONFIG_H


// INCLUDES
// none

// LOCAL CONSTANTS
// none


// DATA TYPES
// none

// FORWARD DECLARATIONS
class CDpDataPort;
class CDpPif;
class CDpDataBuffer;

// CLASS DECLARATION

/**
*  CDpDataConfig handles DTE interface port data configuration.
*  CDpDataConfig provides methods for data configuration setting and getting.
*  CDpDataConfig also provides signal mapping related methods.
*/
class CDpDataConfig :public CBase
    {
    public:  // Methods

	// Constructors and destructor

        /**
        * Static constructor.
		* @param aDataPort Dataport main class
        */
    	static CDpDataConfig* NewL( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpDataConfig();

    public:  // New methods

        /**
        * This method gets DTE interface type of break signal.
        * @return Type of break signal
	    */
        inline TUint8 DteTypeOfBreakSignal();

        /**
        * This method sets DTE interface type of break signal.
        * @param aType Type of break signal
        */
        inline void SetDteTypeOfBreakSignal( const TUint8 aType );

        /**
        * This method gets DTE interface parity bit mode in 7 bit data mode.
        * @return Parity bit mode
	    */
        inline TUint8 DteParityBitWhen7Mode();

        /**
        * This method sets DTE interface parity bit mode in 7 bit data mode.
        * @param aParity Parity bit mode
        */
        inline void SetDteParityBitWhen7Mode( const TUint8 aParity );

        /**
        * This method gets DTE interface auto bauding mode.
        * @return Auto bauding mode
	    */
        inline TUint8 DteAutoBauding();

        /**
        * This method sets DTE interface auto bauding mode.
        * @param aAutoBauding Auto bauding mode
        */
        inline void SetDteAutoBauding( const TUint8 aAutoBauding );

        /**
        * This method gets DTE interface escape character.
        * @return Escape character
	    */
        inline TUint8 EscChar();

        /**
        * This method sets DTE interface escape character.
        * @param aEscChar Escape character
        */
        inline void SetEscChar( const TUint8 aEscChar );

        /**
        * This method gets DTE interface escape time.
        * @return Escape time in 20 milliseconds
	    */
        inline TUint8 EscTime();

        /**
        * This method sets DTE interface escape time..
        * @param aEscTime Escape time in 20 milliseconds
        */
        inline void SetEscTime( const TUint8 aEscTime );

        /**
        * This method informs, whether DTE interface flow control extraction
        * character is used.
        * @return Flow control extraction character used
	    */
        inline TBool IsFlowCtrlCharExtraction();

        /**
        * This method gets DTE interface port configuration settings.
        * @return Port configuration settings
	    */
        inline const TCommConfigV01& CommConfig();

        /**
        * This method gets DTE interface handshake mode.
        * @return Handshake mode
	    */
        inline TUint Handshake();

        /**
        * This method gets DTE interface data bits mode.
        * @return Data bits mode
	    */
        inline TDataBits DataBits();

        /**
        * This method gets DTE interface parity setting.
        * @return Parity setting
	    */
        inline TParity Parity();

        /**
        * This method gets DTE interface XON character.
        * @return XON character
	    */
        inline TText8 XonChar();

        /**
        * This method gets DTE interface XOFF character.
        * @return XOFF character
	    */
        inline TText8 XoffChar();

        /**
        * This method gets DTE interface echo mode.
        * @return Echo mode
	    */
        inline TBool IsEchoDte();

        /**
        * This method gets DTE interface terminator count.
        * @return Terminator count
	    */
        inline TInt TerminatorCount();

        /**
        * This method gets DTE interface terminator character.
        * @param aIndex Index of terminator character
        * @return Terminator character at index
	    */
        TText8 Terminator( const TUint aIndex );

        /**
        * This method sets DTE interface echo mode.
        * @param aEcho Echo mode
        */
        TInt SetEchoDte( const TUint8 aEcho );
    
        /**
        * This method gets port capabilities in TCommCapsV01.
        * @param aCommCaps Port capabilities (TCommCapsV01 Reference)
        */
        void GetCommCaps( TCommCapsV01& aCommCaps ) const;

        /**
        * This method gets port capabilities in TCommCapsV02.
        * @param aCommCaps Port capabilities (TCommCapsV02 Reference)        
		*/
        void GetCommCapsV02( TCommCapsV02& aCommCapsV02 ) const;

        /**
        * This method sets DTE interface port configuration settings.
        * @param aConfig Port configuration settings (TCommConfigV01 Reference)
        * @return Error code
	    */
        TInt SetConfig( TCommConfigV01& aConfig );

        /**
        * This method gets status of signals.
        * @param aSignals On return, the bits set to reflect the status of
        * the handshaking lines
        */
        void GetSignals( TUint& aSignals );

        /**
        * This method sets signal values and also notifies the C32 client about
        * changed signals, if the client has requested notification. Only input
        * signals are notified.
        * @param aSignals Signal values
        * @return Error code
	    */
        TInt SetSignals2Epoc( const TUint aSignals );

        /**
        * This method sends mapped signal values to CMT and sets signal values.
        * This method also notifies the C32 client about changed signals, if the
        * client has requested notification. Only output signals are notified.
        * @param aSignals Signal values
        * @return Signal values
	    */
        TInt SetSignals2Cmt( const TUint aSignals );

        /**
        * This method sets up flag to indicate that we must notify C32 about
        * signal changes. Previous signal changes are forgotten.
        * @param aEnableNotify Is notifying enabled or disabled?
        */
        void SetSignalNotify( const TBool aEnableNotify );

        /**
        * This method sets signal mask for signal change notifications.
        * @param aSignalMask Signal mask
        */
        void SetNotifySignalMask( const TUint aSignalMask );

        /**
        * This method notifies the client when the port configuration of DCS
		* changes.
        * @param aEnableNotify Is notifying enabled or disabled?
        */
        void SetConfigNotify( const TBool aEnableNotify );

		/**
		* This method sets role (DCE/DTE) Capabalities
		* @param aRole dataport role
		* @return TInt: result
		*/
		TInt SetRoleCap(TCommRole aRole);

        /**
        * This method maps signals from CMT and sets signals with
        * SetSignals2Epoc() method.
        * @param aIndData Indication data from status indication
        * @return Error code
	    */
        TInt MapSignalsCmt2Epoc( TDesC8& aIndData );

        /**
        * This method maps signals and sends mapped signals to CMT.
        * @param aSignals Signals
        * @param aRole Role
        * @return Error code
	    */
        TInt MapSignalsEpoc2Cmt( const TUint aSignals, const TCommRole aRole );

        /**
        * This method returns water mark low size.
        * @param aBuffer Buffer, for which size is calculated
        * @return Water mark low size
	    */
        TInt WaterMarkLowSize( CDpDataBuffer& aBuffer );

        /**
        * This method returns water mark high size.
        * @param aBuffer Buffer, for which size is calculated
        * @return Water mark high size
	    */
        TInt WaterMarkHighSize( CDpDataBuffer& aBuffer );

        /**
        * This method completes signal change notify, if there are new signals.
        * New signals are already mapped with notify mask. This method disables
        * notifying after completion.
        * @param TUint aNewSignals
        */
        void CompleteSignalChangeNotify( const TUint aNewSignals );

        /**
        * This method completes configuration change, if notifying is active and
        * there are configuration changes. Handshaking change is only
        * configuration change, which is notified. This method disables
        * notifying after completion.
        */
        void CompleteConfigChangeNotify();

        /**
        * This method sets flow control configuration. Inwards flow control
        * configuration, outwards flow control configuration and character
        * extraction status are set. This method also notifies about flow
        * control change, if handshaking configuration is changed.
        * @param aFlowCtrlInwards Inwards flow control configuration
        * @param aFlowCtrlOutwards Outwards flow control configuration
        * @param aExtract Character extraction status
        * @return Error code
		*/
        TInt SetFlowControlConfig(
            const TUint8 aFlowCtrlInwards,
            const TUint8 aFlowCtrlOutwards,
            const TUint8 aExtract);

        /**
        * This method informs, whether there is a line fail. There can be a line
        * fail, when the C32 client has configured some signal to fail e.g. CTS
        * --> KConfigFailCTS. Now when CTS goes down, there is a line fail.
        * @param aRole Role
        * @return TBool: Is there a line fail?
		*/
        TBool IsLineFail( const TCommRole aRole ) const;

    private: //Methods

        /**
        * C++ default constructor.
        */
        CDpDataConfig( CDpDataPort& aDataPort );

        /**
        * Symbian 2nd phase constructor.
        */
	    void ConstructL();

    private:// New Methods

        /**
        * This method finds out bit rate from configuration.
        * @param aConfig Configuration
        * @param aRate On return, aRate contains rate mapped to CMT
        * @return Error code
		*/
	    TInt Rate( TCommConfigV01& aConfig, TUint8& aRate );

        /**
        * This method finds out data bits from configuration.
        * @param aConfig Configuration
        * @param aDataBits On return, aDataBits contains data bits mapped to CMT
        * @return Error code
		*/
	    TInt DataBits( TCommConfigV01& aConfig, TUint8& aDataBits );

        /**
        * This method finds out stop bits from configuration.
        * @param aConfig Configuration
        * @param aStopBits On return aStopBits contains stop bits mapped to CMT
        * @return Error code
		*/
	    TInt StopBits( TCommConfigV01& aConfig, TUint8& aStopBits );

        /**
        * This method finds out parity setting from configuration.
        * @param aConfig Configuration
        * @param aParity On return, aParity contains parity setting mapped to
        * CMT
        * @return Error code
		*/
	    TInt Parity ( TCommConfigV01& aConfig, TUint8& aParity );

        /**
        * This method maps DTE type signals to CMT.
        * @param aSignals Signals
        * @param aData On return, aData contains mapped signals
        */
	    void HandleDteSignalMapping( const TUint aSignals, TDes8& aData );

        /**
        * This method maps DCE type signals to CMT.
        * @param aSignals Signals
        * @param aData On return, aData contains mapped signals
        */
	    void HandleDceSignalMapping( const TUint aSignals, TDes8& aData );
    
    private:    // Data
		/** DataPort. */
        CDpDataPort&    iDataPort;

        /** DTE type of break signal. */
        TUint8          iDteTypeOfBreakSignal;

        /** Parity bit mode in 7 bit data mode. */
        TUint8          iDteParityBitWhen7Mode;

        /** Auto bauding mode. */
        TUint8          iDteAutoBauding;

        /** Escape character. */
        TUint8          iEscChar;

        /** Escape time. */
        TUint8          iEscTime;

        /** Peer configuration. */
        TCommNotificationV01 iPeerConfig;

        /** Confuration. */
        TCommConfigV01  iConfig;

        /** TCommCapsV02 capabilities. */
        TCommCapsV02    iCapsV02;

        /** Current signals. */
        TUint           iSignals;

        /** Signal change notify status. */
        TBool           iSignalNotify;

        /** Configuration change notify status. */
        TBool           iConfigNotify;

        /** Signal change notify mask. */
        TUint           iNotifySignalMask;

        /** Flow control extraction character status. */
        TBool           iFlowCtrlCharExtraction;

        /** Echo mode. */
        TBool           iEchoDte;
    
		/** Dataport Role **/
		TCommRole iRole;
    };

#include "dpdataconfig.inl"   // inline methods

#endif      // DPDATACONFIG_H

//  End of File

