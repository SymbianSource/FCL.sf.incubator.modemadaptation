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



#include <e32std.h>
#include <iscapi.h>
#include <iscnokiadefinitions.h>
#include <pn_const.h>
#ifndef NCP_COMMON_BRIDGE_FAMILY       
#include <nsisi.h>
#include <pipeisi.h>
#include <pipe_sharedisi.h>
#endif
#include <tisi.h>
#include <at_modemisi.h>
#include "cmodemathandler.h"
#include "cmodematpipecontroller.h"
#include "modemattrace.h"
#include "cmodematsrv.h"

#define KCommandBits 0x0f
#define KMaxBufferLength 1024
#define KTransactionIdBitsShift 4
#define KFiller 0
#define KIndicationOrderMsgLength 14
#define KObjectIdLength 2
#define KEightBits 8
#define KDefaultTId 0

const TInt KUnsubscribeFourEvents(4);
const TInt KAlignToFourBytes(4);

#ifdef __WINSCW__
#define KEmulatedReplyMsgLength 50
#endif

CModemAtHandler* CModemAtHandler::NewL( CModemAtSrv& aServer )
    {
    CModemAtHandler* self = new (ELeave) CModemAtHandler( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CModemAtHandler::~CModemAtHandler()
    {
    C_TRACE(( _T("CModemAtHandler::~CModemAtHandler()") ));
    TBuf8<KUnsubscribeFourEvents> events;
    events.Append( KFiller );
    events.Append( KFiller );  
    events.Append( KFiller );
    events.Append( KFiller );

    if( iIscApi )
        {
        iIscApi->CustomFunction( EIscNokiaEventSubscribe, &events );
        }

    if(iPipeController)
        {
        delete iPipeController;
        iPipeController = NULL;
        } 
    Cancel();

    if( iIscApi )
        {
        iIscApi->Close();
        delete iIscApi;
        }
   }

CModemAtHandler::CModemAtHandler( CModemAtSrv& aServer )
    : CActive( EPriorityHigh ),
    iServer( aServer ),
    iIscApi( NULL ),
    iReceivedMessageBuffer( NULL ),
    iMessageBufferSize( KMaxBufferLength ),
    iReceivePtr( 0, 0 ),
    iReceiveMessageLength( 0 ),
    iReceivedMessage( NULL ),
    iPepObjId(0),
    iPipeController(NULL),
    iLastTransactionId( 0 ),
    iDisconnected( EFalse ),
    iModemAtExistInCmt( EFalse )
    {
    C_TRACE (( _T("CModemAtHandler::CModemAtHandler") ));
    }

void CModemAtHandler::DoCancel()
    {
    C_TRACE (( _T("CModemAtHandler::DoCancel()") ));
    iIscApi->ReceiveCancel();
    }


void CModemAtHandler::RunL()
    {
    C_TRACE (( _T("CModemAtHandler::RunL()") ));
   
    if( iStatus == KErrNone ) 
        {
        TIsiReceiveC receiveMessage = TIsiReceiveC( *iReceivedMessageBuffer );
        DUMP_MESSAGE( iReceivePtr );
            
        if( iReceivedMessageBuffer->Length() > ISI_HEADER_OFFSET_MESSAGEID )
            {
            switch(receiveMessage.Get8bit(ISI_HEADER_OFFSET_RESOURCEID))
                {
                case PN_AT_MODEM: //AT MODEM COMMANDS
                    C_TRACE(_L("PN_AT_MODEM"));

                    switch( receiveMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
                        {
                        case AT_MODEM_CONNECT_RESP: 
                            HandleModemConnectResp( receiveMessage );
                            break;

                        case AT_MODEM_DISCONNECT_RESP:
                            C_TRACE(_L("AT_MODEM_DISCONNECT_RESP"));
                            break;

                        case AT_MODEM_CMD_RESP:
                            HandleATResponse( receiveMessage );
                            break;

                        case AT_MODEM_SIGNAL_IND:
                            HandleSignalInd( receiveMessage );
                            break;

                        case AT_MODEM_UNSOLICITED_DATA_IND:
                            HandleUnsolicitedData( receiveMessage );
                            break;

                        case AT_MODEM_INTERMEDIATE_DATA_IND:
                            HandleIntermediateDataInd( receiveMessage );
                            break;

                        case AT_MODEM_DATA_REDIRECT_IND:
                            HandleRedirectInd( receiveMessage );
                            break;
                            
                        case AT_MODEM_DATA_REDIRECT_RESULT_RESP:
                            C_TRACE((_L("AT_MODEM_DATA_REDIRECT_RESULT_RESP")));
                            HandleRedirectResultResp( receiveMessage );
                            break;

                        case AT_MODEM_SIGNAL_DETECTED_RESP:
                            C_TRACE((_L("AT_MODEM_SIGNAL_DETECTED_RESP")));
                            HandleSignalDetectedResp(  receiveMessage  );
                            break;
                        default: 
                            C_TRACE(_L("CModemAtHandler::RunL: Not a valid message"))  ;
                            TRACE_ASSERT_ALWAYS;
                            break;
                         } //switch( receiveMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
                        
                    break;  //case PN_AT_MODEM: //AT MODEM COMMANDS

#ifndef NCP_COMMON_BRIDGE_FAMILY
                case PN_NAME_SERVICE: 
                    C_TRACE(_L("PN_NAME_SERVICE"));
                    iPipeController->HandleNameServiceMessage(receiveMessage);
                    break;

                case PN_PIPE:
                    C_TRACE(_L("Pipe message"));
                    iPipeController->HandlePipeServiceMessage(receiveMessage);
                    break;
#endif
                default:  
                    C_TRACE((_L("Unknown resource id")));
                    TRACE_ASSERT_ALWAYS;
                    break; 
                }
                
            } //if( receiveMessageBuffer->Length() > ISI_HEADER_OFFSET_MESSAGEID )
        else 
            {
            C_TRACE(_L("ISI message is too short"));
            TRACE_ASSERT_ALWAYS;
            }
        
        //Start receiving ISI messages again
        iReceivePtr.Set( iReceivedMessageBuffer->Des() );
        iIscApi->Receive( iStatus, iReceivePtr, iReceiveMessageLength );
        SetActive();
        } // if( iStatus == KErrNone )
    else 
        {
        TRACE_ASSERT_ALWAYS;
        } 
    }


void CModemAtHandler::ConstructL()
    {
   C_TRACE (( _T("CModemAtHandler::ConstructL()") ));

    CActiveScheduler::Add( this );
    
    iIscApi = new ( ELeave ) RIscApi;
    C_TRACE(_L("RIscApi created"));
   
#ifndef __WINSCW__
      OpenChannelL();
#ifndef NCP_COMMON_BRIDGE_FAMILY
      iPipeController->QueryModemAtFromNameService();
#endif
#else
      iEmulatedValue = 0;
#endif
    
    }

TInt CModemAtHandler::SendATCommand(const TUint8 aDteId,
    const TATPluginInterface aPluginType,
    const TUint8 aMessageType,
    const TDesC8& aCmd)
    {
    C_TRACE (( _T(">>CModemAtHandler::SendATCommand()") ));

    TInt err = KErrNone;
    // aMessageType contains normal AT-cmd (0) or Nvram-query (1)
    TUint transactionId = ( aPluginType << KTransactionIdBitsShift ) | aMessageType;
    // used with intermediate data ind
    // can be removed, when AT_MODEM_INTERMEDIATE_DATA_IND supports transactionid        
    iLastTransactionId = transactionId;  //messages are routed to correct plugin by transaction id
    C_TRACE((_L("transaction id %d, plugintype %d, aMessageType %d"), transactionId, aPluginType, aMessageType));
    C_TRACE((_L("iLastTransctionId = %d "), iLastTransactionId ));

    #ifdef __WINSCW__
    iDteId = aDteId;
    iChannel = aMessageType;
    iPluginType = aPluginType;
    HandleATResponse();
    return KErrNone;
    #endif

    TInt size = ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_CMDLINEBUFFER + aCmd.Length();
    
    TInt sizeWithFillers = size;
    while( sizeWithFillers % KAlignToFourBytes )
        {
        sizeWithFillers++;
        }
    C_TRACE((_L("Message length %d"), sizeWithFillers));

    HBufC8* message = HBufC8::New( sizeWithFillers );
    if( message )
      {
      TPtr8 messageptr = message->Des();
      TIsiSend isimessage( messageptr, sizeWithFillers );
      isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_AT_MODEM );       
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_TID, transactionId );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_MESSAGEID, AT_MODEM_CMD_REQ );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_DTEID, aDteId );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_FILLERBYTE1, 0 );
      C_TRACE((_L("AT command length: %d"), aCmd.Length()));
      isimessage.Set16bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_LENGTH, (TUint16) aCmd.Length() );

      const TUint8* ptr = aCmd.Ptr();
      for ( TInt i = 0; i < aCmd.Length(); i++ )
          {
          C_TRACE((_L("message %d, 0x%x '%c'"), i, ptr[i], ptr[i]));
          isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_CMDLINEBUFFER + i, ptr[i]);
          }

      for( TInt j = sizeWithFillers; j < size; j++ )
          {
          C_TRACE((_L("Add filler [%d]"), j));
          isimessage.Set8bit( j , KFiller );
          }

      isimessage.Complete();
      C_TRACE(_L("Created ISI-message"));
      DUMP_MESSAGE( messageptr );

      err = iIscApi->Send( messageptr );
      TRACE_ASSERT( err == KErrNone );
      delete message;
      message = NULL;
      }
    else
      {
      C_TRACE(_L("No memory - error in sending ISI message from heap"));
      TRACE_ASSERT_ALWAYS;
      }
    C_TRACE (( _T("<<CModemAtHandler::SendATCommand() err: %d"), err ));
    return err;
    }

void CModemAtHandler::OpenChannelL()
    {
    C_TRACE (( _T("CModemAtHandler::OpenChannelL()") ));
    TRequestStatus openStatus;

    iIscApi->Open( EIscNokiaAtController, openStatus, EOwnerThread );
    User::WaitForRequest( openStatus );

    TRACE_ASSERT( !openStatus.Int());
    
    if( iReceivedMessageBuffer )
        {
        delete iReceivedMessageBuffer;
        iReceivedMessageBuffer = NULL;
        }

    iReceivedMessageBuffer = HBufC8::NewL(iMessageBufferSize);
    TRACE_ASSERT(iReceivedMessageBuffer);
    iReceivePtr.Set( iReceivedMessageBuffer->Des() );

    C_TRACE(_L("Start receive ISI messages"));
    iIscApi->Receive( iStatus, iReceivePtr, iReceiveMessageLength );

    SetActive();
    
    C_TRACE(_L("Start receive indications"));
    TBuf8<KIndicationOrderMsgLength> events;
    events.Append( PN_AT_MODEM );
    events.Append( AT_MODEM_INTERMEDIATE_DATA_IND );  
    events.Append( PN_AT_MODEM );
    events.Append( AT_MODEM_SIGNAL_IND );  
    events.Append( PN_AT_MODEM );
    events.Append( AT_MODEM_UNSOLICITED_DATA_IND );  
    events.Append( PN_AT_MODEM );
    events.Append( AT_MODEM_DATA_REDIRECT_IND );  

#ifndef NCP_COMMON_BRIDGE_FAMILY       
    //name add ind
    events.Append( PN_NAMESERVICE );
    events.Append( PNS_NAME_ADD_IND );  
    events.Append( PN_NAMESERVICE );
    events.Append( PNS_NAME_REMOVE_IND );
    events.Append( PN_PIPE );
    events.Append( PNS_PEP_STATUS_IND );
#else
    events.Append( 0 );
    events.Append( 0 );  
    events.Append( 0 );
    events.Append( 0 );
    events.Append( 0 );
    events.Append( 0 );
#endif
    
    iIscApi->CustomFunction(EIscNokiaEventSubscribe,&events);
    TBuf8<KObjectIdLength> objId;
    iIscApi->GetChannelInfo(EIscNokiaAtController, objId);
    
    iPepObjId = (((TUint16)(objId[0]))<<KEightBits) + objId[1];
    C_TRACE((_L("PepObjId 0x%x"),iPepObjId));
    iPipeController = CModemAtPipeController::NewL(*iIscApi, iPepObjId, *this); 
    C_TRACE((_L("iPipeController 0x%x"),iPipeController ));
    ASSERT_PANIC_ALWAYS( iPipeController );
    }


void CModemAtHandler::HandleATResponse( const TIsiReceiveC& aMessage )
    {
    C_TRACE (( _T("CModemAtHandler::HandleATResponse()") ));
    
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_DTEID ) ;
    TUint transactionId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_TID );
    TUint8 command = transactionId & KCommandBits; //normal AT-cmd or Nvram-query
    TInt length = aMessage.Get16bit( ISI_HEADER_SIZE+AT_MODEM_CMD_RESP_OFFSET_LENGTH );
    C_TRACE(( _L("DteId: %d AT (0) or nvram command (1): %d Response length %d"), dteId, command, length ));

    /**
     * Plugintype can be read from transaction id:
     * TUint8 pluginType = transactionId >> KTransactionIdBitsShift;
     * (TATPluginInterface) pluginType
     *
     * Current implementation uses first in first out response completion
     * so plugintype is not needed.
     */
    C_TRACE(( _L("Plugintype: %d"), transactionId >> KTransactionIdBitsShift ));
    
    iServer.HandleATResponse( dteId,  
      aMessage.GetData( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_CMDLINEBUFFER, length),
      command );
    }

#ifdef __WINSCW__
void CModemAtHandler::HandleATResponse( )
    {
    C_TRACE (( _T("CModemAtHandler::HandleATResponse()") ));
    //reply back immediately
    const TInt KReplyLength(50);
    TBuf8<KReplyLength> replyEmulated = _L8("\r\nAT TEST REPLY");
    replyEmulated.AppendNum(iEmulatedValue++);
    replyEmulated.Append(_L8("\r\n"));
    iServer.HandleATResponse( iDteId, replyEmulated, 0 /* 0 = AT-cmd, 1 = nvram */);
    }
#endif

    
TInt CModemAtHandler::Connect( const TUint8 aDteId )
    {
    C_TRACE (( _T("CModemAtHandler::Connect()") ));

    if( !iModemAtExistInCmt )
        {
        C_TRACE((_T("Connection failed, PN_AT_MODEM is not in CMT")));
        TRACE_ASSERT_ALWAYS;
        return KErrNotFound;
        }
    SetDisconnected( EFalse );

#ifdef __WINSCW__
    return KErrNone;
#endif

    TInt err( KErrNone );
	TUint8 pipeHandle( KInvalidPipeHandle );

#ifndef NCP_COMMON_BRIDGE_FAMILY
    pipeHandle = iPipeController->GetPipeHandle();
#endif

    if( pipeHandle == KInvalidPipeHandle )
        {
        C_TRACE(( _L("Connecting with dteId %d. (RComm was not used before connecting the plug-ins)."), aDteId ));
        err = SendAtModemConnectReq( aDteId );
        }
    else
        {
        C_TRACE(( _L("Connecting with pipehandle %d."), pipeHandle ));
        err = SendAtModemConnectReq( pipeHandle );
        }
   
   if( err != KErrNone )
       {
       TRACE_ASSERT_ALWAYS;
       return KErrNotFound;
       }

    return KErrNone;
    }

void CModemAtHandler::SetDisconnected( TBool aIsDisconnected )
    {
    C_TRACE (( _T("CModemAtHandler::SetDisconnected(%d)"),aIsDisconnected ));
    iDisconnected = aIsDisconnected;
    }

TBool CModemAtHandler::IsDisconnected()
    {
    C_TRACE (( _T("CModemAtHandler::IsDisconnected(%d)"),iDisconnected ));
    return iDisconnected;
    }

TBool CModemAtHandler::IsPluginConnected()
    {
    C_TRACE (( _T("CModemAtHandler::IsPluginConnected() sessions: %d"), iServer.SessionCount() ));
    return iServer.SessionCount() > 0;
    }

void CModemAtHandler::RemovePipe( const TUint8 aDteId )
    {
    C_TRACE(( _T("CModemAtHandler::RemovePipe()")));
    iDisconnected = ETrue;
#ifndef NCP_COMMON_BRIDGE_FAMILY
    iPipeController->RemovePipe( aDteId );
#endif
    }

TInt CModemAtHandler::Disconnect( const TUint8 aDteId )
    {
    C_TRACE (( _T("CModemAtHandler::Disconnect(%d)"),aDteId ));
    #ifdef __WINSCW__
    iDteId = aDteId;
    return KErrNone;
    #endif
 
    TInt size = ISI_HEADER_SIZE + SIZE_AT_MODEM_DISCONNECT_REQ;
    HBufC8* message = HBufC8::New( size );
    TRACE_ASSERT( message );
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, size );
    isimessage.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,PN_AT_MODEM);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_TID, 0);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_MESSAGEID,
      AT_MODEM_DISCONNECT_REQ);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_DTEID, aDteId);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_FILLERBYTE1, KFiller);
    isimessage.Complete();

    C_TRACE (( _T("CModemAtHandler::Disconnect Sending AT_MODEM_DISCONNECT_REQ")));
    TInt err = iIscApi->Send(messageptr);
    TRACE_ASSERT( err == KErrNone );
    delete message;
    return err;
    }



void CModemAtHandler::HandleModemConnectResp( const TIsiReceiveC& aMessage )
    {
    C_TRACE (( _T("CModemAtHandler::HandleModemConnectResp()") ));
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CONNECT_RESP_OFFSET_DTEID );
    TInt status = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CONNECT_RESP_OFFSET_RESULT );
    TInt err = KErrGeneral;
    if( status == AT_MODEM_RESULT_OK )
        {
        C_TRACE(_L("Connected ok, AT_MODEM_RESULT_OK received"));
        err = KErrNone;
        }
    else
        {
        C_TRACE((_L("Connection failed, AT_MODEM_RESULT_ERROR received (%d)"), status));
        TRACE_ASSERT_ALWAYS;
        }

    iServer.SetDteIdAndConnect( dteId, err );
    }


void CModemAtHandler::HandleSignalInd( const TIsiReceiveC& aMessage )
    {
    C_TRACE (( _T("CModemAtHandler::HandleSignalInd()") ));
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_IND_OFFSET_DTEID );
    iServer.HandleSignalInd( dteId );
    }

void CModemAtHandler::HandleUnsolicitedData( const TIsiReceiveC& aMessage )
    {
    C_TRACE (( _T("CModemAtHandler::HandleUnsolicitedData()") ));
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE+ AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_LENGTH );
    TInt length = aMessage.Get16bit( ISI_HEADER_SIZE+AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_LENGTH );
    
    iServer.HandleUnsolicitedData( dteId,
      aMessage.GetData( ISI_HEADER_SIZE + AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_UNSOLICITEDDATABUFFER,
      length ) );
    }


void CModemAtHandler::HandleIntermediateDataInd( const TIsiReceiveC& aMessage )
    {
    C_TRACE (( _T("CModemAtHandler::HandleIntermediateDataInd()") ));
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_DTEID );
    TInt length = aMessage.Get16bit( ISI_HEADER_SIZE + AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_LENGTH );
     
    TUint transactionId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_TID );
    
    C_TRACE((_L("transactionId from %d"), transactionId));
    transactionId = iLastTransactionId;
    C_TRACE((_L("transactionId after setting it to iLastTransactionId: %d"), transactionId));
    
    TUint8 command = transactionId & KCommandBits; //normal AT-cmd or Nvram-query
    TUint8 pluginType = transactionId >> KTransactionIdBitsShift;
    C_TRACE((_L("dteId %d, command %d (AT=0,NVRAM=1), pluginType = %d (atext=0,common=1)"), dteId, command, pluginType));
    
    iServer.HandleIntermediateDataInd( dteId,
      (TATPluginInterface) pluginType,
      aMessage.GetData( ISI_HEADER_SIZE + AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_INTERMEDIATEDATABUFFER,
      length),
      command );
    }


void CModemAtHandler::HandleRedirectInd( const TIsiReceiveC& aMessage ) 
    {
    C_TRACE (( _T("CModemAtHandler::HandleRedirectInd()") ));
  
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_IND_OFFSET_DTEID );
    TInt newSecondDevId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_IND_OFFSET_DEVICEID );
    TInt newSecondObjId = aMessage.Get16bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_IND_OFFSET_SERVERID );
#ifndef NCP_COMMON_BRIDGE_FAMILY
    iPipeController->RedirectPipe( dteId, newSecondDevId, newSecondObjId ); 
#endif
    }

void CModemAtHandler::SendAtModemDataRedirectResultReq( const TUint8 aDteId, const TUint aResult )
    {
    C_TRACE (( _T("CModemAtHandler::SendAtModemDataRedirectResultReq()") ));
    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_AT_MODEM_DATA_REDIRECT_RESULT_REQ );
    C_TRACE((_L("Sending AT_MODEM_DATA_REDIRECT_RESULT_REQ dteid %d result %d"), aDteId, aResult));
    
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_AT_MODEM_DATA_REDIRECT_RESULT_REQ );
    isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_AT_MODEM);       
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_TID, 0 );
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_MESSAGEID,
      AT_MODEM_DATA_REDIRECT_RESULT_REQ );
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_DTEID ,aDteId );
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_RESULT ,aResult );
    isimessage.Complete();
        
    C_TRACE(_L("Created ISI-message"));
    for( TInt i = 0 ; i < (ISI_HEADER_SIZE + SIZE_AT_MODEM_DATA_REDIRECT_RESULT_REQ); i++ ) 
        {
        C_TRACE((_L("[%d] 0x%x"),i ,(*message)[i]) );
        }

    TInt err = iIscApi->Send( messageptr );
    TRACE_ASSERT( err == KErrNone );
    delete message;
    message = NULL;
    }


void CModemAtHandler::HandleRedirectResultResp(const TIsiReceiveC& aMessage)
    {
    C_TRACE (( _T("CModemAtHandler::HandleRedirectResultResp()") ));
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_DTEID );
    TInt error = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_RESULT );
    C_TRACE((_L("CModemAtHandler::HandleRedirectResultResp() dte %d"), dteId));
    
    if( error == AT_MODEM_RESULT_ERROR ) 
        {
        C_TRACE(_L("CModemAtHandler::HandleRedirectResultResp() AT_MODEM_RESULT_ERROR received, redirect failed"));
        TRACE_ASSERT_ALWAYS;
        }
    else 
        {
        C_TRACE(_L("CModemAtHandler::HandleRedirectResultResp() redirect OK"));
        }
    
    }
void CModemAtHandler::HandleCommandModeChange( TCommandMode aMode )
    {
    C_TRACE(( _T("CModemAtHandler::HandleCommandModeChange() aMode: %d"), (TInt) aMode ));
    iServer.HandleCommandModeChange( aMode );
    }

TInt CModemAtHandler::SendEscapeSignalDetection( const TUint8 aDteId )
    {
    C_TRACE((_T("CModemAtHandler::SendEscapeSignalDetection(%d)"), aDteId));
#ifdef __WINSCW__
    iDteId = aDteId;
    return KErrNone;
#endif
  
    TInt error( KErrGeneral );  
    TUint msgSize( ISI_HEADER_SIZE + SIZE_AT_MODEM_SIGNAL_DETECTED_REQ );
    TUint size( msgSize + SIZE_AT_MODEM_SB_SIGNAL_DETECTED ); 
    HBufC8* message = HBufC8::New( size );
    if( message )
      {
      TPtr8 messageptr = message->Des();
      TIsiSend isimessage( messageptr, size );
      isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_AT_MODEM );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_TID, 0x00 );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_MESSAGEID,
        AT_MODEM_SIGNAL_DETECTED_REQ );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_DTEID, aDteId );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_NOOFSUBBLOCKS, 0x01 );
      isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_SUBBLOCKID,
        AT_MODEM_SB_SIGNAL_DETECTED );
      isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_SUBBLOCKLENGTH,
        SIZE_AT_MODEM_SB_SIGNAL_DETECTED );
      isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_TYPEOFSIGNALDETECTED,
        AT_MODEM_SIGNAL_ESCAPE_DETECTED );
      isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_FILLERBYTE1, 0x00 );
      isimessage.Complete();
      error = iIscApi->Send(messageptr);
      C_TRACE((_L("escape send result %d"), error ));
      delete message;
      message = NULL;
      }
    else
      {
      ASSERT_ALWAYS;
      error = KErrNoMemory;
      }
    return error; 
    }


void CModemAtHandler::HandleSignalDetectedResp( const TIsiReceiveC& aMessage )
    {
    C_TRACE (( _T("CModemAtHandler::HandleSignalDetectedResp() escape sequence detect response received from cellmo") ));
    TUint8 dteId( aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_DTEID ) );
    TInt result = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_RESULT );
    C_TRACE((_L("CModemAtHandler::HandleSignalDetectedResp() dteId %d"), dteId));
    TRACE_ASSERT( result == AT_MODEM_RESULT_OK );
    }

void CModemAtHandler::SetModemAtExistsInCmt( TBool aModemAtExistsInCmt )
    {
    C_TRACE((_T("CModemAtHandler::SetModemAtExistsInCmt(%d)"), (TInt)aModemAtExistsInCmt));
    iModemAtExistInCmt = aModemAtExistsInCmt;
    }

TInt CModemAtHandler::SendAtModemConnectReq( const TUint8 aDteId )
    {
    C_TRACE (( _T("CModemAtHandler::SendAtModemConnectReq()") ));
    TInt size = ISI_HEADER_SIZE + SIZE_AT_MODEM_CONNECT_REQ;
    HBufC8* message = HBufC8::New( size );
    TRACE_ASSERT( message );
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, size );
    isimessage.Set8bit(ISI_HEADER_OFFSET_RESOURCEID, PN_AT_MODEM); 
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_TID, KDefaultTId);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_MESSAGEID, AT_MODEM_CONNECT_REQ);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_DTEID, aDteId);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_FILLERBYTE1, KFiller);
    isimessage.Complete();
    DUMP_MESSAGE( messageptr );
    TInt err = iIscApi->Send( messageptr );
    TRACE_ASSERT( err == KErrNone );
    delete message;
    message = NULL;
    C_TRACE (( _T("CModemAtHandler::SendAtModemConnectReq()<< returned: %d"), err ));
    return err;
    }

