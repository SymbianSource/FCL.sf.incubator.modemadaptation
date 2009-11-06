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



#include <phonetisi.h>              // For ISI_HEADER_OFFSET... defines
#include <pipeisi.h>                // For PNS_PEP..& PNS_PIPE... defines
#include <pipe_sharedisi.h>         // For PN_MULTI_CREDIT_FLOW_CONTROL etc..
#include <pn_const.h>               
#include "pipehandler.h"
#include "pep.h"
#include "iadtrace.h"               // For C_TRACE.. trace macros
#include "internalapi.h"
#include "iadnokiadefinitions.h"
#include "router.h"
#include "iadhelpers.h"             // For GET_.& SET_.. macros
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "pipehandlerTraces.h"
#endif


const TInt KFirstPepIndex( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_FIRSTPEPDEV );
const TInt KFirstPepObj( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_FIRSTPEPOBJ );
//const TInt KFirstPepType( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_FIRSTPEPTYPE );
const TInt KSecondPepIndex( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_SECONDPEPDEV );
const TInt KSecondPepObj( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_SECONDPEPOBJ );
//const TInt KSecondPepType( ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_SECONDPEPTYPE );
/*TBR
const TInt KPnMsgReceiver = 1;
const TInt KPnMsgReceiverObject = 6;
*/

DPipeHandler::DPipeHandler(DRouter& aRouter) : iRouter(aRouter)
    {
    OstTrace1( TRACE_NORMAL, DPIPEHANDLER_DPIPEHANDLER_ENTRY, "<>DPipeHandler::DPipeHandler;aRouter=%x", ( TUint )&( aRouter ) );
    C_TRACE( ( _T( "DPipeHandler::DPipeHandler(0x%x) ->" ), &aRouter ) );
    C_TRACE( ( _T( "DPipeHandler::DPipeHandler <-" ) ) );
    }

DPipeHandler::~DPipeHandler()
    {
    OstTrace0( TRACE_NORMAL, DUP1_DPIPEHANDLER_DPIPEHANDLER_ENTRY, ">DPipeHandler::~DPipeHandler" );
    C_TRACE( ( _T( "DPipeHandler::~DPipeHandler ->" ) ) );
    iPepContainer.ResetAndDestroy(); 
    C_TRACE( ( _T( "DPipeHandler::~DPipeHandler <-" ) ) );
    OstTrace0( TRACE_NORMAL, DPIPEHANDLER_DPIPEHANDLER_EXIT, "<DPipeHandler::~DPipeHandler" );
    }
    
void DPipeHandler::ReceivePipeMessage(
        const TDesC8& aMsg,
        MIAD2ChannelApi* aChannel // TODO to ref?
        )
    {
    OstTraceExt2( TRACE_NORMAL, DPIPEHANDLER_RECEIVEPIPEMESSAGE_ENTRY, ">DPipeHandler::ReceivePipeMessage;aMsg=%x;aChannel=%x", ( TUint )&( aMsg ), ( TUint )( aChannel ) );

    C_TRACE( ( _T( "DPipeHandler::ReceivePipeMessage ->" ) ) );
    ASSERT_RESET_ALWAYS( aChannel, EIADNullParameter | EIADFaultIdentifier14 << KFaultIdentifierShift  );
    const TUint8* ptr( aMsg.Ptr() );
    DPep* pep = FindPep( GET_RECEIVER_OBJ( ptr ) ); //FindPep( aMsg );
    if( pep )
        {
        pep->ReceiveMessage( aMsg, aChannel );
        }
    else
        {
        // We are interested only ISA originated pipes that are type of PN_PEP_TYPE_COMMON (a.g.a credit based flowcontrol)
        const TUint8 msgId( ptr[ ISI_HEADER_OFFSET_MESSAGEID ] );
        // TODO: Remove this if pep_type_common to be the only pep_type const TUint8 pepType( ptr[ ISI_HEADER_SIZE + PNS_PEP_CONNECT_REQ_OFFSET_OTHERPEPTYPE ] );
        if( msgId == PNS_PEP_CONNECT_REQ )// TODO: Remove this if pep_type_common to be the only pep_type && pepType == PN_PEP_TYPE_COMMON )
            {
            HandlePepConnectReq( aMsg, aChannel); // creating ISA originated pipe
            }
         else
            {
            aChannel->ReceiveMsg(aMsg);
            }
        }
    C_TRACE( ( _T( "DPipeHandler::ReceivePipeMessage <-" ) ) );
    OstTrace0( TRACE_NORMAL, DPIPEHANDLER_RECEIVEPIPEMESSAGE_EXIT, "<DPipeHandler::ReceivePipeMessage" );
    }

TInt DPipeHandler::SendPipeMessage(
        TDes8& aMsg,
        TUint16 aFromChannel
        )
    {
    OstTraceExt2( TRACE_NORMAL, DPIPEHANDLER_SENDPIPEMESSAGE_ENTRY, ">DPipeHandler::SendPipeMessage;aMsg=%x;aFromChannel=%hx", ( TUint )&( aMsg ), aFromChannel );

    C_TRACE( ( _T( "DPipeHandler::SendPipeMessage from: %d ->" ), aFromChannel ) );
    TInt retVal( KErrNone );
    const TUint8* ptr( const_cast<TUint8*>( aMsg.Ptr() ) );
    DPep* pep = FindPep( GET_SENDER_OBJ( ptr ) );
    if( pep )
        {
        retVal = pep->SendMessage( aMsg, aFromChannel ); // is this aFromChannel needed?
        }
    else
        {
        switch( ptr[ ISI_HEADER_OFFSET_MESSAGEID ] )
            {
            case PNS_PIPE_DATA:
                {
                TRACE_ASSERT_INFO( 0, (TUint8)aFromChannel<<KChannelNumberShift );
                retVal = KErrNotFound;
                break;
                }
            case PNS_PIPE_CREATE_REQ:
                {
                retVal = HandlePipeCreateReq( aMsg, aFromChannel );
                break;
                }
            default:
                {
                retVal = iRouter.SendMsg( aMsg );
                break;
                }
            }
        }
    C_TRACE( ( _T( "DPipeHandler::SendPipeMessage 0x%x <-" ), retVal ) );
    
    OstTrace1( TRACE_NORMAL, DPIPEHANDLER_SENDPIPEMESSAGE_EXIT, "<DPipeHandler::SendPipeMessage;retVal=%d", retVal );
    return retVal;
    }

void DPipeHandler::HandlePepConnectReq(
        const TDesC8& aMsg,
        MIAD2ChannelApi* aChannel
        )
    {
    OstTraceExt2( TRACE_NORMAL, DPIPEHANDLER_HANDLEPEPCONNECTREQ_ENTRY, ">DPipeHandler::HandlePepConnectReq;aMsg=%x;aChannel=%x", ( TUint )&( aMsg ), ( TUint )( aChannel ) );

    C_TRACE( ( _T( "DPipeHandler::HandlePepConnectReq ->" ) ) );
    ASSERT_RESET_ALWAYS( aChannel, EIADNullParameter | EIADFaultIdentifier15 << KFaultIdentifierShift );
    const TUint8* ptr( const_cast<TUint8*>( aMsg.Ptr() ) );
    TUint16 receiverObj = GET_RECEIVER_OBJ( ptr );
    ASSERT_RESET_ALWAYS( KErrNone == CreatePep( receiverObj, EFalse), ( receiverObj << KChannelNumberShift ) );
    // TODO change DPep to get & not a * IST
    DPep* temp = FindPep( receiverObj );
    ASSERT_RESET_ALWAYS( temp, 0 );
    temp->ReceiveMessage( aMsg, aChannel);	
    C_TRACE( ( _T( "DPipeHandler::HandlePepConnectReq pepcontainer c %d" ), iPepContainer.Count() ) );
    C_TRACE( ( _T( "DPipeHandler::HandlePepConnectReq <-" ) ) );

    OstTrace1( TRACE_NORMAL, DPIPEHANDLER_HANDLEPEPCONNECTREQ_EXIT, "<DPipeHandler::HandlePepConnectReq;iPepContainer.Count()=%d", iPepContainer.Count() );
    }


TInt DPipeHandler::HandlePipeCreateReq( TDes8& aMessage, TUint16 aFromChannel )
    {
    OstTraceExt2( TRACE_NORMAL, DPIPEHANDLER_HANDLEPIPECREATEREQ_ENTRY, ">DPipeHandler::HandlePipeCreateReq;aMessage=%x;aFromChannel=%hx", ( TUint )&( aMessage ), aFromChannel );

    C_TRACE( ( _T( "DPipeHandler::HandlePipeCreateReq from: %d ->" ), aFromChannel ) );

    const TUint8* tmpPtr( aMessage.Ptr() );
    ASSERT_RESET_ALWAYS( aMessage.Length() > KSecondPepObj, EIADOverTheLimits | EIADFaultIdentifier13 << KFaultIdentifierShift );
    TUint8 firstPepDev = ( tmpPtr[ KFirstPepIndex ] & 0xFC );//OK
    TUint16 firstPepObj = IS_DEV_HOST( tmpPtr[ KFirstPepIndex ] ) ? 
                          ( ((TUint16)tmpPtr[ KFirstPepObj ] ) + (( firstPepDev & 0x0F ) << 8) ) :
                          ( ((TUint16)tmpPtr[ KFirstPepObj ] ) + (( firstPepDev & 0x03 ) << 8) );
    TUint8 secondPepDev = ( tmpPtr[ KSecondPepIndex ] & 0xFC );//OK
    TUint16 secondPepObj = IS_DEV_HOST( tmpPtr[ KSecondPepIndex ] ) ?
                           ( ((TUint16)tmpPtr[ KSecondPepObj ]) + (( secondPepDev & 0x0F ) << 8 ) ) :
                           ( ((TUint16)tmpPtr[ KSecondPepObj ]) + (( secondPepDev & 0x03 ) << 8 ) );
    TRACE_ASSERT_INFO( firstPepDev == THIS_DEVICE || firstPepDev == OTHER_DEVICE_1, (TUint8)aFromChannel<<KChannelNumberShift | firstPepDev );
    TRACE_ASSERT_INFO( secondPepDev == THIS_DEVICE || secondPepDev == OTHER_DEVICE_1, (TUint8)aFromChannel<<KChannelNumberShift | secondPepDev );

    C_TRACE((_T("DIscIsiPipeHandler::HandlePipeCreateReq StateAfterCreate 0x%x"), tmpPtr[ ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_STATEAFTERCREATION ] ) );
    C_TRACE((_T("DIscIsiPipeHandler::HandlePipeCreateReq Priority 0x%x"), tmpPtr[ ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_PRIORITY ] ) );
    C_TRACE((_T("DIscIsiPipeHandler::HandlePipeCreateReq 1stPepDev 0x%x"), firstPepDev));
    C_TRACE((_T("DIscIsiPipeHandler::HandlePipeCreateReq 1stPepObj 0x%x"), firstPepObj));
    C_TRACE((_T("DIscIsiPipeHandler::HandlePipeCreateReq 2ndPepDev 0x%x"), secondPepDev));
    C_TRACE((_T("DIscIsiPipeHandler::HandlePipeCreateReq 2ndPepObj 0x%x"), secondPepObj));
    
    OstTraceExt5( TRACE_NORMAL, DPIPEHANDLER_HANDLEPIPECREATEREQ, "DPipeHandler::HandlePipeCreateReq;StateAfterCreate=%hhx;firstPepDev=%hhx;firstPepObj=%hhx;secondPepDev=%hhx;secondPepObj=%hhx", 
    			tmpPtr[ ISI_HEADER_SIZE + PNS_PIPE_CREATE_REQ_OFFSET_STATEAFTERCREATION ], firstPepDev, firstPepObj, secondPepDev, secondPepObj );

    

    TInt retVal(KErrNone);
    if( firstPepDev == THIS_DEVICE )
        {
        if( aFromChannel == firstPepObj )
            {
            retVal = CreatePep( firstPepObj, ETrue );
            C_TRACE( ( _T( "DPipeHandler::HandlePipeCreateReq CreatePep sender=1st pepobj (controller) retVal %d" ), aFromChannel ) );
            OstTraceExt1( TRACE_NORMAL, DUP1_DPIPEHANDLER_HANDLEPIPECREATEREQ, "DPipeHandler::HandlePipeCreateReq CreatePep sender=1st pepobj (controller);channelId=%hx", aFromChannel );
            
            }
        else
            {
            retVal = CreatePep( firstPepObj, EFalse );
            C_TRACE( ( _T( "DPipeHandler::HandlePipeCreateReq CreatePep 1st retVal %d" ), aFromChannel ) );
            OstTraceExt1( TRACE_NORMAL, DUP2_DPIPEHANDLER_HANDLEPIPECREATEREQ, "DPipeHandler::HandlePipeCreateReq CreatePep 1st;channelId=%hx", aFromChannel );
            
            }
        }
    if( retVal != KErrNone )
        {
        TRACE_ASSERT_INFO( 0, (TUint8)aFromChannel<<KChannelNumberShift );
        return retVal;//TODO: remove multiple return points
        }

    if( secondPepDev == THIS_DEVICE )
        {
        if( aFromChannel == secondPepObj )
            {
            retVal = CreatePep( secondPepObj, ETrue );
            C_TRACE( ( _T( "DPipeHandler::HandlePipeCreateReq CreatePep sender=2nd pepobj (controller) retVal %d" ), aFromChannel ) );
            OstTraceExt1( TRACE_NORMAL, DUP3_DPIPEHANDLER_HANDLEPIPECREATEREQ, "DPipeHandler::HandlePipeCreateReq CreatePep sender=2nd pepobj (controller);channelId=%hx", aFromChannel );
            
            }
        else
            {
            retVal = CreatePep( secondPepObj, EFalse );
            C_TRACE( ( _T( "DPipeHandler::HandlePipeCreateReq CreatePep 2nd retVal %d" ), aFromChannel ) );
            OstTraceExt1( TRACE_NORMAL, DUP4_DPIPEHANDLER_HANDLEPIPECREATEREQ, "DPipeHandler::HandlePipeCreateReq CreatePep 2nd ;channelId=%hx", aFromChannel );
            
            }
        }
    if( retVal != KErrNone )
        {
        TRACE_ASSERT_INFO( 0, (TUint8)aFromChannel<<KChannelNumberShift );
        return retVal;//TODO: remove multiple return points
        }
    // forward to ISA
    C_TRACE( ( _T( "DPipeHandler::HandlePipeCreateReq CreatePep TBR sending...%d %d" ), aMessage.Length(), aMessage.MaxLength() ) );
    retVal = iRouter.SendMsg( aMessage );
    C_TRACE( ( _T( "DPipeHandler::HandlePipeCreateReq return: %d <-" ), retVal ) );

    OstTrace1( TRACE_NORMAL, DUP5_DPIPEHANDLER_HANDLEPIPECREATEREQ_EXIT, "<DPipeHandler::HandlePipeCreateReq;retVal=%d", retVal );
    return retVal;
    }

TInt DPipeHandler::CreatePep(
        TUint16 aPepChannel,
        TBool aIsController
        )
    {
    OstTraceExt2( TRACE_NORMAL, DPIPEHANDLER_CREATEPEP_ENTRY, ">DPipeHandler::CreatePep;aPepChannel=%hx;aIsController=%d", aPepChannel, aIsController );

    C_TRACE( ( _T( "DPipeHandler::CreatePep 0x%x %d ->" ), aPepChannel, aIsController ) );
    TInt retVal( KErrNone );
    MIAD2ChannelApi* channel = iRouter.GetChannel( aPepChannel );
    if( channel )
        {
        DPep* tmp = NULL;
        tmp = new DPep( &iRouter, channel, aPepChannel, *this );
        ASSERT_RESET_ALWAYS( tmp, EIADMemoryAllocationFailure | EIADFaultIdentifier24 << KFaultIdentifierShift  ); 
        iPepContainer.Append( tmp );
        C_TRACE( ( _T( "DPipeHandler::CreatePep pepcontainer c %d" ), iPepContainer.Count() ) );
        OstTrace1( TRACE_NORMAL, DPIPEHANDLER_CREATEPEP, "DPipeHandler::CreatePep;iPepContainer.Count()=%d", iPepContainer.Count() );
        
        
        }
    else
        {
        C_TRACE( ( _T( "DPipeHandler::CreatePep channel not open" ) ) );
        OstTraceExt1( TRACE_NORMAL, DUP1_DPIPEHANDLER_CREATEPEP, "DPipeHandler::CreatePep;aPepChannel=%hx", aPepChannel );
        
        retVal = KErrGeneral;
        TRACE_ASSERT_INFO( 0, aPepChannel << KChannelNumberShift );        
        }
    C_TRACE( ( _T( "DPipeHandler::CreatePep 0x%x %d &d <-" ), aPepChannel, aIsController, retVal ) );

    OstTrace1( TRACE_NORMAL, DPIPEHANDLER_CREATEPEP_EXIT, "<DPipeHandler::CreatePep;retVal=%d", retVal );
    return retVal;
    }

void DPipeHandler::RemovePep(
        DPep* aPep
        )
    {
    OstTrace1( TRACE_NORMAL, DPIPEHANDLER_REMOVEPEP_ENTRY, ">DPipeHandler::RemovePep;aPep=%x", ( TUint )( aPep ) );

    C_TRACE( ( _T( "DPipeHandler::RemovePep 0x%x ->" ), aPep ) );
    TInt ind = iPepContainer.Find( aPep );
    if( KErrNotFound != ind )
        {
        iPepContainer.Remove( ind );
        C_TRACE( ( _T( "DPipeHandler::RemovePep pepcontainer c %d" ), iPepContainer.Count() ) );
        OstTrace1( TRACE_NORMAL, DPIPEHANDLER_REMOVEPEP, "DPipeHandler::RemovePep;iPepContainer.Count()=%d", iPepContainer.Count() );
        
        delete aPep;
        aPep = NULL;
        if( iPepContainer.Count() == 0 )
            {
            iPepContainer.Compress();
            iPepContainer.Reset();
            }
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    C_TRACE( ( _T( "DPipeHandler::RemovePep 0x%x <-" ), aPep ) );

    OstTrace0( TRACE_NORMAL, DPIPEHANDLER_REMOVEPEP_EXIT, "<DPipeHandler::RemovePep" );
    }

DPep* DPipeHandler::FindPep(
        const TUint16 aChannelId
        )
    {
    OstTraceExt2( TRACE_NORMAL, DPIPEHANDLER_FINDPEP_ENTRY, ">DPipeHandler::FindPep;aChannelId=%hx;iPepContainer.Count()=%d", aChannelId, iPepContainer.Count() );

    DPep* pep( NULL );
    TInt count( iPepContainer.Count() );
    C_TRACE( ( _T( "DPipeHandler::FindPep 0x%x %d ->" ), aChannelId, count ) );

	  for( TInt i( count - 1 ); i >= 0; --i )
		    {
		    if( iPepContainer[ i ]->ChannelId() == aChannelId )
			     {
			     pep = iPepContainer[ i ];
			     break;
			     }
        }
    C_TRACE( ( _T( "DPipeHandler::FindPep 0x%x 0x%x <-" ), aChannelId, pep ) );
    
    OstTrace1( TRACE_PIPE, DPIPEHANDLER_FINDPEP_EXIT, "<DPipeHandler::FindPep;pep=%x", (TUint)(pep) );
    return pep;
    }

#if (NCP_COMMON_SOS_VERSION_SUPPORT >= SOS_VERSION_95)
void DPipeHandler::PipeLoaned(
        const TUint16 aLoanerChannel,
        MIAD2ChannelApi* aCh
        )
    {

    C_TRACE( ( _T( "DPipeHandler::PipeLoaned 0x%x ->" ), aLoanerChannel ) );
    OstTrace1( TRACE_PIPE, DPIPEHANDLER_PIPELOANED_ENTRY, ">DPipeHandler::PipeLoaned;loanerChannel=%x", (TUint)aLoanerChannel );
    DPep* pep = FindPep( aLoanerChannel );
    if( pep )
        {
        pep->SetLoaned( aCh );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    OstTrace1( TRACE_PIPE, DPIPEHANDLER_PIPELOANED_EXIT, "<DPipeHandler::PipeLoaned;loanerChannel=%x", (TUint)aLoanerChannel );
    C_TRACE( ( _T( "DPipeHandler::PipeLoaned 0x%x <-" ), aLoanerChannel ) );

    }

void DPipeHandler::PipeLoanReturned(
        const TUint16 aLoanerChannel,
        MIAD2ChannelApi* aOldCh
        )
    {

    C_TRACE( ( _T( "DPipeHandler::PipeLoanReturned 0x%x ->" ), aLoanerChannel ) );
    DPep* pep = FindPep( aLoanerChannel );
    if( pep )
        {
        pep->SetLoanReturned( aOldCh );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    C_TRACE( ( _T( "DPipeHandler::PipeLoanReturned 0x%x <-" ), aLoanerChannel ) );

    }

#endif
