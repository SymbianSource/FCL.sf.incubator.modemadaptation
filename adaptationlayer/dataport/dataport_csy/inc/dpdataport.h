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



#ifndef DPDATAPORT_H
#define DPDATAPORT_H

//  INCLUDES
// C32 interface
#include <cs_port.h>

//  LOCAL CONSTANTS  
//  none

//  DATA TYPES  
//  none

//  FORWARD DECLARATIONS
class CDpDataPort;
class CDpDataBuffer;
class CDpDte2Tx;
class CDpTx2Pn;
class CDpPif;
class CDpMif;
class CDpPn2Rx;
class CDpRx2Dte;
class CDpDataConfig;
class CDpFlowCtrl;
class CDpCommPep;
class CDpEscDetect;
class CDpBreak;
class CDpParityHandler;
class CDpTermDetect;
class RIscApi;


// CLASS DECLARATION

/**
*  CDpDataPort implements C32 serial protocol port (CPort).
*
*  CDpDataPort interface is used via C32 client side serial port interface
*  (RComm).
*   
*  CDpDataPort implements methods, which support serial like data transfer
*  with CMT side data servers. CDpDataPort implements methods for port
*  creation and deletion, reading, writing, port configuration, capability
*  checking, status checking, signalling, notifying and break sending.
*  
*  CDpDataPort owns other DataPort objects. CDpDataPort provides also internal
*  accessor methods to access these DataPort objects from DataPort classes.
*
*  CDpDataPort provides also internal signalling methods to enable signalling of
*  DataPort active objects.
*/
class CDpDataPort : public CPort
    {

    public:  // Constructors and destructor

        /**
        * Static constructor.
        * @param aUnit: dataport port number
        * @aParityHandler: parityhandler
        */
        static CDpDataPort * NewL(
            const TUint aUnit,
            CDpParityHandler* aParityHandler );

    public:  // New methods

    // Debug methods
    // These methods are only for DataPort internal use.

#ifdef USE_LOGGER
        /**
        * This method writes a state log.
        */
        void WriteStateLog();
#endif

#ifdef USE_FILE_SERIAL_TRUST_DEBUG
        /**
        * This method stores amount of bytes received from DTE.
        * @param aLength: Amount of bytes is increased by this length
        */
        void DebugBytesFromDte( const TInt aLength );

        /**
        * This method stores amount of bytes received from ISC Api (DCS).
        * @param aLength: Amount of bytes is increased by this length
        */
        void DebugBytesFromPn( const TInt aLength );

        /**
        * This method stores amount of bytes sent to DTE.
        * @param aLength: Amount of bytes is increased by this length
        */
        void DebugBytesToDte( const TInt aLength );

        /**
        * This method stores amount of bytes sent to ISC Api (DCS).
        * @param aLength: Amount of bytes is increased by this length
        */
        void DebugBytesToPn( const TInt aLength );
#endif

    // Accessor methods

        /**
        * This method returns ISC Api handle (RIscApi).
        * @return RIscApi&: IscApi handle
        */

        RIscApi& ISAHandle();

        /**
        * This method returns reference to break signal handler.
        * @return CDpBreak&: Break signal handler reference
        */
        CDpBreak& BreakHandler();

        /**
        * This method returns reference to COMM PEP.
        * @return CDpCommPep&: COMM PEP reference
        */
        CDpCommPep& CommPep();

        /**
        * This method returns reference to data configuration.
        * @return CDpDataConfig&: Data configuration reference
        */
        CDpDataConfig& DataConfig();

        /**
        * This method returns reference to escape sequence detector.
        * @return CDpEscDetect&: Escape sequence detector reference
        */        
        CDpEscDetect& EscDetect();

        /**
        * This method returns reference to flow control handler.
        * @return CDpFlowCtrl&: Flow control handler reference
        */        
        CDpFlowCtrl& FlowCtrl();

        /**
        * This method returns reference to message interface (MIF).
        * @return CDpMif&: MIF reference
        */
        CDpMif& Mif();

        /**
        * This method returns reference to parity handler (CDpParityHandler).
        * @return CDpParityHandler&: Parity handler reference
        */        
        CDpParityHandler& ParityHandler();

        /**
        * This method returns reference to pipe interface (PIF).
        * @return CDpPif&: PIF reference
        */
        CDpPif& Pif();

        /**
        * This method returns reference to RX buffer.
        * @return CDpDataBuffer&: RX buffer reference
        */
        CDpDataBuffer& Rx();

        /**
        * This method returns reference to Rx2Dte.
        * @return CDpRx2Dte&: Rx2Dte reference
        */
        CDpRx2Dte& Rx2Dte();

        /**
        * This method returns reference to terminator characters detector.
        * @return CDpTermDetect&: Terminator characters detector reference
        */        
        CDpTermDetect& TermDetect();

        /**
        * This method returns reference to TX buffer.
        * @return CDpDataBuffer&: TX buffer reference
        */
        CDpDataBuffer& Tx();

        // Signalling methods

        /**
        * This method signals Tx2Pn active object (CDpTx2Pn). Tx2Pn is not
        * signalled, if the request is pending.
        * @return TInt: Error code
        */
        TInt SignalTx2Pn();

        /**
        * This method signals pipe interface (PIF). PIF is not signalled,
        * if the request is pending.
        * @param aError: PIF is signalled with this error value.
        * @return TInt: Error code
        */
        TInt SignalPif( const TInt aError );

        /**
        * This method signals Rx2Dte active object (CDpRx2Dte). Rx2Dte is not
        * signalled, if the request is pending. Rx2Dte is also not signalled,
        * if there is not enough bytes to write and terminator count is zero.
        * There is not enough bytes, when the amount of bytes, which still
        * needs to be written to DTE, is greater than amount of bytes in
        * RX buffer (including echo bytes).
        * @return TInt: Error code
        */
        TInt SignalRx2Dte();

        /**
        * This method signals Dte2Tx active object (CDte2Tx). Dte2Tx is not
        * signalled, if the request is pending.
        * @return TInt: Error code
        */
        TInt SignalDte2Tx();

        // Other internal methods

        /**
        * Obtain transaction id
        */
        inline TUint8 CreateTransactionId();

        /**
        * This method returns pointer to message interface class.
        * @return CDpMif*: Pointer to message interface class
        */
        inline CDpMif* pMif();

        /**
        * This method returns pointer to pipe interface class.
        * @return CDpPif*: Pointer to pipe interface class
        */
        inline CDpPif* pPif();

        /**
        * This method returns port unit value.
        * @return TUint: Port unit
        */
        inline TUint PortUnit();

        /**
        * This method gives pipe handle to enabled or disabled pipe.
        * @param aHandle: On return, aHandle contains pipe handle.
        * If pipe is not enabled or disabled, the value is set to zero.
        * @return TInt: Error code
        */
        TInt ActivePipeHandle( TUint8& aHandle );
        

        /**
        * This method sets amount bytes to be sent before pending break.
        * Break is completed asynchronously with CPort::BreakNotifyCompleted()
        * method, when break has been sent.
        * @param aBreakBytes: Amount of break bytes
        * @return TInt: Error code
        */
        TInt SetRxBreakBytes( const TInt aBreakBytes );

        /**
        * This method finds out, whether client's read is pending in
        * Rx2Dte (CDpRx2Dte).
        * @return TBool: Is read pending or not
        */
        TBool IsRx2DteReadPending();

        /**
        * This method is used when DataPort::Destruct
        * is called but it can not be destructed before the pipe
        * is disconnected. DPMif calls this method when it recieves
        * PNS_PEP_DISCONNECT_REQ message. 
        */
        void DeleteDataPort();

        /*
        * Finds out if DataPort is ready to be destructed
        * @return TBool: Is DataPort ready to be destructed or not.
        */
        TBool IsReadyToDestruct();

    public:  // Methods from CPort

        /**
        * From CPort This method deletes CDpDataPort port.
        */
        void Destruct();

        /**
        * From CPort This method requests read. Read request is completed
        * asynchronously with CPort::ReadCompleted() method.
        *
        * Error code is returned with ReadCompleted() method. KErrCommsLineFail
        * is returned, when there is a line fail. Error code can also be system
        * error code. KErrNone is returned, when read operation has been
        * completed successfully.
        *
        * @param aClientBuffer: Pointer into client address space
        * to the descriptor containing the client’s buffer 
        * @param aLength: The amount of data to be read
        */
        void StartRead( const TAny* aClientBuffer, TInt aLength );

        /**
        * From CPort This method cancels pending read operation.
        */
        void ReadCancel();

        /**
        * From CPort This method queries the count of bytes in receive buffer.
        * @param aLength: On return, contains the count. If echo buffer is not
        * empty, the parameter contains the number of echo bytes. If echo
        * buffer is empty, the parameter contains the number of bytes in RX
        * buffer.
        * @return TInt: Error code
        */
        TInt QueryReceiveBuffer( TInt& aLength ) const;

        /**
        * From CPort This method resets RX and TX buffers.
        * NOTE: Pending read operations should be cancelled first
        * @param aFlags: Flags KCommResetRx and KCommResetTx describe, whether
        * RX (including echo buffer), TX, or both buffers are reseted.
        */
        void ResetBuffers( TUint aFlags );

        /**
        * From CPort This method requests write. Write operation is completed
        * asynchronously with CPort::WriteCompleted() method. Write is
        * completed, when TX buffer has enough space. 
        *
        * Error code is returned with WriteCompleted() method.
        * KErrCommsLineFail is returned, when there is a line fail. Error code
        * can also be system error code. KErrNone is returned, when write
        * operation has been completed successfully.
        *
        * @param aClientBuffer: Pointer into client address space to the
        * descriptor containing the client’s buffer 
        * @param aLength: The amount of data to be written
        */
        void StartWrite( const TAny* aClientBuffer, TInt aLength );

        /**
        * From CPort This method cancels pending write operation.
        */
        void WriteCancel();

        /**
        * From CPort This method sends break to DCS.
        *
        * Error code is returned with BreakCompleted() method.
        * KErrCommsLineFail is returned, when there is a line fail. Error can
        * also be system error code. KErrNone is returned, when break operation
        * has been completed successfully.
        *
        * Break is sent differently depending on break type configuration
        * from DCS. DCS can configure break type to PEP_COMM_BREAK_DESTRUCT,
        * PEP_COMM_BREAK_EXPEDITED or  PEP_COMM_BREAK_NON_EXPEDITED.
        *
        * PEP_COMM_BREAK_DESTRUCT: Break is sent and transmit buffer (TX)
        * is emptied
        * PEP_COMM_BREAK_EXPEDITED: Break is sent and then data is sent
        * PEP_COMM_BREAK_NON_EXPEDITED: Data is sent and then break is sent
        *
        * @param aTime: Time period to break for in microseconds. Time period
        * does not have any effect.
        */
        void Break( TInt aTime );

        /**
        * From CPort This method cancels pending break operation.
        */
        void BreakCancel();

        /**
        * From CPort This method gets current port configuration.
        * @param aPackage: On return, aPackage contains a packaged
        * TCommConfigV01 buffer 
        * @return TInt: Error code
        */
        TInt GetConfig( TDes8& aPackage ) const;

        /**
        * From CPort This method sets current port configuration.
        * @param aPackage: A packaged TCommConfigV01 buffer holding
        * the new configuration values 
        * @return TInt: Error code
        */
        TInt SetConfig( const TDesC8& aPackage );

        /**
        * From CPort This method is NOT SUPPORTED.
        * @param aPackage: A TCommServerConfig package buffer holding
        * the mode settings
        * @return TInt: A system-wide error code = KErrNotSupported 
        */
        TInt SetServerConfig( const TDesC8& aPackage );

        /**
        * From CPort This method is NOT SUPPORTED.
        * @param aPackage: A TCommServerConfig package buffer that,
        * on return, holds the current buffer mode settings 
        * @return TInt: A system error code = KErrNotSupported 
        */
        TInt GetServerConfig( TDes8& aPackage );

        /**
        * From CPort This method gets port capabilities. Capabilities
        * are returned on TCommCapsV01 orTCommCapsV02 depending on
        * the package size.
        * @param aPackage: A TCommCaps package buffer that, on return,
        * holds the port capabilities 
        * @return TInt: Error code
        */
        TInt GetCaps( TDes8& aPackage );

        /**
        * From CPort This methods gets current status of signals.
        * @param aSignals: On return, the bits set to reflect the status
        * of the handshaking lines
        * @return TInt: Error code
        */
        TInt GetSignals( TUint& aSignals );

        /**
        * From CPort This method sets signals ON and indicates changed signal
        * values to DCS. Only signals, which conform to current role (DTE/DCE),
        * have an effect. This method also notifies the C32 client about
        * changed signals, if the client has requested notification.
        * Only output signals are notified.
        * @param aSignals: A bitmask of the signals to set 
        * @return TInt: Error code
        */
        TInt SetSignalsToMark( TUint aSignals );

        /**
        * From CPort This method sets signals OFF and indicates changed signal
        * values to DCS. Only signals, which conform to current role (DCE/DTE),
        * have an effect. This method also notifies the C32 client about
        * changed signals, if the client has requested notification.
        * Only output signals are notified.
        * @param aSignals: A bitmask of the signals to set 
        * @return TInt: Error code
        */
        TInt SetSignalsToSpace( TUint aSignals );

        /**
        * From CPort This method gets the size of receive (RX) buffer.
        * @param aLength: On return, contains the size
        * @return TInt: Error code
        */
        TInt GetReceiveBufferLength( TInt& aLength ) const;

        /**
        * From CPort This method sets the size of receive (RX) buffer.
        * Read to IsaApi is cancelled before query of size.
        * @param aLength: Requested size of the receive buffer in bytes.
        * Buffer size must be between KDpRxBufferInitialSize and
        * KDpMaximumSetReceiveBufferLength.
        * @return TInt: Error code
        */
        TInt SetReceiveBufferLength( TInt aLength );

        /**
        * From CPort This method notifies the client when one of the signals
        * change. Notify request is completed asynchronously with
        * CPort::SignalChangeCompleted() method. Bitmapped signals in
        * completion indicate, which of the observed signals have changed.
        * Notify is completed with KErrNone.
        * @param aSignalMask: Signal mask
        */
        void NotifySignalChange( TUint aSignalMask );

        /**
        * From CPort This method cancels an outstanding
        * NotifySignalChange request.
        */
        void NotifySignalChangeCancel();

        /**
        * From CPort This method notifies the client when the port
        * configuration of DCS changes. DCS changes only flow control
        * configuration. Notify request is completed asynchronously with
        * CPort::ConfigChangeCompleted() method and with error KErrNone.
        */
        void NotifyConfigChange();

        /**
        * From CPort This method cancels an outstanding NotifyConfigChange
        * request.
        */
        void NotifyConfigChangeCancel();

        /**
        * From CPort This method notifies the client asynchronously about flow
        * control change between DP and DCS. Client gets notified immediately,
        * if current flow control status is different than last time notified
        * flow control status. Notify request is completed asynchronously with
        * CPort::FlowControlChangeCompleted() method. Notify is completed with
        * KErrNone.
        */
        void NotifyFlowControlChange();

        /**
        * From CPort This method cancels an outstanding
        * NotifyFlowControlChange request.
        */
        void NotifyFlowControlChangeCancel();

        /**
        * From CPort This method notifies the client when break signal from
        * DCS is detected. Notify request is completed asynchronously with
        * CPort::BreakNotifyCompleted() method. Notify is completed with
        * KErrNone.
        *
        * Break signal is handled differently depending on break type
        * configuration from DCS.
        *
        * PEP_COMM_BREAK_DESTRUCT: Break is sent and receive buffer (RX) is
        * emptied
        * PEP_COMM_BREAK_EXPEDITED: Break is sent and then data is sent
        * PEP_COMM_BREAK_NON_EXPEDITED: Data is sent and then break is sent
        */
        void NotifyBreak();

        /**
        * From CPort This method cancels an outstanding NotifyBreak request.
        */
        void NotifyBreakCancel();

        /**
        * From CPort This method notifies the client, when data is available
        * to be read from receive (RX) buffer. Notify request is completed
        * asynchronously with CPort::NotifyDataAvailableCompleted() method.
        * Notify is completed with KErrNone.
        */
        void NotifyDataAvailable();

        /**
        * From CPort This method cancels an outstanding NotifyDataAvailable
        * request
        */
        void NotifyDataAvailableCancel();

        /**
        * From CPort This method notifies the client, when transmit (TX)
        * buffer is emptied. Notify request is completed asynchronously with
        * CPort::NotifyOutputEmptyCompleted() method. Notify is completed
        * with KErrNone.
        */
        void NotifyOutputEmpty();

        /**
        * From CPort This method cancels an outstanding NotifyOutputEmpty
        * request.
        */
        void NotifyOutputEmptyCancel();

        /**
        * From CPort This method gets current DP->DCS flow control status.
        * @param aFlowControl: On return, aFlowControl contains TFlowControl
        * flow control status
        * @return TInt: Error code
        */
        TInt GetFlowControlStatus( TFlowControl& aFlowControl );

        /**
        * From CPort This method gets the current role DTE/DCE.
        * @param aRole: On return, aRole contains role in TCommRole
        * @return TInt: Error code
        */
        TInt GetRole( TCommRole& aRole );

        /**
        * From CPort This method sets role.
        * @param aRole Role
        * @return Error code
        */
        TInt SetRole( TCommRole aRole );

        /**
        * From CPort This method is NOT SUPPORTED.
        */
        void FreeMemory();

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @param aParityHandler
        * @param aUnit
        */
        CDpDataPort( CDpParityHandler* aParityHandler, const TInt aUnit );

        /**
        * Destructor.
        */
        ~CDpDataPort();
    
    private:  // New methods

        /**
        * This method opens Isc Api handle and creates other objects.
        */
        void InitL();

        /**
        * This method creates other objects, starts read from MIF and
        * starts pipe initiation.
        */
        void CreateObjectsL();

        /**
        * This method deletes other objects and starts pipe termination.
        */
        void DeleteCommonObjects();

        /**
        * Initialize internal object variables that are not initialized.
        */
        void InitializeObjVars();

    public:  // Data

    // For debugging purposes
#ifdef USE_FILE_SERIAL_TRUST_DEBUG
        // Debug bytes from DTE
        TInt iDebugBytesFromDte;

        // Debug bytes to Isc Api
        TInt iDebugBytesToPn;

        // Debug bytes from Isc Api
        TInt iDebugBytesFromPn;

        // Debug bytes to DTE
        TInt iDebugBytesToDte;

        // Number of DTE writes
        TInt iDebugDteWrites;

        // Number of DTE reads
        TInt iDebugDteReads;

        // Number of Isc Api writes
        TInt iDebugPnWrites;

        // Number of Isc Api reads
        TInt iDebugPnReads;

        // Handle to file server
        RFs iFileServer;

        // RFile handle
        RFile iFile;
#endif

    private:  // Data

        // Handle to IscApi
        RIscApi* iISAHandle;

        // Receive (RX) buffer
        CDpDataBuffer* iRx;

        // Transmit (TX) buffer
        CDpDataBuffer* iTx;

        // Moves data from client to TX buffer
        CDpDte2Tx* iDte2Tx;

        // Moves data from TX buffer to IsaApi
        CDpTx2Pn* iTx2Pn;

        // Pipe interface (PIF)
        CDpPif* iPifDcs;

        // Message interface
        CDpMif* iMif;

        // Moves data from IsaApi to RX buffer
        CDpPn2Rx* iPn2Rx;

        // Moves data from RX buffer to client
        CDpRx2Dte* iRx2Dte;

        // Port data configuration
        CDpDataConfig* iDataConfig;

        // Flow control handler
        CDpFlowCtrl* iFlowCtrl;

        // COMM PEP
        CDpCommPep* iCommPep;

        // Escape sequence detector
        CDpEscDetect* iEscDetect;

        // Termination characters detector
        CDpTermDetect* iTermDetect;

        // Break signal handler
        CDpBreak* iBreak;

        // Transaction ID
        TUint8 iTransactionId;

        // DTE/DCE Role
        TCommRole iRole;

        // Port unit
        const TUint iUnit;

        // Is DataPort ready to be destructed
        TBool iReadyToDestruct;

        // NOT owned by this class

        // Parity handler
        CDpParityHandler* iParityHandler;

    };

// CLASS DECLARATION

/**
*  CDpDataPortFactory implements C32 serial port factory (CSerial).
*
*  CDpDataPortFactory is responsible for creating instances of CDpDataPort
*  objects. CDpDataPortFactory also provides query functions, which return
*  general information about the capabilities of DataPort.
*
*  CDpDataPortFactory supports two ports. Port 0 is for circuit switched (CS)
*  data calls. Port 1 is for Fax Modem.
*
*  CDpDataPortFactory contains common constant data for different ports.
*  Common data consists of parity handler (CDpParityHandler).
*/
class CDpDataPortFactory : public CSerial
    {

    public:  // Enumerations

    enum TDPVersion
        {
        EMajorVersion = 4, 
        EMinorVersion = 0, 
        EBuild = 5 
        };

    public:  // Constructors and destructor

        /**
        * Static constructor.
        */
        static CDpDataPortFactory* NewL();

        /**
        * Destructor.
        */
        ~CDpDataPortFactory();

    public:  // Methods from base classes

        /**
        * From CSerial 
        * This method creates a new port. This function leaves with
        * KErrNotSupported, if unit number is not supported.
        * @param aUnit: Port unit number
        * @return CPort*: Created port
        */
        CPort* NewPortL( const TUint aUnit );

        /**
        * From CSerial
        * This method gets information about DataPort.
        * @param aSerialInfo: On return, aSerialInfo contains information
        * about DataPort in TSerialInfo.
        */
        void Info( TSerialInfo &aSerialInfo );

        /**
        * From CSerial
        * This method returns capabilities for requested port.
        * @param aPort: Port unit number
        * @return TSecurityPolicy: Capabilities for requested port.
        */
        TSecurityPolicy PortPlatSecCapability( TUint aPort ) const;

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CDpDataPortFactory();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:  // Data

        // Parity handler
        CDpParityHandler* iParityHandler;

    };

#include "dpdataport.inl" // inline methods

#endif      // DPDATAPORT_H_

// End of File

