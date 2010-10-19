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



#include <e32std.h>
#include <iscapi.h>
#include <iscnokiadefinitions.h>
#include <pn_const.h>
#include <nsisi.h>
#include <pipeisi.h>
#include <pipe_sharedisi.h>
#include <tisi.h>
#include <at_modemisi.h>
#include "cmodemathandler.h"
#include "cmodematpipecontroller.h"
#include "modemattrace.h"
#include "cmodematsrv.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmodemathandlerTraces.h"
#endif


#define KCommandBits 0x0f
#define KTransactionIdBitsShift 4
#define KFiller 0
#define KIndicationOrderMsgLength 14
#define KObjectIdLength 2
#define KEightBits 8
#define KDefaultTId 0

const TInt KUnsubscribeFourEvents(4);
const TInt KAlignToFourBytes(4);
const TInt KSessionIndLimit(1);        // AT_MODEM_SIGNAL_IND is not handled if only one session is connected
const TInt KEscapeDataLength(2);
const TInt KFlowCtrlDataLength(3);
const TUint8 KReservedDteId( 255 );

#ifdef __WINSCW__
#define KEmulatedReplyMsgLength 50
#endif

CModemAtHandler* CModemAtHandler::NewL( 
    CModemAtSrv& aServer,
    TInt aChannel )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_NEWL, "CModemAtHandler::NewL" );
    CModemAtHandler* self = new (ELeave) CModemAtHandler( aServer, aChannel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CModemAtHandler::~CModemAtHandler()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_CMODEMATHANDLER, "CModemAtHandler::~CModemAtHandler" );
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
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_CMODEMATHANDLER, "CModemAtHandler::~CModemAtHandler - closing isc api" );
        C_TRACE(( _T("CModemAtHandler::~CModemAtHandler() closing isc api") ));
        iIscApi->Close();
        delete iIscApi;
        }
   }

CModemAtHandler::CModemAtHandler( CModemAtSrv& aServer, TInt aChannel )
    : CActive( EPriorityHigh ),
    iServer( aServer ),
    iIscApi( NULL ),
    iReceivedMessageBuffer( NULL ),
    iMessageBufferSize( KMaxBufferLength ),
    iReceivePtr( 0, 0 ),
    iReceiveMessageLength( 0 ),
    iReceivedMessage( NULL ),
    iPepObjId(0),
    iDataportObjId(0),
    iPipeController(NULL),
    iLastTransactionId( 0 ),
    iDisconnected( EFalse ),
    iModemAtExistInCmt( EFalse ),
    iTextMessageMode( EFalse ),
    iChannel( aChannel )
    {
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_CMODEMATHANDLER, "CModemAtHandler::CModemAtHandler" );
    C_TRACE (( _T("CModemAtHandler::CModemAtHandler") ));
    }

void CModemAtHandler::DoCancel()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_DOCANCEL, "CModemAtHandler::DoCancel" );
    C_TRACE (( _T("CModemAtHandler::DoCancel()") ));
    iIscApi->ReceiveCancel();
    }


void CModemAtHandler::RunL()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL" );
    C_TRACE (( _T("CModemAtHandler::RunL() %d"), iDteId ));
   
    if( iStatus == KErrNone ) 
        {
        TIsiReceiveC receiveMessage = TIsiReceiveC( *iReceivedMessageBuffer );
        OstTraceExt1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - message dump;iReceivePtr=%s", iReceivePtr );
        DUMP_MESSAGE( iReceivePtr );
            
        if( iReceivedMessageBuffer->Length() > ISI_HEADER_OFFSET_MESSAGEID )
            {
            switch(receiveMessage.Get8bit(ISI_HEADER_OFFSET_RESOURCEID))
                {
                case PN_AT_MODEM: //AT MODEM COMMANDS
                    OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - PN_AT_MODEM" );
                    C_TRACE(_L("PN_AT_MODEM"));

                    switch( receiveMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
                        {
                        case AT_MODEM_CONNECT_RESP: 
                            HandleModemConnectResp( receiveMessage );
                            break;

                        case AT_MODEM_DISCONNECT_RESP:
                            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - AT_MODEM_DISCONNECT_RESP" );
                            C_TRACE(_L("AT_MODEM_DISCONNECT_RESP"));
                            break;

                        case AT_MODEM_CMD_RESP:
                            HandleATResponse( receiveMessage );
                            break;

                        case AT_MODEM_SIGNAL_IND:
                            HandleSignalInd( receiveMessage );
                            break;

                        case AT_MODEM_UNSOLICITED_DATA_IND:
                            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - AT_MODEM_UNSOLICITED_DATA_IND" );
                            C_TRACE(_L("AT_MODEM_UNSOLICITED_DATA_IND"));
                            HandleUnsolicitedData( receiveMessage );
                            break;

                        case AT_MODEM_INTERMEDIATE_DATA_IND:
                            HandleIntermediateDataInd( receiveMessage );
                            break;

                        case AT_MODEM_DATA_REDIRECT_IND:
                            HandleRedirectInd( receiveMessage );
                            break;
                            
                        case AT_MODEM_DATA_REDIRECT_RESULT_RESP:
                            OstTrace0( TRACE_NORMAL, DUP5_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - AT_MODEM_DATA_REDIRECT_RESULT_RESP" );
                            C_TRACE((_L("AT_MODEM_DATA_REDIRECT_RESULT_RESP")));
                            HandleRedirectResultResp( receiveMessage );
                            break;

                        case AT_MODEM_SIGNAL_DETECTED_RESP:
                            OstTrace0( TRACE_NORMAL, DUP6_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - AT_MODEM_SIGNAL_DETECTED_RESP" );
                            C_TRACE((_L("AT_MODEM_SIGNAL_DETECTED_RESP")));
                            HandleSignalDetectedResp(  receiveMessage  );
                            break;
                        default: 
                            OstTrace0( TRACE_NORMAL, DUP7_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - Not a valid message" );
                            C_TRACE(_L("CModemAtHandler::RunL: Not a valid message"))  ;
                            TRACE_ASSERT_ALWAYS;
                            break;
                         } //switch( receiveMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
                        
                    break;  //case PN_AT_MODEM: //AT MODEM COMMANDS

                case PN_NAMESERVICE: 
                    OstTrace0( TRACE_NORMAL, DUP8_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - PN_NAMESERVICE" );
                    C_TRACE(_L("PN_NAMESERVICE"));
                    iPipeController->HandleNameServiceMessage( receiveMessage );
                    break;

                case PN_PIPE:
                    OstTrace0( TRACE_NORMAL, DUP9_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - PN_PIPE" );
                    C_TRACE(_L("PN_PIPE"));
                    iPipeController->HandlePipeServiceMessage( receiveMessage );
                    break;
                default:  
                    OstTrace0( TRACE_NORMAL, DUP10_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - Unknown resource id" );
                    C_TRACE((_L("Unknown resource id")));
                    TRACE_ASSERT_ALWAYS;
                    break; 
                }

            } //if( receiveMessageBuffer->Length() > ISI_HEADER_OFFSET_MESSAGEID )
        else 
            {
            OstTrace0( TRACE_NORMAL, DUP11_CMODEMATHANDLER_RUNL, "CModemAtHandler::RunL - ISI message is too short" );
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
   OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_CONSTRUCTL, "CModemAtHandler::ConstructL" );
   C_TRACE (( _T("CModemAtHandler::ConstructL()") ));

    CActiveScheduler::Add( this );
    
    iIscApi = new ( ELeave ) RIscApi;
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_CONSTRUCTL, "CModemAtHandler::ConstructL - RIscApi created" );
    C_TRACE(_L("RIscApi created"));
   
#ifndef __WINSCW__
      OpenChannelL();
      if( iChannel == EIscNokiaAtController )
          {
          iPipeController->QueryModemAtFromNameService();
          }
      else
          {
          SetModemAtExistsInCmt( ETrue );
          }
#else
      iEmulatedValue = 0;
#endif
    
    }

TInt CModemAtHandler::SendATCommand(
    const TATPluginInterface aPluginType,
    const TUint8 aMessageType,
    const TDesC8& aCmd)
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand;iDteId=%d", iDteId );
    C_TRACE (( _T(">>CModemAtHandler::SendATCommand() %d"), iDteId ));

    TInt err = KErrNone;
    // aMessageType contains normal AT-cmd (0) or Nvram-query (1)
    TUint transactionId = ( aPluginType << KTransactionIdBitsShift ) | aMessageType;
    // used with intermediate data ind
    // can be removed, when AT_MODEM_INTERMEDIATE_DATA_IND supports transactionid        
    iLastTransactionId = transactionId;  //messages are routed to correct plugin by transaction id
    OstTraceExt4( TRACE_NORMAL, DUP1_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand;transactionId=%d;aPluginType=%d;aMessageType=%d;iLastTransactionId=%d", transactionId, aPluginType, aMessageType, iLastTransactionId );
    C_TRACE((_L("transaction id %d, plugintype %d, aMessageType %d"), transactionId, aPluginType, aMessageType));
    C_TRACE((_L("iLastTransctionId = %d "), iLastTransactionId ));

    TInt size = ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_CMDLINEBUFFER + aCmd.Length();
    
    TInt sizeWithFillers = size;
    while( sizeWithFillers % KAlignToFourBytes )
        {
        sizeWithFillers++;
        }
    OstTrace1( TRACE_NORMAL, DUP2_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand - Message length;sizeWithFillers=%d", sizeWithFillers );
    C_TRACE((_L("Message length %d"), sizeWithFillers));

    HBufC8* message = HBufC8::New( sizeWithFillers );
    if( message )
      {
      TPtr8 messageptr = message->Des();
      TIsiSend isimessage( messageptr, sizeWithFillers );
      isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_AT_MODEM );       
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_TID, transactionId );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_MESSAGEID, AT_MODEM_CMD_REQ );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_DTEID, iDteId );
      isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_FILLERBYTE1, 0 );
      OstTrace1( TRACE_NORMAL, DUP3_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand - AT command length:;aCmd.Length()=%d", aCmd.Length() );
      C_TRACE((_L("AT command length: %d"), aCmd.Length()));
      isimessage.Set16bit( ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_LENGTH, (TUint16) aCmd.Length() );

      const TUint8* ptr = aCmd.Ptr();
      for ( TInt i = 0; i < aCmd.Length(); i++ )
          {
          OstTraceExt3( TRACE_NORMAL, DUP4_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand - message;i=%d;ptr[i]=%x;charptr[i]=%c", i, ptr[i], ptr[i] );
          C_TRACE((_L("message %d, 0x%x '%c'"), i, ptr[i], ptr[i]));
          isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CMD_REQ_OFFSET_CMDLINEBUFFER + i, ptr[i]);
          }

      for( TInt j = sizeWithFillers; j < size; j++ )
          {
          OstTrace1( TRACE_NORMAL, DUP5_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand - Add filler;j=%d", j );
          C_TRACE((_L("Add filler [%d]"), j));
          isimessage.Set8bit( j , KFiller );
          }

      isimessage.Complete();
      OstTraceExt1( TRACE_NORMAL, DUP6_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand - Created ISI-message;messageptr=%s", messageptr );
      C_TRACE(_L("Created ISI-message"));
      DUMP_MESSAGE( messageptr );

      err = iIscApi->Send( messageptr );
      TRACE_ASSERT( err == KErrNone );
      delete message;
      message = NULL;
      }
    else
      {
      OstTrace0( TRACE_NORMAL, DUP7_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand - No memory - error in sending ISI message from heap" );
      C_TRACE(_L("No memory - error in sending ISI message from heap"));
      TRACE_ASSERT_ALWAYS;
      }

    OstTrace1( TRACE_NORMAL, DUP8_CMODEMATHANDLER_SENDATCOMMAND, "CModemAtHandler::SendATCommand;err=%d", err );
    C_TRACE (( _T("<<CModemAtHandler::SendATCommand() err: %d"), err ));
    return err;
    }

void CModemAtHandler::OpenChannelL()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL" );
    C_TRACE (( _T("CModemAtHandler::OpenChannelL()") ));
    TRequestStatus openStatus;

    iIscApi->Open( iChannel, openStatus, EOwnerThread );
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

    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL - Start receive ISI messages" );
    C_TRACE(_L("Start receive ISI messages"));
    iIscApi->Receive( iStatus, iReceivePtr, iReceiveMessageLength );

    SetActive();
    
    OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL - Start receive indications" );
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

#ifndef PIPECAMP_UNSUBSCRIBE_PNS_PEP_STATUS_IND 
    events.Append( PN_PIPE );
    events.Append( PNS_PEP_STATUS_IND );
#endif    
    //name add ind
    events.Append( PN_NAMESERVICE );
    events.Append( PNS_NAME_ADD_IND );  
    events.Append( PN_NAMESERVICE );
    events.Append( PNS_NAME_REMOVE_IND );
    
    iIscApi->CustomFunction( EIscNokiaEventSubscribe, &events );
    TBuf8<KObjectIdLength> objId;
    iIscApi->GetChannelInfo( iChannel, objId );

    iPepObjId = (((TUint16)(objId[0]))<<KEightBits) + objId[1];
    OstTrace1( TRACE_NORMAL, DUP3_CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL;iPepObjId=%x", iPepObjId );
    C_TRACE((_L("PepObjId 0x%x"),iPepObjId));
    iPipeController = CModemAtPipeController::NewL(*iIscApi, iPepObjId, *this); 
    OstTraceExt1( TRACE_NORMAL, DUP4_CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL;iPipeController=%p", iPipeController );
    C_TRACE((_L("iPipeController 0x%x"),iPipeController ));
    ASSERT_PANIC_ALWAYS( iPipeController );
    
    TBuf8<KObjectIdLength> dpObjId;
    OstTrace0( TRACE_NORMAL, DUP5_CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL - GetChannelInfo(EIscNokiaDataport2)" );
    C_TRACE(( _T("GetChannelInfo() EIscNokiaDataport2") ));
    iIscApi->GetChannelInfo( EIscNokiaDataport2, dpObjId );

    OstTrace0( TRACE_NORMAL, DUP6_CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL - GetChannelInfo() done" );
    C_TRACE(( _T("GetChannelInfo() done") ));
    iDataportObjId = (((TUint16)(dpObjId[0])) << KEightBits) + dpObjId[1];
    OstTrace1( TRACE_NORMAL, DUP7_CMODEMATHANDLER_OPENCHANNELL, "CModemAtHandler::OpenChannelL;iDataportObjId=%x", iDataportObjId );
    C_TRACE((_L("DataportObjId 0x%x"), iDataportObjId));
    }


void CModemAtHandler::HandleATResponse( const TIsiReceiveC& aMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_HANDLEATRESPONSE, "CModemAtHandler::HandleATResponse" );
    C_TRACE (( _T("CModemAtHandler::HandleATResponse()") ));
    
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_DTEID ) ;
    TUint transactionId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_TID );
    TUint8 command = transactionId & KCommandBits; //normal AT-cmd or Nvram-query
    TInt length = aMessage.Get16bit( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_LENGTH );
    OstTraceExt3( TRACE_NORMAL, DUP2_CMODEMATHANDLER_HANDLEATRESPONSE, "CModemAtHandler::HandleATResponse;dteId=%d;AT (0) or nvram command (1):;command=%d;Response length;length=%d", dteId, command, length );
    C_TRACE(( _L("DteId: %d AT (0) or nvram command (1): %d Response length %d"), dteId, command, length ));

    /**
     * Plugintype can be read from transaction id:
     * TUint8 pluginType = transactionId >> KTransactionIdBitsShift;
     * (TATPluginInterface) pluginType
     *
     * Current implementation uses first in first out response completion
     * so plugintype is not needed.
     */
    OstTrace1( TRACE_NORMAL, DUP3_CMODEMATHANDLER_HANDLEATRESPONSE, "CModemAtHandler::HandleATResponse - ;Plugintype=%d", transactionId >> KTransactionIdBitsShift );
    C_TRACE(( _L("Plugintype: %d"), transactionId >> KTransactionIdBitsShift ));
    
    iServer.HandleATResponse( 
      dteId,  
      this,
      aMessage.GetData( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_CMDLINEBUFFER, length),
      command );
    }

#ifdef __WINSCW__
void CModemAtHandler::HandleATResponse( )
    {
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_HANDLEATRESPONSE, "CModemAtHandler::HandleATResponse;iDteId=%d", iDteId );
    C_TRACE (( _T("CModemAtHandler::HandleATResponse() %d"), iDteId ));
    //reply back immediately
    const TInt KReplyLength(50);
    TBuf8<KReplyLength> replyEmulated = _L8("\r\nAT TEST REPLY");
    replyEmulated.AppendNum(iEmulatedValue++);
    replyEmulated.Append(_L8("\r\n"));
    iServer.HandleATResponse( iDteId, this, replyEmulated, 0 /* 0 = AT-cmd, 1 = nvram */);
    }
#endif

    
TInt CModemAtHandler::Connect( TUint8 aDevId, TUint8 aObjId )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATHANDLER_CONNECT, "CModemAtHandler::Connect;aDevId=%hhx;aObjId=%hhx", aDevId, aObjId );
    C_TRACE (( _T("CModemAtHandler::Connect() dev id: 0x%x, obj id: 0x%x"), aDevId, aObjId ));

    if( !iModemAtExistInCmt )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_CONNECT, "CModemAtHandler::Connect - Connection failed, PN_AT_MODEM is not in CMT" );
        C_TRACE((_T("Connection failed, PN_AT_MODEM is not in CMT")));
        TRACE_ASSERT_ALWAYS;
        return KErrNotFound;
        }
    SetDisconnected( EFalse );

    iPipeController->SendCreatePipeMessage( aDevId, aObjId );

    return KErrNone;
    }

void CModemAtHandler::SetDisconnected( TBool aIsDisconnected )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_SETDISCONNECTED, "CModemAtHandler::SetDisconnected;aIsDisconnected=%d", aIsDisconnected );
    C_TRACE (( _T("CModemAtHandler::SetDisconnected(%d)"),aIsDisconnected ));
    iDisconnected = aIsDisconnected;
    }

TBool CModemAtHandler::IsDisconnected()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_ISDISCONNECTED, "CModemAtHandler::IsDisconnected;iDisconnected=%d", iDisconnected );
    C_TRACE (( _T("CModemAtHandler::IsDisconnected(%d)"),iDisconnected ));
    return iDisconnected;
    }

TBool CModemAtHandler::IsPluginConnected()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_ISPLUGINCONNECTED, "CModemAtHandler::IsPluginConnected;iServer.SessionCount()=%d", iServer.SessionCount() );
    C_TRACE (( _T("CModemAtHandler::IsPluginConnected() sessions: %d"), iServer.SessionCount() ));
    return iServer.SessionCount() > 0;
    }

void CModemAtHandler::RemovePipe()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_REMOVEPIPE, "CModemAtHandler::RemovePipe;iDteId=%d", iDteId );
    C_TRACE(( _T("CModemAtHandler::RemovePipe() %d"), iDteId ));
    iDisconnected = ETrue;
    iPipeController->RemovePipe();
    }

TInt CModemAtHandler::Disconnect()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_DISCONNECT, "CModemAtHandler::Disconnect;iDteId=%d", iDteId );
    C_TRACE (( _T("CModemAtHandler::Disconnect(%d)"),iDteId ));
 
    TInt size = ISI_HEADER_SIZE + SIZE_AT_MODEM_DISCONNECT_REQ;
    HBufC8* message = HBufC8::New( size );
    TRACE_ASSERT( message );
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, size );
    isimessage.Set8bit(ISI_HEADER_OFFSET_RESOURCEID,PN_AT_MODEM);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_TID, 0);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_MESSAGEID,
      AT_MODEM_DISCONNECT_REQ);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_DTEID, iDteId);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_DISCONNECT_REQ_OFFSET_FILLERBYTE1, KFiller);
    isimessage.Complete();

    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_DISCONNECT, "CModemAtHandler::Disconnect - Sending AT_MODEM_DISCONNECT_REQ" );
    C_TRACE (( _T("CModemAtHandler::Disconnect Sending AT_MODEM_DISCONNECT_REQ")));
    TInt err = iIscApi->Send(messageptr);
    TRACE_ASSERT( err == KErrNone );
    delete message;
    return err;
    }



void CModemAtHandler::HandleModemConnectResp( const TIsiReceiveC& aMessage )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_HANDLEMODEMCONNECTRESP, "CModemAtHandler::HandleModemConnectResp;iDteId=%d", iDteId );
    C_TRACE (( _T("CModemAtHandler::HandleModemConnectResp() %d"), iDteId ));
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CONNECT_RESP_OFFSET_DTEID );
    TInt status = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CONNECT_RESP_OFFSET_RESULT );
    TInt err = KErrGeneral;
    if( status == AT_MODEM_RESULT_OK )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_HANDLEMODEMCONNECTRESP, "CModemAtHandler::HandleModemConnectResp - Connected ok, AT_MODEM_RESULT_OK received" );
        C_TRACE(_L("Connected ok, AT_MODEM_RESULT_OK received"));
        err = KErrNone;
        }
    else
        {
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATHANDLER_HANDLEMODEMCONNECTRESP, "CModemAtHandler::HandleModemConnectResp - Connection failed, AT_MODEM_RESULT_ERROR received;status=%d", status );
        C_TRACE((_L("Connection failed, AT_MODEM_RESULT_ERROR received (%d)"), status));
        }

    iServer.SetDteIdAndConnect( dteId, err );
    }


void CModemAtHandler::HandleSignalInd( const TIsiReceiveC& aMessage )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd;iDteId=%d", iDteId );
    C_TRACE (( _T("CModemAtHandler::HandleSignalInd() (%d)"), iDteId ));

    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_IND_OFFSET_DTEID );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd;dteId=%x", dteId );
    C_TRACE(( _L("dteId: 0x%x"), dteId ));

    if( iDteId != dteId )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - AT_MODEM_SIGNAL_IND is ignored because of it's received from diffrent dte." );
        C_TRACE (( _T("AT_MODEM_SIGNAL_IND is ignored because of it's received from diffrent dte.") ));
        return;
        }

    if( IsTextMessageMode() )
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - AT_MODEM_SIGNAL_IND is ignored during text message mode." );
        C_TRACE (( _T("AT_MODEM_SIGNAL_IND is ignored during text message mode.") ));
        return;
        }

    // Go through indications. Note that common plugin handles only these:
    // echo, verbose, quiet, escape char, linefeed char and backspace char.

    

    TUint8 count = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_IND_OFFSET_NOOFSUBBLOCKS );
    OstTrace1( TRACE_NORMAL, DUP4_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - subblock;count=%d", count );
    C_TRACE(( _L("subblock count: 0x%x"), count ));
    if( count > 0 )
        {
        TInt length( ISI_HEADER_SIZE + SIZE_AT_MODEM_SIGNAL_IND );
        TUint8 messageId( 0 );
        TUint8 echoValue( KInitialValue );
        TUint8 escapeChar( KInitialValue );
        TUint8 flowControlOutward( AT_MODEM_FLOW_CONTROL_SW );
        TUint8 flowControlInward( AT_MODEM_FLOW_CONTROL_SW );
        TUint8 flowControlExtract( AT_MODEM_WITH_EXTRACT );
        TUint8 dcdValue( AT_MODEM_DCD_OFF );
        TUint8 dsrValue( AT_MODEM_DSR_OFF );
        for( TInt i = 0; i < count; i++ )
            {
            messageId = aMessage.Get8bit( length );
            OstTrace1( TRACE_NORMAL, DUP5_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd;messageId=%x", messageId );
            C_TRACE((_L("messageId: 0x%x"), messageId));

            switch( messageId )
                {
                case AT_MODEM_SB_ECHO_VALUE:
                    {
                    echoValue = aMessage.Get8bit( length +
                      AT_MODEM_SB_ECHO_VALUE_OFFSET_ECHOVALUE );
                    OstTrace1( TRACE_NORMAL, DUP6_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd;echoValue=%x", echoValue );
                    C_TRACE(( _L("echo value: 0x%x"), echoValue));
                    length += SIZE_AT_MODEM_SB_ECHO_VALUE;
                    break;
                    }
                case AT_MODEM_SB_DTE_PARAMS:
                    {
                    length += SIZE_AT_MODEM_SB_DTE_PARAMS;
                    break;
                    }
                case AT_MODEM_SB_FLOW_CTRL:
                    {
                    flowControlOutward = aMessage.Get8bit( length +
                      AT_MODEM_SB_FLOW_CTRL_OFFSET_FLOWCTRLOUTWARDDCE );
                    C_TRACE(( _L("Flow control outward used by DCE towards DTE: 0x%x"), flowControlOutward ));
                    
                    flowControlInward = aMessage.Get8bit( length +
                      AT_MODEM_SB_FLOW_CTRL_OFFSET_FLOWCTRLINWARDDTE );
                    C_TRACE(( _L("Flow control inward from DTE: 0x%x"), flowControlInward ));
                    
                    flowControlExtract = aMessage.Get8bit( length +
                      AT_MODEM_SB_FLOW_CTRL_OFFSET_EXTRACT );
                    C_TRACE(( _L("extract: 0x%x"), flowControlExtract ));

                    TBuf8<KFlowCtrlDataLength> data;
                    switch( flowControlOutward )
                        {
                        case AT_MODEM_FLOW_CONTROL_SW:
                            data.Append( PEP_COMM_FLOW_CTRL_SW );
                            break;
                        case AT_MODEM_FLOW_CONTROL_NONE:
                            data.Append( PEP_COMM_FLOW_CTRL_NONE );
                            break;
                        case AT_MODEM_FLOW_CONTROL_HW:
                            data.Append( PEP_COMM_FLOW_CTRL_HW );
                            break;
                        default:
                            data.Append( PEP_COMM_FLOW_CTRL_SW );
                            break;
                        }
                     switch( flowControlInward )
                        {
                        case AT_MODEM_FLOW_CONTROL_SW:
                            data.Append( PEP_COMM_FLOW_CTRL_SW );
                            break;
                        case AT_MODEM_FLOW_CONTROL_NONE:
                            data.Append( PEP_COMM_FLOW_CTRL_NONE );
                            break;
                        case AT_MODEM_FLOW_CONTROL_HW:
                            data.Append( PEP_COMM_FLOW_CTRL_HW );
                            break;
                        default:
                            data.Append( PEP_COMM_FLOW_CTRL_SW );
                            break;
                        }
                    switch( flowControlExtract )
                        {
                        case AT_MODEM_NO_EXTRACT:
                            data.Append( PEP_COMM_FLOW_CTRL_NO_EXTRACT );
                            break;
                        case AT_MODEM_WITH_EXTRACT:
                            data.Append( PEP_COMM_FLOW_CTRL_EXTRACT );
                            break;
                        default:
                            data.Append( PEP_COMM_FLOW_CTRL_EXTRACT );
                            break;
                        }

                    TPipeInfo::EPipeState pipeState = iPipeController->GetPipeState();
                    if( pipeState != TPipeInfo::EPipeNoPipe &&
                        pipeState != TPipeInfo::EPipeRemoving )
                        {
                        iPipeController->SendPepCtrlReq( PEP_COMM_CTRL_ID_FLOW_CTRL, data );
                        }
                    length += SIZE_AT_MODEM_SB_FLOW_CTRL;
                    break;
                    }
                case AT_MODEM_SB_ESCAPE_PARAMS:
                    {
                    escapeChar = aMessage.Get8bit( length +
                      AT_MODEM_SB_ESCAPE_PARAMS_OFFSET_ESCAPECHAR );
                    OstTrace1( TRACE_NORMAL, DUP7_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd;escapeChar=%x", escapeChar );
                    C_TRACE(( _L("escape character: 0x%x"),  escapeChar ));
                    length += SIZE_AT_MODEM_SB_ESCAPE_PARAMS;
                    break;
                    }
                case AT_MODEM_SB_AUTOBAUD:
                    {
                    length += SIZE_AT_MODEM_SB_AUTOBAUD;
                    break;
                    }
                case AT_MODEM_SB_DTE_SIGNALS:
                    {
                   dcdValue = aMessage.Get8bit( length +
                      AT_MODEM_SB_DTE_SIGNALS_OFFSET_DCDSIGNAL );
                    OstTrace1( TRACE_NORMAL, DUP8_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd;dcdValue=%x", dcdValue );
					C_TRACE(( _L("DCD value: 0x%x"), dcdValue));
                    
                    dsrValue = aMessage.Get8bit( length +
                      AT_MODEM_SB_DTE_SIGNALS_OFFSET_DSRSIGNAL );
                    OstTrace1( TRACE_NORMAL, DUP9_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd;dsrValue=%x", dsrValue );
					C_TRACE(( _L("DSR value: 0x%x"), dsrValue));
                    TPipeInfo::EPipeState pipeState = iPipeController->GetPipeState();
                    if( pipeState != TPipeInfo::EPipeNoPipe &&
                        pipeState != TPipeInfo::EPipeRemoving )
                        {
                        if( AT_MODEM_DCD_OFF == dcdValue )
                            {
                            iPipeController->SendPepStatusIndSignal( PEP_COMM_SIGNAL_DCD_OFF );
                            }
                        else if( AT_MODEM_DCD_ON == dcdValue )
                            {
                            iPipeController->SendPepStatusIndSignal( PEP_COMM_SIGNAL_DCD_ON );
                            }
                        
                        if( AT_MODEM_DSR_OFF == dsrValue )
                            {
                            iPipeController->SendPepStatusIndSignal( PEP_COMM_SIGNAL_DSR_OFF );
                            }
                        else if( AT_MODEM_DSR_ON == dsrValue )
                            {
                            iPipeController->SendPepStatusIndSignal( PEP_COMM_SIGNAL_DSR_ON );
                            }
                        }
                    length += SIZE_AT_MODEM_SB_DTE_SIGNALS;
                    break;
                    }
                default:
                    {
                    TRACE_ASSERT_ALWAYS;
                    break;
                    }
                }
            }

        if( escapeChar != KInitialValue )
            {
            OstTrace0( TRACE_NORMAL, DUP10_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - Escape char received, it will be sent to dataport." );
            C_TRACE(( _T("Escape char received, it will be sent to dataport.") ));

            // Dataport should have the pipehandle and escape character. It defaults to handle 0xff and escape char '+'.
            TPipeInfo::EPipeState pipeState = iPipeController->GetPipeState();
            if( pipeState != TPipeInfo::EPipeNoPipe &&
                pipeState != TPipeInfo::EPipeRemoving )
                {
                TBuf8<KEscapeDataLength> data;
                data.Append( escapeChar );
                data.Append( KEscapeTime );
                iPipeController->SendPepCtrlReq( PEP_COMM_CTRL_ID_ESCAPE, data );
                }

            }
        if( IsPipeInDataMode() )
            {
            OstTrace0( TRACE_NORMAL, DUP11_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - AT_MODEM_SIGNAL_IND is ignored during data mode (exception: escape char is allowed)" );
            C_TRACE (( _T("AT_MODEM_SIGNAL_IND is ignored during data mode (exception: escape char is allowed)") ));
            return;
            }

        if( echoValue == KInitialValue )
            {
            OstTrace0( TRACE_NORMAL, DUP12_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - AT_MODEM_SIGNAL_IND not handled (no echo changes)" );
            C_TRACE(( _T("AT_MODEM_SIGNAL_IND not handled (no echo changes)") ));
            return;
            }
        TInt sessionCount = iServer.SessionCount();
        if( sessionCount == KSessionIndLimit )
            {
            OstTrace0( TRACE_NORMAL, DUP13_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - AT_MODEM_SIGNAL_IND not handled (connect ongoing)" );
            C_TRACE(( _T("AT_MODEM_SIGNAL_IND not handled (connect ongoing)") ));
            return;
            }
        if( iServer.IsSettingQueryOngoing() )
            {
            // V, Q or E command is not ongoing, handle AT_MODEM_SIGNAL_IND always.
            OstTrace0( TRACE_NORMAL, DUP14_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - AT_MODEM_SIGNAL_IND not handled (Q, V or E query ongoing)" );
            C_TRACE(( _T("AT_MODEM_SIGNAL_IND not handled (Q, V or E query ongoing)") ));
            return;
            }

        if( !iServer.IsEchoNotified( echoValue ) )
            {
            OstTrace0( TRACE_NORMAL, DUP15_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - Handling AT_MODEM_SIGNAL_IND (new echo value received, more than 1 session open)" );
            C_TRACE(( _T("Handling AT_MODEM_SIGNAL_IND (new echo value received, more than 1 session open)") ));
            iServer.HandleSignalInd( dteId, KErrNone );
            return;
            }

        OstTrace1( TRACE_NORMAL, DUP16_CMODEMATHANDLER_HANDLESIGNALIND, "CModemAtHandler::HandleSignalInd - AT_MODEM_SIGNAL_IND not handled.;sessionCount=%d", sessionCount );
        C_TRACE(( _L("AT_MODEM_SIGNAL_IND not handled. sessionCount: %d"),  sessionCount ));
        }
    // no else
    }

void CModemAtHandler::HandleUnsolicitedData( const TIsiReceiveC& aMessage )
    {
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE+AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_DTEID );
    TInt length = aMessage.Get16bit( ISI_HEADER_SIZE+AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_LENGTH );
    OstTraceExt2( TRACE_NORMAL, CMODEMATHANDLER_HANDLEUNSOLICITEDDATA, "CModemAtHandler::HandleUnsolicitedData;dteId=%d;iDteId=%d", dteId, iDteId );
    C_TRACE (( _T("CModemAtHandler::HandleUnsolicitedData() %d %d"), dteId, iDteId ));
    if( iDteId == dteId ||
        KReservedDteId == dteId )
        {
        iServer.HandleUnsolicitedData( dteId,
          aMessage.GetData( ISI_HEADER_SIZE + AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_UNSOLICITEDDATABUFFER,
          length ) );
        }
    // no else
    }


void CModemAtHandler::HandleIntermediateDataInd( const TIsiReceiveC& aMessage )
    {
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_DTEID );
    TInt length = aMessage.Get16bit( ISI_HEADER_SIZE + AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_LENGTH );
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_HANDLEINTERMEDIATEDATAIND, "CModemAtHandler::HandleIntermediateDataInd;iDteId=%d", iDteId );
    C_TRACE (( _T("CModemAtHandler::HandleIntermediateDataInd() %d"), iDteId ));

    if( iDteId == dteId )
        {
        TUint transactionId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_CMD_RESP_OFFSET_TID );

        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_HANDLEINTERMEDIATEDATAIND, "CModemAtHandler::HandleIntermediateDataInd - transactionId from;transactionId=%d", transactionId );
        C_TRACE((_L("transactionId from %d"), transactionId));
        transactionId = iLastTransactionId;
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATHANDLER_HANDLEINTERMEDIATEDATAIND, "CModemAtHandler::HandleIntermediateDataInd - transactionId after setting it to iLastTransactionId:;transactionId=%d", transactionId );
        C_TRACE((_L("transactionId after setting it to iLastTransactionId: %d"), transactionId));

        TUint8 command = transactionId & KCommandBits; //normal AT-cmd or Nvram-query
        TUint8 pluginType = transactionId >> KTransactionIdBitsShift;
        OstTraceExt4( TRACE_NORMAL, DUP3_CMODEMATHANDLER_HANDLEINTERMEDIATEDATAIND, "CModemAtHandler::HandleIntermediateDataInd;dteId=%d;command=%d;(AT=0,NVRAM=1);pluginType=%d;(atext=0,common=1);iDteId=%d", dteId, command, pluginType, iDteId );
        C_TRACE((_L("dteId %d, command %d (AT=0,NVRAM=1), pluginType = %d (atext=0,common=1)"), dteId, command, pluginType));

        iServer.HandleIntermediateDataInd( dteId,
            (TATPluginInterface) pluginType,
            aMessage.GetData( ISI_HEADER_SIZE + AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_INTERMEDIATEDATABUFFER,
            length),
            command );
        }
    }


void CModemAtHandler::HandleRedirectInd( const TIsiReceiveC& aMessage ) 
    {
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_IND_OFFSET_DTEID );
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_HANDLEREDIRECTIND, "CModemAtHandler::HandleRedirectInd;iChannel=%d", iChannel );
    C_TRACE (( _T("CModemAtHandler::HandleRedirectInd() %d %d"), iDteId, iChannel ));
    if( iDteId == dteId )
        {
        TInt newSecondDevId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_IND_OFFSET_DEVICEID );
        TInt newSecondObjId = aMessage.Get16bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_IND_OFFSET_SERVERID );
        iPipeController->RedirectPipe( newSecondDevId, newSecondObjId ); 
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    }

void CModemAtHandler::SendAtModemDataRedirectResultReq( const TUint aResult )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_SENDATMODEMDATAREDIRECTRESULTREQ, "CModemAtHandler::SendAtModemDataRedirectResultReq" );
    C_TRACE (( _T("CModemAtHandler::SendAtModemDataRedirectResultReq()") ));
    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_AT_MODEM_DATA_REDIRECT_RESULT_REQ );
    OstTraceExt2( TRACE_NORMAL, DUP1_CMODEMATHANDLER_SENDATMODEMDATAREDIRECTRESULTREQ, "CModemAtHandler::SendAtModemDataRedirectResultReq - Sending AT_MODEM_DATA_REDIRECT_RESULT_REQ;iDteId=%d;aResult=%d", (TUint)iDteId, aResult );
    C_TRACE((_L("Sending AT_MODEM_DATA_REDIRECT_RESULT_REQ dteid %d result %d"), iDteId, aResult));
    
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_AT_MODEM_DATA_REDIRECT_RESULT_REQ );
    isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_AT_MODEM);       
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_TID, 0 );
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_MESSAGEID,
      AT_MODEM_DATA_REDIRECT_RESULT_REQ );
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_DTEID ,iDteId );
    isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_RESULT ,aResult );
    isimessage.Complete();
        
    OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_SENDATMODEMDATAREDIRECTRESULTREQ, "CModemAtHandler::SendAtModemDataRedirectResultReq - Created ISI-message" );
    C_TRACE(_L("Created ISI-message"));
    for( TInt i = 0 ; i < (ISI_HEADER_SIZE + SIZE_AT_MODEM_DATA_REDIRECT_RESULT_REQ); i++ ) 
        {
        OstTraceExt2( TRACE_NORMAL, DUP3_CMODEMATHANDLER_SENDATMODEMDATAREDIRECTRESULTREQ, "CModemAtHandler - ;i=%d;%x", (TUint)i, (TUint)((*message)[i]) );
        C_TRACE((_L("[%d] 0x%x"),i ,(*message)[i]) );
        }

    TInt err = iIscApi->Send( messageptr );
    TRACE_ASSERT( err == KErrNone );
    delete message;
    message = NULL;
    }


void CModemAtHandler::HandleRedirectResultResp(const TIsiReceiveC& aMessage)
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_HANDLEREDIRECTRESULTRESP, "CModemAtHandler::HandleRedirectResultResp;iDteId=%d", iDteId );
    C_TRACE (( _T("CModemAtHandler::HandleRedirectResultResp() %d"), iDteId ));
    TUint8 dteId = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_DTEID );
    TInt error = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_RESULT );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_HANDLEREDIRECTRESULTRESP, "CModemAtHandler::HandleRedirectResultResp;dteId=%d", dteId );
    C_TRACE((_L("CModemAtHandler::HandleRedirectResultResp() dte %d"), dteId));
    
    if( error == AT_MODEM_RESULT_ERROR ) 
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_HANDLEREDIRECTRESULTRESP, "CModemAtHandler::HandleRedirectResultResp - AT_MODEM_RESULT_ERROR -> COMMAND MODE" );
        C_TRACE(_L("AT_MODEM_RESULT_ERROR -> COMMAND MODE"));
        iServer.HandleCommandModeChange( dteId, ECommandMode );
        }
    else 
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATHANDLER_HANDLEREDIRECTRESULTRESP, "CModemAtHandler::HandleRedirectResultResp - redirect OK, enable pipe" );
        C_TRACE(_L("CModemAtHandler::HandleRedirectResultResp() redirect OK"));
        TPipeInfo::EPipeState pipeState = iPipeController->GetPipeState();
        OstTrace1( TRACE_NORMAL, DUP4_CMODEMATHANDLER_HANDLEREDIRECTRESULTRESP, "CModemAtHandler::HandleRedirectResultResp;pipeState=%d", pipeState );
        C_TRACE((_L("CModemAtHandler::HandleRedirectResultResp() pipe state %d"), pipeState));
        if( pipeState != TPipeInfo::EPipeDisabled )
            {
            iPipeController->SendEnablePipeReq();
            }
        }
    }

void CModemAtHandler::HandleCommandModeChange( TCommandMode aMode )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATHANDLER_HANDLECOMMANDMODECHANGE, "CModemAtHandler::HandleCommandModeChange;aMode=%d;iDteId=%d", (TUint)aMode, (TUint)iDteId );
    C_TRACE(( _T("CModemAtHandler::HandleCommandModeChange() aMode: %d dteId: %d"), (TInt) aMode, iDteId ));
    iServer.HandleCommandModeChange( iDteId, aMode );
    }

TInt CModemAtHandler::SendSignalDetection( const TUint8 aSignal )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_SENDSIGNALDETECTION, "CModemAtHandler::SendSignalDetection;iDteId=%d", iDteId );
    C_TRACE((_T("CModemAtHandler::SendSignalDetection( dteId: %d, aSignal: 0x%x)"), iDteId, aSignal ));

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
        isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_DTEID, iDteId );
        isimessage.Set8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_NOOFSUBBLOCKS, 0x01 );
        isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_SUBBLOCKID,
          AT_MODEM_SB_SIGNAL_DETECTED );
        isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_SUBBLOCKLENGTH,
          SIZE_AT_MODEM_SB_SIGNAL_DETECTED );
        isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_TYPEOFSIGNALDETECTED,
          aSignal );
        isimessage.Set8bit( msgSize + AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_FILLERBYTE1, 0x00 );
        isimessage.Complete();
        error = iIscApi->Send(messageptr);
        TRACE_ASSERT( error == KErrNone );
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
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_HANDLESIGNALDETECTEDRESP, "CModemAtHandler::HandleSignalDetectedResp() - escape sequence detect response received from cellmo" );
    C_TRACE (( _T("CModemAtHandler::HandleSignalDetectedResp()") ));
    TUint8 dteId( aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_DTEID ) );
    TInt result = aMessage.Get8bit( ISI_HEADER_SIZE + AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_RESULT );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_HANDLESIGNALDETECTEDRESP, "CModemAtHandler::HandleSignalDetectedResp;dteId=%d", dteId );
    C_TRACE((_L("CModemAtHandler::HandleSignalDetectedResp() dteId %d"), dteId));
    TRACE_ASSERT( result == AT_MODEM_RESULT_OK );
    }

void CModemAtHandler::SetModemAtExistsInCmt( TBool aModemAtExistsInCmt )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_SETMODEMATEXISTSINCMT, "CModemAtHandler::SetModemAtExistsInCmt;aModemAtExistsInCmt=%d", (TInt)aModemAtExistsInCmt );
    C_TRACE((_T("CModemAtHandler::SetModemAtExistsInCmt(%d)"), (TInt)aModemAtExistsInCmt));
    iModemAtExistInCmt = aModemAtExistsInCmt;
    }

TInt CModemAtHandler::SendAtModemConnectReq()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_SENDATMODEMCONNECTREQ, "CModemAtHandler::SendAtModemConnectReq;iDteId=%d", iDteId );
    C_TRACE (( _T("CModemAtHandler::SendAtModemConnectReq() %d"), iDteId ));
    TInt size = ISI_HEADER_SIZE + SIZE_AT_MODEM_CONNECT_REQ;
    HBufC8* message = HBufC8::New( size );
    TRACE_ASSERT( message );
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, size );
    isimessage.Set8bit(ISI_HEADER_OFFSET_RESOURCEID, PN_AT_MODEM); 
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_TID, KDefaultTId);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_MESSAGEID, AT_MODEM_CONNECT_REQ);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_DTEID, iDteId);
    isimessage.Set8bit(ISI_HEADER_SIZE + AT_MODEM_CONNECT_REQ_OFFSET_FILLERBYTE1, KFiller);
    isimessage.Complete();
    OstTraceExt1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_SENDATMODEMCONNECTREQ, "CModemAtHandler::SendAtModemConnectReq - message dump;messageptr=%s", messageptr );
    DUMP_MESSAGE( messageptr );
    TInt err = iIscApi->Send( messageptr );
    TRACE_ASSERT( err == KErrNone );
    delete message;
    message = NULL;
    OstTrace1( TRACE_NORMAL, DUP2_CMODEMATHANDLER_SENDATMODEMCONNECTREQ, "CModemAtHandler::SendAtModemConnectReq - return;err=%d", err );
    C_TRACE (( _T("CModemAtHandler::SendAtModemConnectReq()<< returned: %d"), err ));
    return err;
    }

TBool CModemAtHandler::IsPipeInDataMode()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_ISPIPEINDATAMODE, "CModemAtHandler::IsPipeInDataMode;iDteId=%d", iDteId );
    C_TRACE(( _T("CModemAtHandler::IsPipeInDataMode() dteId: %d"), iDteId ));
    TPipeInfo::EPipeState pipeState( iPipeController->GetPipeState() );
    if( pipeState == TPipeInfo::EPipeEnabled ||
        pipeState == TPipeInfo::EPipeRedirected )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_ISPIPEINDATAMODE, "CModemAtHandler::IsPipeInDataMode - True" );
        C_TRACE(( _T("CModemAtHandler::IsPipeInDataMode() True") ));
        return ETrue;
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_ISPIPEINDATAMODE, "CModemAtHandler::IsPipeInDataMode - False" );
        C_TRACE(( _T("CModemAtHandler::IsPipeInDataMode() False") ));
        return EFalse;
        }
    }


void CModemAtHandler::SetTextMessageMode( TBool aTextMode )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATHANDLER_SETTEXTMESSAGEMODE, "CModemAtHandler::SetTextMessageMode(;aTextMode=%d;);iDteId=%d", (TUint)aTextMode, (TUint)iDteId );
    C_TRACE (( _T("CModemAtHandler::SetTextMessageMode( aTextMode: %d ) dte id: %d"), (TInt) aTextMode, iDteId ));
    iTextMessageMode = aTextMode;
    }

TBool CModemAtHandler::IsTextMessageMode()
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATHANDLER_ISTEXTMESSAGEMODE, "CModemAtHandler::IsTextMessageMode;iTextMessageMode=%d;iDteId=%d", (TUint)iTextMessageMode, (TUint)iDteId );
    C_TRACE (( _T("CModemAtHandler::IsTextMessageMode() iTextMessageMode: %d dte id: %d"), (TInt) iTextMessageMode, iDteId ));
    return iTextMessageMode;
    }

void CModemAtHandler::PipeCreated( TUint8 aPipeHandle, TInt aError )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_PIPECREATED, "CModemAtHandler::PipeCreated;aPipeHandle=%d", aPipeHandle );
    C_TRACE(( _T("CModemAtHandler::PipeCreated() pipehandle: %d"), aPipeHandle ));

    iDteId = aPipeHandle;

    if( KErrNone == aError )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_PIPECREATED, "CModemAtHandler::PipeCreated - pipe created, let's connect to modem" );
        C_TRACE(( _T("CModemAtHandler::pipeCreated(): pipe created, let's connect to modem") ));
        SendAtModemConnectReq();
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_PIPECREATED, "CModemAtHandler::PipeCreated - pipe creation has failed, connect is aborted" );
        C_TRACE(( _T("CModemAtHandler::pipeCreated(): pipe creation has failed, connect is aborted") ));
        iServer.SetDteIdAndConnect( KInvalidPipeHandle, aError );
        }
    }
    

void CModemAtHandler::AddDevice( TUint8 aDevId, TUint8 aObjId )
    {
	OstTraceExt2( TRACE_NORMAL, CMODEMATHANDLER_ADDDEVICE, "CModemAtHandler::AddDevice;aDevId=%x;aObjId=%x", aDevId, aObjId );
	C_TRACE(( _T("CModemAtHandler::AddDevice() aDevId: 0x%x, aObjId: 0x%x"), aDevId, aObjId ));
    iServer.AddDevice( aDevId, aObjId );
    }


void CModemAtHandler::RemoveDevice( TUint8 aDevId, TUint8 aObjId )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATHANDLER_REMOVEDEVICE, "CModemAtHandler::RemoveDevice;aDevId=%hhu;aObjId=%hhu", aDevId, aObjId );
    C_TRACE(( _T("CModemAtHandler::RemoveDevice() aDevId: 0x%x, aObjId: 0x%x"), aDevId, aObjId ));
    iServer.RemoveDevice( aDevId, aObjId );
    }

void CModemAtHandler::GetDataportObjectId( TUint& aObjId )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_GETDATAPORTOBJECTID, "CModemAtHandler::GetDataportObjectId" );
    C_TRACE(( _T("CModemAtHandler::GetDataportObjectId()") ));
    aObjId = iDataportObjId;
    }

void CModemAtHandler::GetSecondaryDataportObjectId( TUint& aObjId )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATHANDLER_GETSECONDARYDATAPORTOBJECTID, "CModemAtHandler::GetSecondaryDataportObjectId" );
    C_TRACE(( _T("CModemAtHandler::GetScondaryDataportObjectId()") ));
    TBuf8<KObjectIdLength> dpObjId;
    
    if( iIscApi )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATHANDLER_GETSECONDARYDATAPORTOBJECTID, "CModemAtHandler::GetSecondaryDataportObjectId - call GetChannelInfo(EIscNokiaDataport4) " );
        C_TRACE(( _T("GetChannelInfo() EIscNokiaDataport4") ));
        iIscApi->GetChannelInfo( EIscNokiaDataport4, dpObjId );
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATHANDLER_GETSECONDARYDATAPORTOBJECTID, "CModemAtHandler::GetSecondaryDataportObjectId - GetChannelInfo() done" );
        C_TRACE(( _T("GetChannelInfo() done") ));
        aObjId = (((TUint16)(dpObjId[0])) << KEightBits) + dpObjId[1];
        OstTrace1( TRACE_NORMAL, DUP3_CMODEMATHANDLER_GETSECONDARYDATAPORTOBJECTID, "CModemAtHandler::GetSecondaryDataportObjectId;aObjId=%d", aObjId );
        C_TRACE((_L("DataportObjId 0x%x"), aObjId ));
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP4_CMODEMATHANDLER_GETSECONDARYDATAPORTOBJECTID, "CModemAtHandler::GetSecondaryDataportObjectId - No IscApi found, using default dataport object id" );
        C_TRACE(( _T("No IscApi found, using default dataport object id") ));
        }
    }


TInt CModemAtHandler::GetChannelId()
    {
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATHANDLER_GETCHANNELID, "CModemAtHandler::GetChannelId;iChannel=%x", iChannel );
    OstTrace1( TRACE_NORMAL, CMODEMATHANDLER_GETCHANNELID, "CModemAtHandler::GetChannelId;this=%x", this );
    C_TRACE(( _T("CModemAtHandler::GetChannelId() handler: 0x%x, channel id: 0x%x"), this, iChannel ));
    return iChannel;
    }

