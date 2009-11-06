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



#ifndef ISCAPI_H
#define ISCAPI_H

//  includes
#include <e32std.h>

//  panic codes
enum TIscApiPanics
    {
    EIscApiChannelNumberOutOfRange = 1,
    EIscApiChannelNotOpen,
    EIscApiCustomFunctionUndefined,
    EIscApiFalseIniFile,
    EIscApiDriverLoadFail,
    EIscApiChannelCreateFail,
    EIscApiMemoryAllocationFailure
    };

/**
*  Class RIscApi defines an interface for sending and receiving data
*  from Domestic OS. 
*
*  @lib IscApi.LIB
*  @since 2.8
*/
class RIscApi : public RBusLogicalChannel
    {
    public:  // Constructors and destructor
      
        //C++ default constructor.
        IMPORT_C RIscApi();

        //Destructor.
        IMPORT_C ~RIscApi();

    public: // New functions

        /**
        Initialize the ISC Interface
        @param aStatus Request status, completed with KErrNone if successful
        */
        IMPORT_C void InitializeModemInterface( TRequestStatus& aStatus );

        /**
        Cancel ISC Initialization sequence
        */
        IMPORT_C void InitializeCancel();

        /**
        Open ISC channel asyncronously.
        @param aChannelNumber Channel id to open. Licensee specific ISC. 
        @param aStatus Request status, completed with KErrNone if successful
        @param aHandleType Specifies whether the handle is owned by process or by thread
        @param aOpenParams Optional parameters given for the ISC implementation
        @capability CommDD
        */
        IMPORT_C void Open( const TUint16 aChannelNumber,
                            TRequestStatus& aStatus,
                            TOwnerType aHandleType = EOwnerThread,
                            const TDesC8* aOpenParams = NULL );

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)

        /**
        Loan a channel owned by some other object. After loan messages directed to and from this
        channel number are coming to this client not the one which opened the channel. After channel is not
        needed anymore the loan should be returned.
        Returns KErrNone when ok, KErrNotFound if channel to be loaned is not open or connection is lost,
        KErrAlreadyExist if tried to loan already loaned channel.
        @param aChannelNumber
        @return TInt
        */
        IMPORT_C TInt Loan( const TUint16 aChannelNumber, TOwnerType aHandleType = EOwnerThread );

        /**
        Returns a loaned channel owned by some other object. After loan is retuned messages directed to and from this
        channel number are not coming anymore to this client they are directed back to client which opened the channel.
        Does nothing if the channel is not open.
        @param aChannelNumber
        @param TInt
        */
        IMPORT_C void ReturnLoan( const TUint16 aChannelNumber );
#endif

        /**
        Cancel asynchronous channel opening.
        */
        IMPORT_C void OpenCancel();

        /**
        Close ISC Channel synchronously.
        */
        IMPORT_C void Close();

        
        /**
        Close ISC Channel asynchronously.
        */
        IMPORT_C void Close( TRequestStatus& aStatus );

        /**
        Empties the receive buffers of the channel.
        */
        IMPORT_C void ResetBuffers();

        /**
        Send data to baseband modem (synchronous).
        @param aData Data to be sent
        @return TInt KErrNone if successful 
        */
        IMPORT_C TInt Send( const TDesC8& aData );

        /**
        Send data to baseband modem (asynchronous).
        @param aStatus Request status, completed with KErrNone if successful
        @param aData Data to be sent 
        */
        IMPORT_C void Send( TRequestStatus& aStatus, const TDesC8& aData );

        /**
        Cancel asynchronous Send request
        */
        IMPORT_C void SendCancel();

        /**
        Send data to baseband modem (synchronous).
        @param aData Data to be sent
        @return TInt KErrNone is successful
        */
        IMPORT_C TInt DataSend( const TDesC8& aData );

        /**
        Send data to baseband modem (asynchronous).
        @param aStatus Request status, completed with KErrNone is successful
        @param aData Data to be sent
        */
        IMPORT_C void DataSend( TRequestStatus& aStatus,
                                const TDesC8& aData );

        /**
        Cancel asynchronous DataSend request.
        */
        IMPORT_C void DataSendCancel();

        /**
        Receive data from baseband modem.
        @param aStatus Request status, completed with KErrNone when data has arrived
        @param aData A buffer in the client address space where to write the incoming data
        @param aNeededBufLen If incoming data is too big to fit into client buffer in this variable the correct size is found
        */
        IMPORT_C void Receive( TRequestStatus& aStatus, 
                               TDes8& aData,
                               TUint16& aNeededBufLen );

        /**
        Cancel data receiving from baseband modem
        */
        IMPORT_C void ReceiveCancel();

        /**
        Receive data from baseband modem.
        @param aStatus Request status, completed with KErrNone when data has arrived
        @param aData A buffer in the client address space where to write the incoming data
        @param aNeededBufLen If incoming data is too big to fit into client buffer in this variable the correct size is found
        */
        IMPORT_C void DataReceive( TRequestStatus& aStatus,
                                   TDes8& aData,
                                   TUint16& aNeededBufLen );

        /**
        Cancel data receiving from baseband modem
        */
        IMPORT_C void DataReceiveCancel();

        /**
        Get the current connection status.
        @return TInt status of connection either EIscConnectionOk or EIscConnectionNotOk.
        @see TIscConnectionStatus
        */
        IMPORT_C TInt ConnectionStatus();

        /**
        Listen for connection status changes.
        @param aStatus Request status, completed with KErrNone when status changes
        @param aConnectionStatus new connection status either EIscConnectionOk or EIscConnectionNotOk
        @see TIscConnectionStatus
        */
        IMPORT_C void NotifyConnectionStatus( TRequestStatus& aStatus,
                                              TInt& aConnectionStatus );

        /**
        Stop listening for connection status changes.
        */
        IMPORT_C void NotifyConnectionStatusCancel();

        /**
        Get the current flow control status (in uplink-direction).
        @return TInt flow control status.  Either EIscFlowControlOn or EIscFlowControlOff
        @see TIscFlowControlStatus
        */
        IMPORT_C TInt FlowControlStatus();

        /**
        Listen for flow control status changes.
        @param aStatus Request status, completed with KErrNone when status changes
        @param aFlowControlStatus new flow control status.  Either EIscFlowControlOn or EIscFlowControlOff
        @see TIscFlowControlStatus
        */
        IMPORT_C void NotifyFlowControl( TRequestStatus& aStatus,
                                         TInt& aFlowControlStatus );

        /**
        Stop listening for flow control status changes.
        */
        IMPORT_C void NotifyFlowControlCancel();

        /**
        Get channel info. The data that is returned depends on the ISC implementation.
        @param aChannel The channel which info is requested
        @param aInfo 
        @return TInt KErrNone if successful. KErrNotSupported, if not supported by the ISC implementation.
        */
        IMPORT_C TInt GetChannelInfo( const TUint16 aChannel,
                                      TDes8& aInfo );

        /**
        Get maximun data size (for sending and receiving).
        @return TInt Maximum data size in bytes
        */
        IMPORT_C TInt MaximumDataSize();

        /**
        Asynchronous API extension function, functionality has to be specified by the ISC implementation.
        @param aStatus Request status, completed with KErrNone if successful
        @param aOperation Which custom function to execute
        @param aParameters Optional parameters given to the ISC implementation
        */
        IMPORT_C void CustomFunction( TRequestStatus& aStatus, 
                                 const TUint16 aOperation, 
                                 TAny* aParameters = NULL );

        /**
        Synchronous API extension function, functionality has to be specified by the ISC implementation.
        @param aOperation Which custom function to execute
        @param aParameters Optional parameters given to the ISC implementation
        @return TInt KErrNone if successful
        */
        IMPORT_C TInt CustomFunction( const TUint16 aOperation, 
                                 TAny* aParameters = NULL );

        /**
        Cancel the execution of the asynchronous API extension function.
        @param aOperation Which function to cancel
        */
        IMPORT_C void CustomFunctionCancel( const TUint16 aOperation );

    private:

        /**
        Load all required drivers using Isc_config.ini file.
        @param aMultiplexerInit, descriptor where ISC Multiplexer initialization string is written.
        @param aDataTransmissionInit, descriptor where ISC Data Transmission module initialization string is written
        */
        void LoadL( const TDesC8& aMultiplexerInit, const TDesC8& aDataTransmissionInit ) const;

        /**
        Helper function for isc_config.ini parsing.
        @param TDes8& aInifileLine, current line from the .ini file.
        @param TDes8& aDriverName, name of the driver to be searched.
        @return TInt, the position. 
        */                    
        TInt InifileLineParser( TDes& aInifileLine, TDes& aDriverName )const;

        /**
        Helper function for isc_config.ini parsing.
        @param TDes& aInitString, the initstring.
        @param TDes8& aDriverName, name of the driver to be searched.
        @param const TDesC8& aString, descriptor for
        @return TInt, the position. 
        */         
        void DriverNameParser( TDes& aInitString, TDes& aDriverName, const TDesC8& aString )const;

        /**
        Panic current thread if channel has not been opened before using IscApi
        */
        void PanicIfNotOpen() const;


    private:    // Data

        TPtr8   iNeededLen;
        TPtr8   iNeededDataLen;
        TPtr8   iConnectionStatus;
        TPtr8   iFlowControlStatus;
        TBool   iOpen;
        TUint16 iChannelNumber;

#ifdef __WINS__
        TBool iInitializing;
#endif

    };

#endif      // ISCAPI_H
            
// End of File
