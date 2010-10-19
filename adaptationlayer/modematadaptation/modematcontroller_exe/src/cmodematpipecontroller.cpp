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



#include <iscapi.h>                 //For ISC
#include <iscnokiadefinitions.h>    //isc definitions
#include <pn_const.h>
#include <nsisi.h>
#include <pipeisi.h>                //pipe
#include <pipe_sharedisi.h>         //pipe
#include <tisi.h>                   //for isimessage
#include "cmodematpipecontroller.h"
#include "cmodemathandler.h"
#include <at_modemisi.h>            //redirectreq error codes
#include "modemattrace.h"
#include "cmodematsrv.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmodematpipecontrollerTraces.h"
#endif


const TUint8 KFiller = 0;
const TUint8 KDefaultTrId = 0;
const TInt KLastByteIndex = 3;
const TUint8 KDpPadding = 0;
const TUint8 KDialUpPort1 = 1;// from Dataport export (otherwise hard coded and incorrect)
const TUint8 KDialUpPort4 = 4;
const TUint8 KDialUpPort5 = 5;
const TUint8 KPepCtrlDataSize = 1;
const TUint8 KPepFlowCtrlDataSize = 2;

CModemAtPipeController* CModemAtPipeController::NewL( RIscApi& aIscApi,
    TUint& aObjId,
    CModemAtHandler& aHandler )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_NEWL, "CModemAtPipeController::NewL" );
    CModemAtPipeController* self = new (ELeave) CModemAtPipeController( aIscApi,aObjId, aHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CModemAtPipeController::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_CONSTRUCTL, "CModemAtPipeController::ConstructL" );
    C_TRACE (( _T("CModemAtPipeController::ConstructL()") ));
    }

CModemAtPipeController::~CModemAtPipeController() 
     {
     OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_CMODEMATPIPECONTROLLER, "CModemAtPipeController::~CModemAtPipeController" );
     C_TRACE((_L(">>CModemAtPipeController::~CModemAtPipeController()")));
     if( iSchedulerWait )
        {
        C_TRACE((_L("CModemAtPipeController::~CModemAtPipeController() iSchedulerWait->Stop")));
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_CMODEMATPIPECONTROLLER, "CModemAtPipeController::~CModemAtPipeController - iSchedulerWait->Stop" );
        iSchedulerWait->AsyncStop();
        delete iSchedulerWait;
        iSchedulerWait = NULL;
        }
     C_TRACE((_L("<<CModemAtPipeController::~CModemAtPipeController()")));
     }

CModemAtPipeController::CModemAtPipeController( RIscApi& aIscApi,
    TUint& aObjId,
    CModemAtHandler& aHandler ) : 
    iIscApi( aIscApi ),
    iModemAtObjId( aObjId ),
    iModemAtDevId( THIS_DEVICE ),
    iDpDevId( 0 ),
    iDpObjId( 0 ),
    iAtHandler( aHandler ),
    iSchedulerWait( NULL )
        {
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_CMODEMATPIPECONTROLLER, "CModemAtPipeController::CModemAtPipeController;aObjId=%d", aObjId );
        C_TRACE (( _T("CModemAtPipeController::CModemAtPipeController() obj id: 0x%x>"), aObjId ));
        iPipe.pipeHandle = KInvalidPipeHandle;
        iPipe.firstDevId = 0;
        iPipe.firstObjId = 0;
        iPipe.secondDevId = 0;
        iPipe.secondDevId = 0;
        iPipe.pipeState = TPipeInfo::EPipeNoPipe;

        C_TRACE (( _T("CModemAtPipeController::CModemAtPipeController()<") ));
    }

 
 void CModemAtPipeController::HandleNameServiceMessage( const TIsiReceiveC& aReceivedMessage )
     {
     OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLENAMESERVICEMESSAGE, "CModemAtPipeController::HandleNameServiceMessage" );
     C_TRACE (( _T("CModemAtPipeController::HandleNameServiceMessage()") ));
     switch( aReceivedMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
        {
        case PNS_NAME_QUERY_RESP: 
            C_TRACE(_L("PNS_NAME_QUERY_RESP"));
            OstTrace0( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLENAMESERVICEMESSAGE, "CModemAtPipeController::HandleNameServiceMessage - PNS_NAME_QUERY_RESP" );
            HandleNameQueryResp( aReceivedMessage );
            break;

        case PNS_NAME_ADD_IND:
            C_TRACE(_L("PNS_NAME_ADD_IND"));
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLENAMESERVICEMESSAGE, "CModemAtPipeController::HandleNameServiceMessage - PNS_NAME_ADD_IND" );
            HandleNameAddInd( aReceivedMessage );
            break;
         
        case PNS_NAME_REMOVE_IND:
            C_TRACE(_L("PNS_NAME_REMOVE_IND")); 
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLENAMESERVICEMESSAGE, "CModemAtPipeController::HandleNameServiceMessage  - PNS_NAME_REMOVE_IND" );
            HandleNameRemoveInd( aReceivedMessage );
            break;

        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }
     }

 void CModemAtPipeController::HandlePipeServiceMessage( const TIsiReceiveC& aReceivedMessage)
     {
     OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage" );
     OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage;messageId=%d", aReceivedMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) );
     C_TRACE (( _T("CModemAtPipeController::HandlePipeServiceMessage()") ));
     C_TRACE((_L("message id: %d") , aReceivedMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) ));
     switch( aReceivedMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
        {
        case PNS_PIPE_CREATE_RESP:
            C_TRACE(_L("PNS_PIPE_CREATE_RESP"));
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage - PNS_PIPE_CREATE_RESP" );
            HandlePipeCreateResp( aReceivedMessage );
            break;

        case PNS_PIPE_REDIRECT_RESP:
            C_TRACE(_L("PNS_PIPE_REDIRECT_RESP"));
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage - PNS_PIPE_REDIRECT_RESP" );
            HandlePipeRedirectResp( aReceivedMessage );
            break;

        case PNS_PIPE_REMOVE_RESP:
            C_TRACE(_L("PNS_PIPE_REMOVE_RESP"));
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage - PNS_PIPE_REMOVE_RESP" );
            HandlePipeRemoveResp( aReceivedMessage );
            break;
         
        case PNS_PIPE_ENABLE_RESP:
            C_TRACE(_L("PNS_PIPE_ENABLE_RESP"));
            OstTrace0( TRACE_NORMAL, DUP5_CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage - PNS_PIPE_ENABLE_RESP" );
            HandlePipeEnabledResp( aReceivedMessage );
            break;

        case PNS_PEP_STATUS_IND :
            C_TRACE(_L("PNS_PEP_STATUS_IND"));
            OstTrace0( TRACE_NORMAL, DUP6_CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage - PNS_PEP_STATUS_IND" );
            HandlePepStatusInd( aReceivedMessage );
            break;
        
        case PNS_PEP_CTRL_RESP:
            C_TRACE(_L("PNS_PEP_CTRL_RESP"));
            OstTrace0( TRACE_NORMAL, DUP7_CMODEMATPIPECONTROLLER_HANDLEPIPESERVICEMESSAGE, "CModemAtPipeController::HandlePipeServiceMessage - PNS_PEP_CTRL_RESP" );
            HandlePepCtrlResp( aReceivedMessage );
            break;

        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }
     }


TBool CModemAtPipeController::MatchDataportName( TUint aName )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_MATCHDATAPORTNAME, "CModemAtPipeController::MatchDataportName" );

    /*
    See from dataport: 
    void CDpMif::SendNameAddReqL():

        case KDpDialUpPort:
        case KDpPort4:
        case KDpPort5:
            {
            messageData.Append( aUnit );                //name
            messageData.Append( KDpPadding );           //name
            messageData.Append( PN_PEP_TYPE_COMM );     //name
            messageData.Append( PN_PIPE );              //name
            break;
    Only 3 last bytes count
    */
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_MATCHDATAPORTNAME, "CModemAtPipeController::MatchDataportName;aName=%d", aName );
    C_TRACE((_L("CModemAtPipeController::MatchDataportName(): %d") , aName ) );
    TBool ret( EFalse );

    TUint dataportName1(0);
    dataportName1 = (
    ( (TUint)PN_PIPE ) | 
    ( (TUint)PN_PEP_TYPE_COMM << 8 ) | 
    ( KDpPadding << 16) | 
    ( KDialUpPort1 << 24 ) );
    OstTrace1( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_MATCHDATAPORTNAME, "CModemAtPipeController::MatchDataportName;dataportName1=%d", dataportName1 );
    C_TRACE((_L("CModemAtPipeController::MatchDataportName(): name1 %d") , dataportName1 ) );
    TUint dataportName4(0);
    dataportName4 = (
    ( (TUint)PN_PIPE ) | 
    ( (TUint)PN_PEP_TYPE_COMM << 8 ) | 
    ( KDpPadding << 16) | 
    ( KDialUpPort4 << 24 ) );
    OstTrace1( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_MATCHDATAPORTNAME, "CModemAtPipeController::MatchDataportName;dataportName4=%d", dataportName4 );
    C_TRACE((_L("CModemAtPipeController::MatchDataportName(): name4 %d") , dataportName4 ) );
    TUint dataportName5(0);
    dataportName5 = (
    ( (TUint)PN_PIPE ) | 
    ( (TUint)PN_PEP_TYPE_COMM << 8 ) | 
    ( KDpPadding << 16) | 
    ( KDialUpPort5 << 24 ) );
    OstTrace1( TRACE_NORMAL, DUP4_CMODEMATPIPECONTROLLER_MATCHDATAPORTNAME, "CModemAtPipeController::MatchDataportName;dataportName5=%d", dataportName5 );
    C_TRACE((_L("CModemAtPipeController::MatchDataportName(): name5 %d") , dataportName5 ) );

      if( dataportName1 == aName ||
          dataportName4 == aName ||
          dataportName5 == aName )
        {
        ret = ETrue;
        }        
    return ret;
    }

void CModemAtPipeController::HandleNameAddInd( const TIsiReceiveC& aReceivedMessage )     
     {
     OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLENAMEADDIND, "CModemAtPipeController::HandleNameAddInd" );
     C_TRACE (( _T("CModemAtPipeController::HandleNameAddInd()") ));
     TInt matchesInThisMsg = aReceivedMessage.Get16bit( ISI_HEADER_SIZE +
       PNS_NAME_ADD_IND_OFFSET_MATCHESINMSG );
     
     OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLENAMEADDIND, "CModemAtPipeController::HandleNameAddInd;matchesInThisMsg=%d", matchesInThisMsg );
     C_TRACE((_L("matchesInThisMsg: %d") ,matchesInThisMsg ));
     TUint name = 0;
    
     for( TInt i = 1; i <= matchesInThisMsg; i++ ) 
         {
         name = aReceivedMessage.Get32bit(ISI_HEADER_SIZE +
           PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL * i);
         OstTraceExt2( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLENAMEADDIND, "CModemAtPipeController:;i=%d;name=%x", i, name );
         C_TRACE((_L("name [%d] 0x%x"), i ,name));

         if( MatchDataportName(name) )
             {
             TUint8 devId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
               PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL * i +
               PN_NAME_SRV_ITEM_STR_OFFSET_DEV );
             TUint8 objId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
               PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL * i +
               PN_NAME_SRV_ITEM_STR_OFFSET_OBJ);

             if( devId == THIS_DEVICE )
                {
                OstTraceExt2( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLENAMEADDIND, "CModemAtPipeController::HandleNameAddInd - Adding dataport name.;devId=%x;objId=%x", devId, objId );
                C_TRACE((_L("Adding dataport name. (Dataport device id 0x%x,  object id: 0x%x)"),devId,objId));
                iAtHandler.AddDevice( devId, objId );
                }
             else
                {
                OstTraceExt2( TRACE_NORMAL, DUP4_CMODEMATPIPECONTROLLER_HANDLENAMEADDIND, "CModemAtPipeController::HandleNameAddInd - ERROR: Pipe is not created.;devId=%x;objId=%x", devId, objId );
                C_TRACE((_L("ERROR: Pipe is not created. (Dataport device id 0x%x,  object id: 0x%x)"),devId, objId ));
                }
             }
         }
     }

void CModemAtPipeController::HandleNameRemoveInd( const TIsiReceiveC& aReceivedMessage )     
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLENAMEREMOVEIND, "CModemAtPipeController::HandleNameRemoveInd" );
    C_TRACE (( _T("CModemAtPipeController::HandleNameRemoveInd()") ));
    TInt matchesInThisMsg = aReceivedMessage.Get16bit( ISI_HEADER_SIZE +
      PNS_NAME_REMOVE_IND_OFFSET_MATCHESINMSG );
    
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLENAMEREMOVEIND, "CModemAtPipeController::HandleNameRemoveInd;matchesInThisMsg=%d", matchesInThisMsg );
    C_TRACE((_L("matchesInThisMsg: %d"), matchesInThisMsg ));
    TUint name = 0;

    for( TInt i = 1; i <= matchesInThisMsg; i++ ) 
        {
        name = aReceivedMessage.Get32bit(ISI_HEADER_SIZE +
          PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL * i);
        OstTraceExt2( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLENAMEREMOVEIND, "CModemAtPipeController::HandleNameRemoveInd;i=%d;name=%x", i, name );
        C_TRACE((_L("name [%d] 0x%x"), i ,name));

        if( MatchDataportName(name) )
            {
            TUint devId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
              PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL * i +
              PN_NAME_SRV_ITEM_STR_OFFSET_DEV );
            TUint objId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
              PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL * i +
              PN_NAME_SRV_ITEM_STR_OFFSET_OBJ);

            OstTraceExt2( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLENAMEREMOVEIND, "CModemAtPipeController::HandleNameRemoveInd - Dataport removed from name service;devId=%x;objId=%x", devId, objId );
            C_TRACE((_L("Dataport removed from name service (devid %x  o-bjid %x) "), devId, objId));
            iAtHandler.RemoveDevice( devId, objId );
            }
        }
    }

void CModemAtPipeController::HandleNameQueryResp( const TIsiReceiveC& aReceivedMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLENAMEQUERYRESP, "CModemAtPipeController::HandleNameQueryResp" );
    C_TRACE((_L("CModemAtPipeHandler::HandleNameQueryResp")));
    TUint16 matchesInThisMsg( aReceivedMessage.Get16bit(
        ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_MATCHESINMSG ) );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLENAMEQUERYRESP, "CModemAtPipeController::HandleNameQueryResp;matchesInThisMsg=%d", matchesInThisMsg );
    C_TRACE((_L("matchesInThisMsg: %d"), matchesInThisMsg ));

    TBool found = EFalse;
    if( 0 < matchesInThisMsg )
        {
        for( TInt i = 0; i < matchesInThisMsg; i++ )
            {
            TInt recordIndex( i * SIZE_PN_NAME_SRV_ITEM_STR );
            TUint8 name( aReceivedMessage.Get8bit(
                ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_NAMEENTRYTBL +
                recordIndex + ( PN_NAME_SRV_ITEM_STR_OFFSET_NAME + KLastByteIndex ) ) );
            OstTraceExt2( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLENAMEQUERYRESP, "CModemAtPipeController::HandleNameQueryResp;i=%d;name=%x", i, name );
            C_TRACE((_L("name [%d] 0x%x"), i, name ));
            if( PN_AT_MODEM == name )
                {
                OstTrace0( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLENAMEQUERYRESP, "CModemAtPipeController::HandleNameQueryResp - PN_AT_MODEM found." );
                C_TRACE((_L("PN_AT_MODEM found.")));
                i = matchesInThisMsg;
                found = ETrue;
                }
            }
        }
    iAtHandler.SetModemAtExistsInCmt( found );
    }



void CModemAtPipeController::SendCreatePipeMessage( const TUint8 aDevId, const TUint8 aObjId ) 
     {
     OstTraceExt2( TRACE_NORMAL, CMODEMATPIPECONTROLLER_SENDCREATEPIPEMESSAGE, "CModemAtPipeController::SendCreatePipeMessage;aDevId=%x;aObjId=%x", aDevId, aObjId );
     C_TRACE (( _T("CModemAtPipeController::SendCreatePipeMessage() DevId: 0x%x ObjId: 0x%x"), aDevId, aObjId ));
     HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_PIPE_CREATE_REQ );
     TRACE_ASSERT(message);

     iDpObjId = aObjId;
     iDpDevId = aDevId;
     
     TPtr8 messageptr = message->Des();
     TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_PIPE_CREATE_REQ );
     isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE );
     
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_UTID, 0);
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_SUBFUNCTION, PNS_PIPE_CREATE_REQ );
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_STATEAFTERCREATION ,PN_PIPE_DISABLE );
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_PRIORITY ,0 );
     
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_FIRSTPEPDEV, aDevId );
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_FIRSTPEPOBJ, aObjId );
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_FIRSTPEPTYPE, PN_PEP_TYPE_COMMON );
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_FILLERBYTE1, KFiller );
     
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_SECONDPEPDEV, THIS_DEVICE );
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_SECONDPEPOBJ, iModemAtObjId );
     isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_SECONDPEPTYPE, PN_PEP_TYPE_COMMON );
     isimessage.Complete();
    
     OstTrace0( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_SENDCREATEPIPEMESSAGE, "CModemAtPipeController::SendCreatePipeMessage - Created ISI-message" );
     OstTraceExt1( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_SENDCREATEPIPEMESSAGE, "CModemAtPipeController::SendCreatePipeMessage - msg;messageptr=%s", messageptr );
     C_TRACE(_L("Created ISI-message"));
     DUMP_MESSAGE( messageptr );
     TInt retVal = iIscApi.Send( messageptr );
     TRACE_ASSERT( retVal == KErrNone );
     delete message;
     }


 void CModemAtPipeController::HandlePipeCreateResp( const TIsiReceiveC& aReceivedMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLEPIPECREATERESP, "CModemAtPipeController::HandlePipeCreateResp" );
    C_TRACE (( _T("CModemAtPipeController::HandlePipeCreateResp()") ));
    TInt error = aReceivedMessage.Get8bit(ISI_HEADER_SIZE +
      PNS_PIPE_CREATE_RESP_OFFSET_ERRORCODE );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLEPIPECREATERESP, "CModemAtPipeController::HandlePipeCreateResp - Pipe create resp:;error=%d", error );
    C_TRACE((_L("Pipe create resp: %d"), error ));
    
    if( error == PN_PIPE_NO_ERROR)
        {
        TUint8 pipeHandle;
        pipeHandle = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
          PNS_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE );
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLEPIPECREATERESP, "CModemAtPipeController::HandlePipeCreateResp - Pipe created successfully.;pipeHandle=%d", pipeHandle );
        C_TRACE( (_L("Pipe created successfully. Pipehandle: %d"), pipeHandle ));
        
        // Now that pipehandle is received, it will also be used as dteId when connecting to AT Modem.
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLEPIPECREATERESP, "CModemAtPipeController::HandlePipeCreateResp -  pipe found" );
        C_TRACE((_L("CModemAtPipeHandler::HandlePipeCreateResp: pipe found")));
        iPipe.pipeHandle = pipeHandle;
        iPipe.firstDevId = iDpDevId;
        iPipe.firstObjId = iDpObjId;
        iPipe.secondObjId = iModemAtObjId;
        iPipe.secondDevId = THIS_DEVICE;
        ChangePipeState( TPipeInfo::EPipeCreated );

        iAtHandler.PipeCreated( pipeHandle, KErrNone );
        }
    else
        {
        OstTrace1( TRACE_NORMAL, DUP4_CMODEMATPIPECONTROLLER_HANDLEPIPECREATERESP, "CModemAtPipeController::HandlePipeCreateResp - Pipe creation failed:;error=%d", error );
        C_TRACE((_L("Pipe creation failed: %d"), error ));
        iPipe.pipeHandle = KInvalidPipeHandle;
        iPipe.firstDevId = 0;
        iPipe.firstObjId = 0;
        iPipe.secondDevId = 0;
        iPipe.secondObjId = 0;
        ChangePipeState( TPipeInfo::EPipeNoPipe );

        iAtHandler.PipeCreated( KInvalidPipeHandle, KErrGeneral  );
        }
    }



void CModemAtPipeController::HandlePipeRemoveResp( const TIsiReceiveC& aReceivedMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLEPIPEREMOVERESP, "CModemAtPipeController::HandlePipeRemoveResp" );
    C_TRACE (( _T(">>CModemAtPipeController::HandlePipeRemoveResp()") ));
    TInt error = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PIPE_REMOVE_RESP_OFFSET_ERRORCODE );
    TInt pipehandle = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PIPE_REMOVE_RESP_OFFSET_PIPEHANDLE );


    TRACE_ASSERT( error == PN_PIPE_NO_ERROR )
    if( error != PN_PIPE_NO_ERROR )
        {
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLEPIPEREMOVERESP, "CModemAtPipeController::HandlePipeRemoveResp;error=%d", error );
        C_TRACE(( _T("CModemAtPipeController::HandlePipeRemoveResp() error %d"), error ));
        return;
        }

    ChangePipeState( TPipeInfo::EPipeNoPipe );

    if( iSchedulerWait )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLEPIPEREMOVERESP, "CModemAtPipeController::HandlePipeRemoveResp - iSchedulerWait->Stop" );
        C_TRACE((_L("CModemAtPipeController::HandlePipeRemoveResp() iSchedulerWait->Stop")));
        iSchedulerWait->AsyncStop();
        delete iSchedulerWait;
        iSchedulerWait = NULL;
        }
    C_TRACE((_L("<<CModemAtPipeController::HandlePipeRemoveResp()")));
    }

 
void CModemAtPipeController::RemovePipe()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_REMOVEPIPE, "CModemAtPipeController::RemovePipe" );
    OstTraceExt3( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_REMOVEPIPE, "CModemAtPipeController::RemovePipe - ;iPipe.pipeHandle=%d;this=%p;iPipe.pipeState=%d", iPipe.pipeHandle, this, iPipe.pipeState );
    C_TRACE (( _T("CModemAtPipeController::RemovePipe(%d, 0x%x)"), iPipe.pipeHandle, this ));
    C_TRACE (( _T("CModemAtPipeController::RemovePipe( dteID: %d, state: %d)"), iPipe.pipeHandle, iPipe.pipeState ));
            
    if( KInvalidPipeHandle != iPipe.pipeHandle &&
        TPipeInfo::EPipeRemoving != iPipe.pipeState &&
        TPipeInfo::EPipeNoPipe != iPipe.pipeState )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_REMOVEPIPE, "CModemAtPipeController::RemovePipe - start to remove pipe" );
        C_TRACE((_L("CModemAtPipeController::RemovePipe: start to remove pipe")));
        ChangePipeState( TPipeInfo::EPipeRemoving );
        SendRemovePipeReq( iPipe.pipeHandle );
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_REMOVEPIPE, "CModemAtPipeController::RemovePipe - no need to remove pipe" );
        C_TRACE((_L("CModemAtPipeController::RemovePipe: no need to remove pipe")));
        }
    }
 

void CModemAtPipeController::SendRemovePipeReq( const TUint8 aPipeHandle ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATPIPECONTROLLER_SENDREMOVEPIPEREQ, "CModemAtPipeController::SendRemovePipeReq();aPipeHandle=%d", aPipeHandle );
    C_TRACE (( _T(">>CModemAtPipeController::SendRemovePipeReq()") ));
    C_TRACE((_L("Remove pipe handle %d"), aPipeHandle));
    TInt size = ISI_HEADER_SIZE + SIZE_PNS_PIPE_REMOVE_REQ;
    HBufC8* message = HBufC8::New( size );
    TRACE_ASSERT( message );

    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, size );
    isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REMOVE_REQ_OFFSET_UTID, 0 );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REMOVE_REQ_OFFSET_SUBFUNCTION, 
      PNS_PIPE_REMOVE_REQ );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REMOVE_REQ_OFFSET_PIPEHANDLE,
      aPipeHandle ); //disabled
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REMOVE_REQ_OFFSET_FILLERBYTE1 ,0 );
    isimessage.Complete();
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_SENDREMOVEPIPEREQ, "CModemAtPipeController::SendRemovePipeReq - Created ISI-message" );
    C_TRACE(_L("Created ISI-message"));

    TInt retVal = iIscApi.Send( messageptr );
    TRACE_ASSERT( retVal == KErrNone );
    delete message;
    if( !iSchedulerWait )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_SENDREMOVEPIPEREQ, "CModemAtPipeController::SendRemovePipeReq - iSchedulerWait->Start()" );
        C_TRACE(_L("CModemAtPipeController::SendRemovePipeReq() iSchedulerWait->Start()"));
        TRAPD(err, iSchedulerWait = new (ELeave) CActiveSchedulerWait);
        ASSERT_PANIC_ALWAYS( err == KErrNone );
        iSchedulerWait->Start();
        }
    C_TRACE (( _T("<<CModemAtPipeController::SendRemovePipeReq()") ));
    }

void CModemAtPipeController::RedirectPipe( 
    const TUint8 aNewDevId,
    const TUint8 aNewObjId ) 
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATPIPECONTROLLER_REDIRECTPIPE, "CModemAtPipeController::RedirectPipe;aNewDevId=%x;aNewObjId=%x", aNewDevId, aNewObjId );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_REDIRECTPIPE, "CModemAtPipeController::RedirectPipe;iPipe.pipeHandle=%d", iPipe.pipeHandle );
    C_TRACE (( _T("CModemAtPipeController::RedirectPipe() dteid %d"), iPipe.pipeHandle));
    C_TRACE((_L("CModemAtPipeController::RedirectPipe() New pep, deviceId: 0x%x objId: 0x%x "), aNewDevId, aNewObjId));

    if( iPipe.pipeState == TPipeInfo::EPipeRemoving || 
        iPipe.pipeState == TPipeInfo::EPipeNoPipe ) 
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_REDIRECTPIPE, "CModemAtPipeController::RedirectPipe - Pipe is already removed -> cant redirect" );
        C_TRACE(_L("CModemAtPipeController::RedirectPipe() Pipe is already removed -> cant redirect"));
        return;
        }

    if( aNewDevId == THIS_DEVICE && aNewObjId == iModemAtObjId )
        {
        OstTrace1( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_REDIRECTPIPE, "CModemAtPipeController::RedirectPipe - EPipeDisabled.;iModemAtObjId=%x", iModemAtObjId );
        C_TRACE((_L("CModemAtPipeController::RedirectPipe() EPipeDisabled. iModemAtObjId: 0x%x"), iModemAtObjId));
        // NOTE: Setting pipe state disabled here won't send command mode yet.
        iPipe.pipeState = TPipeInfo::EPipeDisabled;
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP4_CMODEMATPIPECONTROLLER_REDIRECTPIPE, "CModemAtPipeController::RedirectPipe - EPipeRedirected" );
        C_TRACE(_L("CModemAtPipeController::RedirectPipe() EPipeRedirected"));
        ChangePipeState( TPipeInfo::EPipeRedirected );
        }

    OstTrace0( TRACE_NORMAL, DUP5_CMODEMATPIPECONTROLLER_REDIRECTPIPE, "CModemAtPipeController::RedirectPipe - Redirecting pipe" );
    C_TRACE(_L("CModemAtPipeController::RedirectPipe() Redirecting pipe"));

    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_PIPE_REDIRECT_REQ );
    ASSERT_PANIC_ALWAYS( message );
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_PIPE_REDIRECT_REQ );
    isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID,PN_PIPE);
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_UTID, 0); 
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_SUBFUNCTION,
        PNS_PIPE_REDIRECT_REQ );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_PIPEHANDLE,
        iPipe.pipeHandle );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_STATEAFTERCREATION,
        PN_PIPE_DISABLE );
    //old pep 
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_OLDPEPDEV,
        iPipe.secondDevId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_OLDPEPOBJ, 
        iPipe.secondObjId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_OLDPEPTYPE, PN_PEP_TYPE_COMMON); 
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_FILLERBYTE1, KFiller );

    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPDEV, aNewDevId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPOBJ, aNewObjId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPTYPE, 
        PN_PEP_TYPE_COMMON ); 

    iPipe.secondDevId = aNewDevId;
    iPipe.secondObjId = aNewObjId;

    isimessage.Set8bit(ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_NSB, KFiller );
    isimessage.Complete();

    OstTrace0( TRACE_NORMAL, DUP6_CMODEMATPIPECONTROLLER_REDIRECTPIPE, "CModemAtPipeController::RedirectPipe - Created ISI-message" );
    C_TRACE(_L("Created ISI-message"));
    TInt retVal = iIscApi.Send( messageptr );
    TRACE_ASSERT( retVal == KErrNone );
    delete message;
    }
 
void CModemAtPipeController::HandlePipeRedirectResp( const TIsiReceiveC& aReceivedMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLEPIPEREDIRECTRESP, "CModemAtPipeController::HandlePipeRedirectResp" );
    C_TRACE (( _T("CModemAtPipeController::HandlePipeRedirectResp()") ));
    TInt error = aReceivedMessage.Get8bit( ISI_HEADER_SIZE +
      PNS_PIPE_REDIRECT_RESP_OFFSET_ERRORCODE );
    TInt pipeHandle = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE );
    C_TRACE((_L("CModemAtPipeController::HandlePipeRedirectResp() pipehandle: %d"), pipeHandle ));
    
    if( error != PN_PIPE_NO_ERROR )
        {
        iAtHandler.SendAtModemDataRedirectResultReq( AT_MODEM_REDIRECT_RESULT_ERROR );
        RemovePipe();
        }
    else
        {
        iAtHandler.SendAtModemDataRedirectResultReq( AT_MODEM_REDIRECT_RESULT_OK );
        }
    }

void CModemAtPipeController::QueryModemAtFromNameService() 
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_QUERYMODEMATFROMNAMESERVICE, "CModemAtPipeController::QueryModemAtFromNameService" );
    C_TRACE (( _T("CModemAtPipeController::QueryModemAtFromNameService()") ));
    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_NAME_QUERY_REQ );
    TRACE_ASSERT( message );
    if( message )
        {
        TPtr8 messageptr = message->Des();
        TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_NAME_QUERY_REQ );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_NAMESERVICE );       
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_UTID, KDefaultTrId );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_SUBFUNCTION, PNS_NAME_QUERY_REQ );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_RESERVED1, 0 );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_RESERVED2, 0);
        isimessage.Set32bit( ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_NAME, PN_AT_MODEM );
        isimessage.Set32bit( ISI_HEADER_SIZE + PNS_NAME_QUERY_REQ_OFFSET_BITMASK, PN_AT_MODEM );
        isimessage.Complete();

        OstTraceExt1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_QUERYMODEMATFROMNAMESERVICE, "CModemAtPipeController::QueryModemAtFromNameService - msg dump;messageptr=%s", messageptr );
        DUMP_MESSAGE( messageptr );
        TInt retVal = iIscApi.Send( messageptr );
        TRACE_ASSERT( retVal == KErrNone );
        delete message;
        }
    }

void CModemAtPipeController::SendEnablePipeReq()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_SENDENABLEPIPEREQ, "CModemAtPipeController::SendEnablePipeReq" );
    C_TRACE (( _T("CModemAtPipeController::SendEnablePipeReq()") ));
    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_PIPE_ENABLE_REQ );
    ASSERT_PANIC_ALWAYS( message ) 
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_PIPE_ENABLE_REQ );
    isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE );       
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_UTID, 0 );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_SUBFUNCTION, PNS_PIPE_ENABLE_REQ );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_PIPEHANDLE , iPipe.pipeHandle );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_FILLERBYTE1 , 0 );
    isimessage.Complete();

    DUMP_MESSAGE( messageptr );
    TInt retVal = iIscApi.Send( messageptr );
    TRACE_ASSERT( retVal == KErrNone );
    delete message;
    }

void CModemAtPipeController::HandlePipeEnabledResp( const TIsiReceiveC& aReceivedMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLEPIPEENABLEDRESP, "CModemAtPipeController::HandlePipeEnabledResp" );
    C_TRACE (( _T("CModemAtPipeController::HandlePipeEnabledResp()") ));
    TInt error = aReceivedMessage.Get8bit(ISI_HEADER_SIZE + 
      PNS_PIPE_ENABLE_RESP_OFFSET_ERRORCODE);
    TInt pipeHandle = aReceivedMessage.Get8bit(ISI_HEADER_SIZE + 
      PNS_PIPE_ENABLE_RESP_OFFSET_PIPEHANDLE);

    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLEPIPEENABLEDRESP, "CModemAtPipeController::HandlePipeEnabledResp;pipeHandle=%d", pipeHandle );
    C_TRACE((_L("CModemAtPipeController::HandlePipeEnabledResp() pipehandle %d"), pipeHandle));       
    if( error == PN_PIPE_NO_ERROR )
        {
        ChangePipeState( TPipeInfo::EPipeEnabled );
        }
    else
        {
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLEPIPEENABLEDRESP, "CModemAtPipeController::HandlePipeEnabledResp - pipe;error=%d", error );
        C_TRACE((_L("CModemAtPipeController::HandlePipeEnabledResp() pipe error: %d"), error));
        }
    }


void CModemAtPipeController::ChangePipeState( TPipeInfo::EPipeState aState )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATPIPECONTROLLER_CHANGEPIPESTATE, "CModemAtPipeController::ChangePipeState;aState=%d;iPipe.pipeHandle=%d", (TInt)aState, iPipe.pipeHandle );
    C_TRACE((_L("CModemAtPipeController::ChangePipeState: pipe handle: %d ( New state: %d )"), iPipe.pipeHandle, (TInt)aState));

    iPipe.pipeState = aState;
    if( TPipeInfo::EPipeRedirected == aState )
        {
        // NOTE: Setting data mode when pipe is being redirected.
        // This will fasten the discovery of data mode.
        // If pipe fails, we return to command mode.
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_CHANGEPIPESTATE, "CModemAtPipeController::ChangePipeState - DATA MODE" );
        C_TRACE(( _T("DATA MODE") ));
        iAtHandler.HandleCommandModeChange( EDataMode );
        }
    else if( TPipeInfo::EPipeDisabled == aState )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_CHANGEPIPESTATE, "CModemAtPipeController::ChangePipeState - COMMAND MODE" );
        C_TRACE(( _T("COMMAND MODE") ));
        iAtHandler.HandleCommandModeChange( ECommandMode );
        }
    }

void CModemAtPipeController::HandlePepStatusInd( const TIsiReceiveC& aReceivedMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "CModemAtPipeController::HandlePepStatusInd" );
    C_TRACE(( _L("CModemAtPipeController::HandlePepStatusInd()")));
    TUint8 pepType( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ) );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "CModemAtPipeController::HandlePepStatusInd;pepType=%d", pepType );
    C_TRACE(( _L("pepType: %d"), pepType ));
    
    TUint8 pipeHandle( aReceivedMessage.Get8bit( ISI_HEADER_SIZE +
      PNS_PEP_STATUS_IND_OFFSET_PIPEHANDLE ) );
    OstTrace1( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "CModemAtPipeController::HandlePepStatusInd;pipeHandle=%d", pipeHandle );
    C_TRACE(( _L("pipeHandle: %d"), pipeHandle ));
    
    if( iPipe.pipeHandle != pipeHandle )
        {
        C_TRACE( (_L("Unknown pipe handle: %d, iPipe.pipeHandle: %d "), pipeHandle, iPipe.pipeHandle ));
        return;
        }

    if( pepType == PN_PEP_TYPE_COMM )
        {
        TUint8 indicationId( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
          PNS_PEP_STATUS_IND_OFFSET_INDICATIONID ) );
        OstTrace1( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "CModemAtPipeController::HandlePepStatusInd;indicationId=%d", indicationId );
        C_TRACE(( _L("indicationId: %d"), indicationId ));
        if( indicationId == PEP_COMM_IND_ID_ESCAPE )
            {
            C_TRACE(( _T("PEP_COMM_IND_ID_ESCAPE -> AT_MODEM_SIGNAL_ESCAPE_DETECTED") ));
            iAtHandler.SendSignalDetection( AT_MODEM_SIGNAL_ESCAPE_DETECTED );
            }
        else if( indicationId == PEP_COMM_IND_ID_V24_CHANGE )
            {
			OstTrace0( TRACE_NORMAL, DUP4_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "PEP_COMM_IND_ID_V24_CHANGE" );
            C_TRACE(( _T("PEP_COMM_IND_ID_V24_CHANGE") ));

            TUint8 indicationData( aReceivedMessage.Get8bit( ISI_HEADER_SIZE +
              PNS_PEP_STATUS_IND_OFFSET_INDICATIONDATA ) );
            OstTrace1( TRACE_NORMAL, DUP5_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "CModemAtPipeController::HandlePepStatusInd;indicationData=%d", indicationData );
            C_TRACE(( _L("indicationData: %d"), indicationData ));

            if( indicationData == PEP_COMM_SIGNAL_DTR_ON )
                {
                OstTrace0( TRACE_NORMAL, DUP6_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "AT_MODEM_SIGNAL_DTR_ON" );
                C_TRACE(( _T("PEP_COMM_SIGNAL_DTR_ON -> AT_MODEM_SIGNAL_DTR_ON") ));
                iAtHandler.SendSignalDetection( AT_MODEM_SIGNAL_DTR_ON );
                }
            else if( indicationData == PEP_COMM_SIGNAL_DTR_OFF )
                {
				OstTrace0( TRACE_NORMAL, DUP7_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "AT_MODEM_SIGNAL_DTR_OFF" );
                C_TRACE(( _T("PEP_COMM_SIGNAL_DTR_OF -> AT_MODEM_SIGNAL_DTR_OFF") ));
                iAtHandler.SendSignalDetection( AT_MODEM_SIGNAL_DTR_OFF );
                }
            else
                {
				OstTrace0( TRACE_NORMAL, DUP8_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "Unknown indication data" );
                C_TRACE(( _T("Unknown indication data") ));
                }
            }
        else
            {
			OstTrace1( TRACE_NORMAL, DUP9_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "CModemAtPipeController::HandlePepStatusInd;indicationId=%d", indicationId );
            C_TRACE((_L("Unknown indicationId: %d pepType %d"), indicationId, pepType));
            }
        }
     else
        {
        OstTrace1( TRACE_NORMAL, DUP10_CMODEMATPIPECONTROLLER_HANDLEPEPSTATUSIND, "CModemAtPipeController::HandlePepStatusInd;pepType=%d", pepType );
        C_TRACE((_L("CModemAtPipeController::HandlePepStatusInd() Peptype ignored %d "), pepType));
        }
    }


TPipeInfo::EPipeState CModemAtPipeController::GetPipeState()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_GETPIPESTATE, "CModemAtPipeController::GetPipeState" );
    C_TRACE((_L("CModemAtPipeController::GetPipeState") ) );
    return iPipe.pipeState;
    }


void CModemAtPipeController::SendPepCtrlReq( const TUint8 aControlId, TDesC8& aData )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_SENDPEPCTRLREQ, "CModemAtPipeController::SendPepCtrlReq" );
    C_TRACE (( _T("CModemAtPipeController::SendPepCtrlReq()") ));
    TInt length( ISI_HEADER_SIZE + SIZE_PNS_PEP_CTRL_REQ );
    TInt fillers( 0 );
    if( aControlId == PEP_COMM_CTRL_ID_ESCAPE )
        {
        fillers++;
        length += ( KPepCtrlDataSize + fillers );
        }
    else if( aControlId == PEP_COMM_CTRL_ID_FLOW_CTRL)
        {
        length += KPepFlowCtrlDataSize;
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        return;
        }
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_SENDPEPCTRLREQ, "CModemAtPipeController::SendPepCtrlReq;length=%d", length );
    C_TRACE((_L("ISI message length: %d"), length));
    HBufC8* message = HBufC8::New( length );
    if( message )
        {
        TPtr8 messageptr = message->Des();
        TIsiSend isimessage( messageptr, length );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, iDpDevId );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, iDpObjId );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_UTID, 0 );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_SUBFUNCTION, PNS_PEP_CTRL_REQ );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PIPEHANDLE , iPipe.pipeHandle );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPTYPE , PN_PEP_TYPE_COMM );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLID, aControlId );
        for( TInt i = 0; i < aData.Length(); i++ )
            {
            C_TRACE((_L("Setting control data[%d] = 0x%x"), ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + i, aData[ i ] ));
            isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA + i, aData[ i ] );
            }
        if( fillers > 0 )
            {
            C_TRACE((_L("Setting filler to data[%d] = 0x%x"), length - 1, KFiller ));
            isimessage.Set8bit( length - 1, KFiller );
            }
        isimessage.Complete();

        DUMP_MESSAGE( messageptr );
        TInt retVal = iIscApi.Send( messageptr );
        TRACE_ASSERT( KErrNone == retVal );
        delete message;
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_SENDPEPCTRLREQ, "CModemAtPipeController::SendPepCtrlReq - No enough memory" );
        C_TRACE (( _T("CModemAtPipeController::SendPepCtrlReq(): No enough memory") ));
        }
    }

void CModemAtPipeController::HandlePepCtrlResp( const TIsiReceiveC& aReceivedMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp" );
    C_TRACE(( _L("CModemAtPipeController::HandlePepCtrlResp") ));
    TUint8 pepType( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
        PNS_PEP_CTRL_RESP_OFFSET_PEPTYPE ) );
    C_TRACE(( _L("pepType: %d"), pepType ));

    TUint8 pipeHandle( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
        PNS_PEP_CTRL_RESP_OFFSET_PIPEHANDLE ) );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp;pipeHandle=%d", pipeHandle );
    C_TRACE(( _L("pipeHandle: %d"), pipeHandle ));
    
    TUint8 controlId( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
        PNS_PEP_CTRL_RESP_OFFSET_PEPCTRLID ) );
    OstTrace1( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp;controlId=%d", controlId );
    C_TRACE(( _L("controlId: %d"), controlId ));

    if( PN_PEP_TYPE_COMM == pepType &&
        PEP_COMM_CTRL_ID_ESCAPE == controlId )
        {
        TUint8 error( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
            PNS_PEP_CTRL_RESP_OFFSET_ERRORCODE ) );
        OstTrace1( TRACE_NORMAL, DUP3_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp;error=%d", error );
        C_TRACE(( _L("error code: %d"), error ));

        switch( error )
            {
            case PN_PIPE_NO_ERROR:
                {
                OstTrace0( TRACE_NORMAL, DUP4_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp - PN_PIPE_NO_ERROR" );
                C_TRACE(( _L("PN_PIPE_NO_ERROR")));
                break;
                }
            case PN_PIPE_ERR_NOT_SUPPORTED:
                {
                OstTrace0( TRACE_NORMAL, DUP5_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp - error:PN_PIPE_ERR_NOT_SUPPORTED" );
                C_TRACE(( _L("error: PN_PIPE_ERR_NOT_SUPPORTED")));
                break;
                }
            case PN_PIPE_ERR_INVALID_PARAM:
                {
                OstTrace0( TRACE_NORMAL, DUP6_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp - error: PN_PIPE_ERR_INVALID_PARAM" );
                C_TRACE(( _L("error: PN_PIPE_ERR_INVALID_PARAM")));
                break;
                }
            case PN_PIPE_ERR_INVALID_CTRL_ID:
                {
                OstTrace0( TRACE_NORMAL, DUP7_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp - error: PN_PIPE_ERR_INVALID_CTRL_ID" );
                C_TRACE(( _L("error: PN_PIPE_ERR_INVALID_CTRL_ID")));
                break;
                }
            case PN_PIPE_ERR_GENERAL:
                {
                OstTrace0( TRACE_NORMAL, DUP8_CMODEMATPIPECONTROLLER_HANDLEPEPCTRLRESP, "CModemAtPipeController::HandlePepCtrlResp - error: PN_PIPE_ERR_GENERAL" );
                C_TRACE(( _L("error: PN_PIPE_ERR_GENERAL")));
                break;
                }
            }
        }
    }
//  sending PNS_PEP_STATUS_IND from pipe controller should be moved to PEP !
void CModemAtPipeController::SendPepStatusIndSignal( const TUint8 aPepCommSignal )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATPIPECONTROLLER_SENDPEPSTATUSINDSIGNAL, "CModemAtPipeController::SendPepStatusIndSignal;aPepCommSignal=%x", aPepCommSignal );
    C_TRACE((_L("CModemAtPipeController::SendPepStatusIndSignal() aPepCommSignal: 0x%x"), aPepCommSignal ));
    TInt length( ISI_HEADER_SIZE + SIZE_PNS_PEP_STATUS_IND );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATPIPECONTROLLER_SENDPEPSTATUSINDSIGNAL, "CModemAtPipeController::SendPepStatusIndSignal;length=%d", length );
	C_TRACE((_L("ISI message length: %d"), length));
    HBufC8* message = HBufC8::New( length );
    if( message )
        {
        TPtr8 messageptr = message->Des();
        TIsiSend isimessage( messageptr, length );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, iDpDevId );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, iDpObjId );
        isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_UTID, 0 );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_SUBFUNCTION, PNS_PEP_STATUS_IND );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PIPEHANDLE, iPipe.pipeHandle );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_PEPTYPE, PN_PEP_TYPE_COMM );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_INDICATIONID, PEP_COMM_IND_ID_V24_CHANGE );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_FILLERBYTE1, 0 );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_FILLERBYTE2, 0 );
        isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PEP_STATUS_IND_OFFSET_INDICATIONDATA, aPepCommSignal );
        isimessage.Complete();
        DUMP_MESSAGE( messageptr );
        TInt retVal = iIscApi.Send( messageptr );
        TRACE_ASSERT( KErrNone == retVal );
        delete message;
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATPIPECONTROLLER_SENDPEPSTATUSINDSIGNAL, "CModemAtPipeController::SendPepStatusIndSignal - error: not enough memory" );
        C_TRACE (( _T("CModemAtPipeController::SendPepStatusIndSignal(): Not enough memory") ));
        }
    }


