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



#include <iscapi.h>                 //For ISC
#include <iscnokiadefinitions.h>    //isc definitions
#include <pn_const.h>
#ifndef NCP_COMMON_BRIDGE_FAMILY       
#include <nsisi.h>
#endif
#include <tisi.h>                   //for isimessage
#include <pipeisi.h>                //pipe
#include <pipe_sharedisi.h>         //pipe
#include "cmodematpipecontroller.h"    
#include "cmodemathandler.h"
#include <at_modemisi.h>            //redirectreq error codes
#include "modemattrace.h"
#include "cmodematsrv.h"

const TInt KInvalidPipeHandle = -1;
#ifndef NCP_COMMON_BRIDGE_FAMILY
const TUint8 PEP_COMM_IND_ID_ESCAPE(0x07);
const TInt KInvalidDteId = -1;
const TUint8 KFiller = 0;
const TUint8 KDefaultTrId(0);
#endif
const TInt KLastByteIndex = 3;

CModemAtPipeController* CModemAtPipeController::NewL( RIscApi& aIscApi,
    TUint& aObjId,
    CModemAtHandler& aHandler )
    {
    CModemAtPipeController* self = new (ELeave) CModemAtPipeController( aIscApi,aObjId, aHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CModemAtPipeController::ConstructL()
    {
    C_TRACE (( _T("CModemAtPipeController::ConstructL()") ));
    }

CModemAtPipeController::~CModemAtPipeController() 
     {
     C_TRACE((_L(">>CModemAtPipeController::~CModemAtPipeController()")));
     if( iSchedulerWait )
        {
        C_TRACE((_L("CModemAtPipeController::~CModemAtPipeController() iSchedulerWait->Stop")));
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
    iPipeHandle( KInvalidPipeHandle ),
    iDataportDevId( 0 ),
    iDataportObjId( 0 ),
    iAtHandler( aHandler ),
    iSchedulerWait( NULL ),
    iDteId( KInitialDteId )
    {
    C_TRACE (( _T("CModemAtPipeController::CModemAtPipeController()") ));
    }

 
#ifndef NCP_COMMON_BRIDGE_FAMILY
 void CModemAtPipeController::HandleNameServiceMessage( const TIsiReceiveC& aReceivedMessage )
     {
     C_TRACE (( _T("CModemAtPipeController::HandleNameServiceMessage()") ));
     switch( aReceivedMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
        {
        case PNS_NAME_QUERY_RESP: 
            C_TRACE(_L("PNS_NAME_QUERY_RESP"));
            HandleNameQueryResp( aReceivedMessage );
            break;

        case PNS_NAME_ADD_IND:
            C_TRACE(_L("PNS_NAME_ADD_IND"));
            HandleNameAddInd( aReceivedMessage );
            break;
         
        case PNS_NAME_REMOVE_IND:
            C_TRACE(_L("PNS_NAME_REMOVE_IND")); 
            HandleNameRemoveInd( aReceivedMessage );
            break;

        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }
     }

 void CModemAtPipeController::HandlePipeServiceMessage( const TIsiReceiveC& aReceivedMessage)
     {
     C_TRACE (( _T("CModemAtPipeController::HandlePipeServiceMessage()") ));
     C_TRACE((_L("message id: %d") , aReceivedMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) ));
     switch( aReceivedMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
        {
        case PNS_PIPE_CREATE_RESP:
            C_TRACE(_L("PIPE CREATE RESP"));
            HandlePipeCreateResp( aReceivedMessage );
            break;

        case PNS_PIPE_REDIRECT_RESP:
            C_TRACE(_L("PNS_PIPE_REDIRECT_RESP"));
            HandlePipeRedirectResp( aReceivedMessage );
            break;

        case PNS_PIPE_REMOVE_RESP:
            C_TRACE(_L("PNS_PIPE_REMOVE_RESP"));
            HandlePipeRemoveResp( aReceivedMessage );
            break;
         
        case PNS_PIPE_ENABLE_RESP:
            C_TRACE(_L("PNS_PIPE_ENABLE_RESP"));
            HandlePipeEnabledResp( aReceivedMessage );
            break;

        case PNS_PEP_STATUS_IND :
            C_TRACE(_L("PNS_PEP_STATUS_IND "));
            HandlePepStatusInd( aReceivedMessage );
            break;

        default:
            TRACE_ASSERT_ALWAYS;
            break;
        }                            
     }


TUint CModemAtPipeController::MatchDataportName( TUint aName)
    {

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

    TUint KDpDialUpPort = 1;
    TUint KDpPadding = 0;
    TUint dataportName(0);
    dataportName = (
    ( (TUint)PN_PIPE ) | 
    ( (TUint)PN_PEP_TYPE_COMM << 8 ) | 
    ( KDpPadding << 16) | 
    ( KDpDialUpPort << 24 ) 
    );

    if( aName == dataportName )
        {
        return ETrue;
        }
    return EFalse;
    }

void CModemAtPipeController::HandleNameAddInd( const TIsiReceiveC& aReceivedMessage )     
     {
     C_TRACE (( _T("CModemAtPipeController::HandleNameAddInd()") ));
     TInt matchesInThisMsg = aReceivedMessage.Get16bit( ISI_HEADER_SIZE +
       PNS_NAME_ADD_IND_OFFSET_MATCHESINMSG );
     
     C_TRACE((_L("matchesInThisMsg: %d") ,matchesInThisMsg ));
     TUint name = 0;
    
     for( TInt i = 1; i <= matchesInThisMsg; i++ ) 
         {
         name = aReceivedMessage.Get32bit(ISI_HEADER_SIZE +
           PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL * i);
         C_TRACE((_L("name [%d] 0x%x"), i ,name));

         if( MatchDataportName(name) )
             {
             iDataportDevId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
               PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL * i +
               PN_NAME_SRV_ITEM_STR_OFFSET_DEV );
             iDataportObjId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
               PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL * i +
               PN_NAME_SRV_ITEM_STR_OFFSET_OBJ);

             if( iDataportDevId == THIS_DEVICE )
                {
                C_TRACE((_L("CREATE PIPE FROM DATAPORT when ATEXT plugins connect (devid %x  o-bjid %x) "),iDataportDevId,iDataportObjId));
                }
             else
                {
                C_TRACE((_L("ERROR: PIPE not Created due to unmatching DevId: 0x%x or ObjId 0x%x"),iDataportDevId, iDataportObjId ));
                TRACE_ASSERT_ALWAYS;
                }
             }
         }
     }

void CModemAtPipeController::HandleNameRemoveInd( const TIsiReceiveC& aReceivedMessage )     
    {
    C_TRACE (( _T("CModemAtPipeController::HandleNameRemoveInd()") ));
    TInt matchesInThisMsg = aReceivedMessage.Get16bit( ISI_HEADER_SIZE +
      PNS_NAME_REMOVE_IND_OFFSET_MATCHESINMSG );
    
    C_TRACE((_L("matchesInThisMsg: %d"), matchesInThisMsg ));
    TUint name = 0;

    for( TInt i = 1; i <= matchesInThisMsg; i++ ) 
        {
        name = aReceivedMessage.Get32bit(ISI_HEADER_SIZE +
          PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL * i);
        C_TRACE((_L("name [%d] 0x%x"), i ,name));

        if( MatchDataportName(name) )
            {
            TUint devId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
              PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL * i +
              PN_NAME_SRV_ITEM_STR_OFFSET_DEV );
            TUint objId = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
              PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL * i +
              PN_NAME_SRV_ITEM_STR_OFFSET_OBJ);

            if( iDataportDevId == devId && iDataportObjId == objId )
               {
               C_TRACE((_L("Dataport removed from name service (devid %x  o-bjid %x) "), iDataportDevId, iDataportObjId));
               iDataportDevId = 0;
               iDataportObjId = 0;
               }
            else
               {
               C_TRACE((_L("Unused dataport removed from name service. DevId: 0x%x or ObjId 0x%x"), iDataportDevId, iDataportObjId ));
               TRACE_ASSERT_ALWAYS;
               }
            }
        }
    }

void CModemAtPipeController::HandleNameQueryResp( const TIsiReceiveC& aReceivedMessage )
    {
    C_TRACE((_L("CModemAtPipeHandler::HandleNameQueryResp")));
    TUint16 matchesInThisMsg( aReceivedMessage.Get16bit(
        ISI_HEADER_SIZE + PNS_NAME_QUERY_RESP_OFFSET_MATCHESINMSG ) );
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
            C_TRACE((_L("name [%d] 0x%x"), i, name ));
            if( PN_AT_MODEM == name )
                {
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
     C_TRACE (( _T("CModemAtPipeController::SendCreatePipeMessage() DevId: 0x%x ObjId: 0x%x"), aDevId, aObjId ));
     HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_PIPE_CREATE_REQ );
     TRACE_ASSERT(message);
     
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
    
     C_TRACE(_L("Created ISI-message"));
     DUMP_MESSAGE( messageptr );
     TInt retVal = iIscApi.Send( messageptr );
     TRACE_ASSERT( retVal == KErrNone );
     delete message;
     }


 void CModemAtPipeController::HandlePipeCreateResp( const TIsiReceiveC& aReceivedMessage )
    {
    C_TRACE (( _T("CModemAtPipeController::HandlePipeCreateResp()") ));
    TInt error = aReceivedMessage.Get8bit(ISI_HEADER_SIZE +  PNS_PIPE_CREATE_RESP_OFFSET_ERRORCODE );
    C_TRACE((_L("Pipe create resp: %d"), error ));
    TRACE_ASSERT( iDteId != KInitialDteId );

    if( error == PN_PIPE_NO_ERROR)
        {
        iPipeHandle = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
          PNS_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE );
        C_TRACE( (_L("handle %d"), iPipeHandle ));
        iPipeTable[iDteId].iHandle = iPipeHandle;
        iPipeTable[iDteId].iFirstDevId = iDataportDevId;
        iPipeTable[iDteId].iFirstObjId =  iDataportObjId; 
        iPipeTable[iDteId].iSecondDevId = THIS_DEVICE;
        iPipeTable[iDteId].iSecondObjId =  iModemAtObjId;
        ChangePipeState( iDteId, TPipeInfo::EPipeCreated );
        iPipeHandle = KInvalidPipeHandle;
        iDteId = KInitialDteId;
        }
    else
        {
        C_TRACE (( _T("iPipeHandle == KInvalidPipeHandle ")));
        iPipeTable[iDteId].iHandle = KInvalidPipeHandle;
        iPipeTable[iDteId].iFirstDevId = 0;
        iPipeTable[iDteId].iFirstObjId = 0;
        iPipeTable[iDteId].iSecondDevId = 0;
        iPipeTable[iDteId].iSecondObjId = 0;
        ChangePipeState( iDteId, TPipeInfo::EPipeNoPipe );
        ASSERT_ALWAYS;
        }
    }

 void CModemAtPipeController::LinkDteIdToPipe( const TUint8 aDteId )
    {
    C_TRACE (( _T("CModemAtPipeController::LinkDteIdToPipe(0x%x)"), aDteId ));
    TRACE_ASSERT( aDteId < KMaxDteIdCount );
    if( !(aDteId < KMaxDteIdCount) )
        {
        C_TRACE(( _T("CModemAtPipeController::LinkDteIdToPipe() illegal dteid %d"), aDteId ));
        return;
        }

    iDteId = aDteId;
    if( iDataportDevId == THIS_DEVICE )
        {
        C_TRACE((_L("CREATE PIPE FROM DATAPORT (plugins are connecting) (devid %x  o-bjid %x) "),iDataportDevId,iDataportObjId));
        SendCreatePipeMessage( iDataportDevId, iDataportObjId );
        }
    else
        {
        C_TRACE((_L("Dataport has not been created (devid %x  o-bjid %x) "), iDataportDevId, iDataportObjId ));
        C_TRACE (( _T("iPipeHandle == KInvalidPipeHandle ")));
        iPipeTable[aDteId].iHandle = KInvalidPipeHandle;
        iPipeTable[aDteId].iFirstDevId = 0;
        iPipeTable[aDteId].iFirstObjId = 0;
        iPipeTable[aDteId].iSecondDevId = 0;
        iPipeTable[aDteId].iSecondObjId = 0;
        ChangePipeState( aDteId, TPipeInfo::EPipeNoPipe );
        }
    }


void CModemAtPipeController::HandlePipeRemoveResp( const TIsiReceiveC& aReceivedMessage )
    {
    C_TRACE (( _T(">>CModemAtPipeController::HandlePipeRemoveResp()") ));
    TInt error = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PIPE_REMOVE_RESP_OFFSET_ERRORCODE );
    TInt pipehandle = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PIPE_REMOVE_RESP_OFFSET_PIPEHANDLE );

    TInt dteId = FindDteId( pipehandle );
    
    TRACE_ASSERT( dteId < KMaxDteIdCount )
    if( !(dteId < KMaxDteIdCount) )
        {
        C_TRACE(( _T("CModemAtPipeController::HandlePipeRemoveResp() illegal dteid %d"), dteId ));
        return;
        }
    
    TRACE_ASSERT( error == PN_PIPE_NO_ERROR )
    if( error != PN_PIPE_NO_ERROR )
        {
        C_TRACE(( _T("CModemAtPipeController::HandlePipeRemoveResp() error %d"), error ));
        return;
        }

    ChangePipeState( dteId, TPipeInfo::EPipeNoPipe );

    iPipeTable[ dteId ].iHandle = KInvalidPipeHandle; 

    if( iSchedulerWait )
        {
        C_TRACE((_L("CModemAtPipeController::HandlePipeRemoveResp() iSchedulerWait->Stop")));
        iSchedulerWait->AsyncStop();
        delete iSchedulerWait;
        iSchedulerWait = NULL;
        }
    C_TRACE((_L("<<CModemAtPipeController::HandlePipeRemoveResp()")));
    }

 
void CModemAtPipeController::RemovePipe( const TUint8 aDteId )
    {
    C_TRACE (( _T("CModemAtPipeController::RemovePipe(%d, 0x%x)"), aDteId, this ));

    TRACE_ASSERT( aDteId < KMaxDteIdCount );
    if( !(aDteId < KMaxDteIdCount) )
        {
        C_TRACE(( _T("CModemAtPipeController::RemovePipe() illegal dteid %d"), aDteId ));
        return;
        }

    C_TRACE (( _T("iPipeTable[aDteId]:0x%x"), &iPipeTable[aDteId] ));
    C_TRACE (( _T("iHandle: %d"), iPipeTable[aDteId].iHandle ));
    C_TRACE (( _T("i1stDevId: %d"), iPipeTable[aDteId].iFirstDevId ));
    C_TRACE (( _T("i1stObjId: %d"), iPipeTable[aDteId].iFirstObjId )); 
    C_TRACE (( _T("i2ndDevId: %d"), iPipeTable[aDteId].iSecondDevId ));
    C_TRACE (( _T("i2ndObjId: %d"), iPipeTable[aDteId].iSecondObjId ));
    
    if( iPipeTable[aDteId].iHandle != KInvalidPipeHandle )   
        {
        if( ( iPipeTable[aDteId].iPipeState == TPipeInfo::EPipeRemoving ) || 
            ( iPipeTable[aDteId].iPipeState == TPipeInfo::EPipeNoPipe ) )
            {
            C_TRACE((_L("Already removing pipe or pipe is removed. Handle %d"), iPipeTable[aDteId].iHandle));
            return;
            }
        C_TRACE((_L("Remove pipe. Handle %d"), iPipeTable[aDteId].iHandle));
        ChangePipeState( aDteId, TPipeInfo::EPipeRemoving );
        SendRemovePipeReq( iPipeTable[aDteId].iHandle );
        }
    else
        {
        C_TRACE((_L("No Pipe to Remove")));
        }
    }
 

void CModemAtPipeController::SendRemovePipeReq( const TUint8 aPipeHandle ) 
    {
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
    C_TRACE(_L("Created ISI-message"));

    TInt retVal = iIscApi.Send( messageptr );
    TRACE_ASSERT( retVal == KErrNone );
    delete message;
    if( !iSchedulerWait )
        {
        C_TRACE(_L("CModemAtPipeController::SendRemovePipeReq() iSchedulerWait->Start()"));
        TRAPD(err, iSchedulerWait = new (ELeave) CActiveSchedulerWait);
        ASSERT_PANIC_ALWAYS( err == KErrNone );
        iSchedulerWait->Start();
        }
    C_TRACE (( _T("<<CModemAtPipeController::SendRemovePipeReq()") ));
    }

void CModemAtPipeController::RedirectPipe( const TUint8 aDteId,
    const TUint8 aNewDevId,
    const TUint8 aNewObjId ) 
    {
    C_TRACE (( _T("CModemAtPipeController::RedirectPipe() dteid %d"), aDteId ));
    C_TRACE((_L("CModemAtPipeController::RedirectPipe() New pep, deviceId: 0x%x objId: 0x%x "), aNewDevId, aNewObjId));
    TRACE_ASSERT( aDteId < KMaxDteIdCount );
    if( !(aDteId < KMaxDteIdCount) )
        {
        C_TRACE(( _T("CModemAtPipeController::RedirectPipe() illegal dteid %d"), aDteId ));
        return;
        }
    TRACE_ASSERT( iPipeTable[aDteId].iHandle != KInvalidPipeHandle );
    if( iPipeTable[aDteId].iHandle == KInvalidPipeHandle )
        {
        C_TRACE(( _T("CModemAtPipeController::RedirectPipe() invalid pipe handle %d"), iPipeTable[aDteId].iHandle ));
        return;
        }

    if( iPipeTable[aDteId].iPipeState == TPipeInfo::EPipeRemoving || 
        iPipeTable[aDteId].iPipeState == TPipeInfo::EPipeNoPipe ) 
        {
        C_TRACE(_L("CModemAtPipeController::RedirectPipe() Pipe is already removed -> cant redirect"));
        return;
        }

    if( aNewDevId == THIS_DEVICE && aNewObjId == iModemAtObjId )
        {
        C_TRACE((_L("CModemAtPipeController::RedirectPipe() EPipeDisabled. iModemAtObjId: 0x%x"), iModemAtObjId));
        ChangePipeState( aDteId, TPipeInfo::EPipeDisabled );
        }
    else
        {
        C_TRACE(_L("CModemAtPipeController::RedirectPipe() EPipeRedirected"));
        ChangePipeState( aDteId, TPipeInfo::EPipeRedirected );
        }

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
      iPipeTable[aDteId].iHandle ); 
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_STATEAFTERCREATION,
      PN_PIPE_DISABLE );
    //old pep 
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_OLDPEPDEV,
      iPipeTable[aDteId].iSecondDevId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_OLDPEPOBJ, 
      iPipeTable[aDteId].iSecondObjId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_OLDPEPTYPE, PN_PEP_TYPE_COMMON); 
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_FILLERBYTE1, KFiller );

    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPDEV, aNewDevId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPOBJ, aNewObjId );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPTYPE, 
      PN_PEP_TYPE_COMMON ); 

    iPipeTable[aDteId].iSecondDevId = aNewDevId;
    iPipeTable[aDteId].iSecondObjId = aNewObjId;

    isimessage.Set8bit(ISI_HEADER_SIZE + PNS_PIPE_REDIRECT_REQ_OFFSET_NSB, KFiller );
    isimessage.Complete();

    C_TRACE(_L("Created ISI-message"));
    TInt retVal = iIscApi.Send( messageptr );
    TRACE_ASSERT( retVal == KErrNone );
    delete message;
    }
 
void CModemAtPipeController::HandlePipeRedirectResp( const TIsiReceiveC& aReceivedMessage )
    {
    C_TRACE (( _T("CModemAtPipeController::HandlePipeRedirectResp()") ));
    TInt error = aReceivedMessage.Get8bit( ISI_HEADER_SIZE +
      PNS_PIPE_REDIRECT_RESP_OFFSET_ERRORCODE );
    TInt pipeHandle = aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE );
    C_TRACE((_L("CModemAtPipeController::HandlePipeRedirectResp() pipehandle: %d"), pipeHandle ));
    TInt dteId = FindDteId( pipeHandle );
    
    if( error != PN_PIPE_NO_ERROR )
        {
        if( dteId != KInvalidDteId )
            {
            C_TRACE(( _L("CModemAtPipeController::HandlePipeRedirectResp() pipe error: %d"), error ));
            iAtHandler.SendAtModemDataRedirectResultReq( dteId, AT_MODEM_REDIRECT_RESULT_ERROR);
            RemovePipe( dteId );
            return;
            }
        else
            {
            C_TRACE(( _L("CModemAtPipeController::HandlePipeRedirectResp() dteId not found, pipe error: %d"), error ));
            TRACE_ASSERT_ALWAYS;
            return;
            }
        }

    iAtHandler.SendAtModemDataRedirectResultReq( dteId, AT_MODEM_REDIRECT_RESULT_OK );

    if( iPipeTable[ dteId ].iPipeState != TPipeInfo::EPipeDisabled )
        {
        SendEnablePipeReq( pipeHandle );
        }

    }

void CModemAtPipeController::QueryModemAtFromNameService() 
    {
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

        DUMP_MESSAGE( messageptr );
        TInt retVal = iIscApi.Send( messageptr );
        TRACE_ASSERT( retVal == KErrNone );
        delete message;
        }
    }

void CModemAtPipeController::SendEnablePipeReq( const TUint8 aPipeHandle )
    {
    C_TRACE (( _T("CModemAtPipeController::SendEnablePipeReq()") ));
    HBufC8* message = HBufC8::New( ISI_HEADER_SIZE + SIZE_PNS_PIPE_ENABLE_REQ );
    ASSERT_PANIC_ALWAYS( message ) 
    TPtr8 messageptr = message->Des();
    TIsiSend isimessage( messageptr, ISI_HEADER_SIZE + SIZE_PNS_PIPE_ENABLE_REQ );
    isimessage.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_PIPE );       
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_UTID, 0 );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_SUBFUNCTION, PNS_PIPE_ENABLE_REQ );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_PIPEHANDLE , aPipeHandle );
    isimessage.Set8bit( ISI_HEADER_SIZE + PNS_PIPE_ENABLE_REQ_OFFSET_FILLERBYTE1 , 0 );
    isimessage.Complete();

    DUMP_MESSAGE( messageptr );
    TInt retVal = iIscApi.Send( messageptr );
    TRACE_ASSERT( retVal == KErrNone );
    delete message;
    }

 TInt CModemAtPipeController::FindDteId( const TInt aHandle )
    {
    C_TRACE (( _T("CModemAtPipeController::FindDteId()") ));
    TInt dteId = KInvalidDteId;
    for( TInt i = 0 ; i < KMaxDteIdCount ; i++ ) 
        {
        if( iPipeTable[i].iHandle == aHandle ) 
            {
            dteId = i;
            break;
            }
        }

    TRACE_ASSERT( dteId != KInvalidDteId );
    C_TRACE((_L("CModemAtPipeController::FindDteId() returns dteid: %d"),dteId));
    return dteId;
    } 

 
void CModemAtPipeController::HandlePipeEnabledResp( const TIsiReceiveC& aReceivedMessage )
    {
    C_TRACE (( _T("CModemAtPipeController::HandlePipeEnabledResp()") ));
    TInt error = aReceivedMessage.Get8bit(ISI_HEADER_SIZE + 
      PNS_PIPE_ENABLE_RESP_OFFSET_ERRORCODE);
    TInt pipeHandle = aReceivedMessage.Get8bit(ISI_HEADER_SIZE + 
      PNS_PIPE_ENABLE_RESP_OFFSET_PIPEHANDLE);
    TInt dteId = FindDteId(pipeHandle);           
    C_TRACE((_L("CModemAtPipeController::HandlePipeEnabledResp() pipehandle %d"), pipeHandle));
       
    if( error == PN_PIPE_NO_ERROR )
        {
        ChangePipeState( dteId, TPipeInfo::EPipeEnabled );
        }
    else
        {
        C_TRACE((_L("CModemAtPipeController::HandlePipeEnabledResp() pipe error: %d"), error));
        TRACE_ASSERT_ALWAYS;
        }
    }


void CModemAtPipeController::ChangePipeState(
    const TInt aDteId,
    TPipeInfo::EPipeState aState )
    {
    C_TRACE((_L("CModemAtPipeController::ChangePipeState(%d, %d)"), aDteId, aState));
    C_TRACE(( _L("Was: iPipeTable[ %d ].iPipeState = %d"), aDteId, (TInt)iPipeTable[ aDteId ].iPipeState ));
    iPipeTable[ aDteId ].iPipeState = aState;
    if( aState == TPipeInfo::EPipeEnabled )
        {
        C_TRACE(( _T("DATA MODE") ));
        iAtHandler.HandleCommandModeChange( aDteId, EDataMode );
        }
    else if( aState == TPipeInfo::EPipeDisabled )
        {
        C_TRACE(( _T("COMMAND MODE") ));
        iAtHandler.HandleCommandModeChange( aDteId, ECommandMode );
        }
    }

void CModemAtPipeController::HandlePepStatusInd( const TIsiReceiveC& aReceivedMessage )
    {
    C_TRACE(( _L("CModemAtPipeController::HandlePepStatusInd")));
    TUint8 pepType( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
      PNS_PEP_STATUS_IND_OFFSET_PEPTYPE ) );
    if( pepType == PN_PEP_TYPE_COMM )
        {
        TUint8 indicationId( aReceivedMessage.Get8bit( ISI_HEADER_SIZE + 
          PNS_PEP_STATUS_IND_OFFSET_INDICATIONID ) );
        if( indicationId == PEP_COMM_IND_ID_ESCAPE )
            {
            TUint8 pipeHandle( aReceivedMessage.Get8bit( ISI_HEADER_SIZE +
              PNS_PEP_STATUS_IND_OFFSET_PIPEHANDLE ) );
            TInt dteId( FindDteId( pipeHandle ) );           
            C_TRACE( (_L("CModemAtPipeController PEP_COMM_IND_ID_ESCAPE received, pipehandle %d dteid %d"), pipeHandle, dteId ));

            if( dteId != KInvalidDteId )
                {
                C_TRACE( (_L("SendEscapeSignalDetection dteid %d"), dteId ));
                iAtHandler.SendEscapeSignalDetection( dteId );
                }
            }
        // Not know should any other indicationid from comm pep type to be handled
        else
            {
            C_TRACE((_L("CModemAtPipeController Indication ignored pepType %d indicationId: %d "), pepType, indicationId));
            }
        }
        // pep type comm only supported one
     else
        {
        C_TRACE((_L("CModemAtPipeController Peptype ignored %d "), pepType));
        }
    }

 #endif // NCP_COMMON_BRIDGE_FAMILY

