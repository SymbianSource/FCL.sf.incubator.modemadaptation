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


#include "cmodematsrv.h"
#include "modematclientsrv.h"
#include "cmodematsession.h"
#include "modemattrace.h"
#include "catmessage.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmodematsessionTraces.h"
#endif


CModemAtSession* CModemAtSession::NewL( CModemAtSrv& aServer,
    const TVersion& aVersion )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSESSION_NEWL, "CModemAtSession::NewL" );
    return new (ELeave) CModemAtSession( aServer, aVersion );
    }

CModemAtSession::~CModemAtSession()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_CMODEMATSESSION, "CModemAtSession::~CModemAtSession;this=%x", this );
    C_TRACE (( _T(">>CModemAtSession::~CModemAtSession() 0x%x"), this ));

    iServer.CancelFifoRequests( this );

    if( !iConnectReq.IsNull() )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSESSION_CMODEMATSESSION, "CModemAtSession::~CModemAtSession - iConnectReq canceled" );
        C_TRACE(_L("iConnectReq canceled"));
        iConnectReq.Complete( KErrCancel );
        }
    if( !iUnsolicitedDataReq.IsNull() )
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSESSION_CMODEMATSESSION, "CModemAtSession::~CModemAtSession - iUnsolicitedDataReq canceled" );
        C_TRACE(_L("iUnsolicitedDataReq canceled"));
        iUnsolicitedDataReq.Complete( KErrCancel );
        }
    if( !iSignalIndReq.IsNull() )
        {
        OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSESSION_CMODEMATSESSION, "CModemAtSession::~CModemAtSession - iSignalIndReq canceled" );
        C_TRACE(_L("iSignalIndReq canceled"));
        iSignalIndReq.Complete( KErrCancel );
        }
    if( !iCommandModeReq.IsNull() )
        {
        OstTrace0( TRACE_NORMAL, DUP5_CMODEMATSESSION_CMODEMATSESSION, "CModemAtSession::~CModemAtSession - iCommandModeReq canceled" );
        C_TRACE(_L("iCommandModeReq canceled"));
        iCommandModeReq.Complete( KErrCancel );
        }
    iServer.ClientClosed(this);

    if(iClientName)
        {
        delete iClientName;
        iClientName = NULL;
        }
    C_TRACE (( _T("<<CModemAtSession::~CModemAtSession()") ));
    }

void CModemAtSession::ServiceL( const RMessage2& aMessage )
    {
   OstTrace0( TRACE_NORMAL, CMODEMATSESSION_SERVICEL, "CModemAtSession::ServiceL" );
   C_TRACE (( _T("CModemAtSession::ServiceL()") ));
    if ( iVersion.iMajor==KServerMajorVersionNumber &&
         iVersion.iMinor==KServerMinorVersionNumber &&
         iVersion.iBuild==KServerBuildVersionNumber )
        {
        DoServiceL( aMessage );
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSESSION_SERVICEL, "CModemAtSession::ServiceL - Complete KErrNotSupported" );
        C_TRACE (( _T("Complete KErrNotSupported") ));
        aMessage.Complete( KErrNotSupported );
        }
    }

TBool CModemAtSession::CheckAndCompleteExistingRequest( const RMessage2& aMessage )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest" );
    C_TRACE (( _T("CModemAtSession::CheckAndCompleteExistingRequest()") ));
    TBool RequestAlreadyPending = EFalse;
    switch ( aMessage.Function() )
        {     
        case EATExtSetExtensionInterface:
            OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EATExtSetExtensionInterface" );
            C_TRACE (( _T("EATExtSetExtensionInterface") ));
        break;
        case EModemAtRemovePipe:
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemAtRemovePipe" );
            C_TRACE (( _T("EModemAtRemovePipe") ));
        break;
        case EModemATHandleCommand:
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemATHandleCommand" );
            C_TRACE (( _T("EModemATHandleCommand") ));
        break;
        case  EReceiveUnsolicitedResult:
            OstTrace0( TRACE_NORMAL, DUP5_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EReceiveUnsolicitedResult" );
            C_TRACE (( _T("EReceiveUnsolicitedResult") ));
            RequestAlreadyPending = !iUnsolicitedDataReq.IsNull(); 
        break;
        case EReceiveUnsolicitedResultCancel:
            OstTrace0( TRACE_NORMAL, DUP6_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EReceiveUnsolicitedResultCancel" );
            C_TRACE (( _T("EReceiveUnsolicitedResultCancel") ));
            if ( iUnsolicitedDataReq.IsNull() )
                {
                OstTrace0( TRACE_NORMAL, DUP7_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - STRAY CANCEL" );
                C_TRACE (( _T("STRAY CANCEL") ));
                aMessage.Complete( KErrNone );
                return ETrue;
                }  
        break;
        case EModemATHandleCommandCancel:
            OstTrace0( TRACE_NORMAL, DUP8_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemATHandleCommandCancel" );
            C_TRACE (( _T("EModemATHandleCommandCancel") ));
        break;
        case EModemATReceiveSignalInd:
            OstTrace0( TRACE_NORMAL, DUP9_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemATReceiveSignalInd" );
            C_TRACE (( _T("EModemATReceiveSignalInd") ));
            RequestAlreadyPending = !iSignalIndReq.IsNull(); 
        break;
        case EModemATReceiveSignalIndCancel:
            OstTrace0( TRACE_NORMAL, DUP10_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemATReceiveSignalIndCancel" );
            C_TRACE (( _T("EModemATReceiveSignalIndCancel") ));
            if( iSignalIndReq.IsNull() )
                {
                OstTrace0( TRACE_NORMAL, DUP11_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - STRAY CANCEL" );
                C_TRACE (( _T("STRAY CANCEL") ));
                aMessage.Complete( KErrNone );
                return ETrue;
                }   
        break;
        
        case EModemATGetNvramStatus:            
            OstTrace0( TRACE_NORMAL, DUP12_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemATGetNvramStatus" );
            C_TRACE (( _T("EModemATGetNvramStatus") ));
        break;
        
        case EModemATCommandMode:
            OstTrace0( TRACE_NORMAL, DUP13_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemATCommandMode" );
            C_TRACE (( _T("EModemATCommandMode") ));
            RequestAlreadyPending = !iCommandModeReq.IsNull(); 
            break;
            
        case EModemATCommandModeCancel:
            OstTrace0( TRACE_NORMAL, DUP14_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - EModemATCommandModeCancel" );
            C_TRACE (( _T("EModemATCommandModeCancel") ));
            if( iCommandModeReq.IsNull())
                {
                OstTrace0( TRACE_NORMAL, DUP15_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - STRAY CANCEL" );
                C_TRACE (( _T("STRAY CANCEL") ));
                aMessage.Complete(KErrNone);
                return ETrue;
                }
            break;

        default:
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - Message not supported" );
            C_TRACE(_L("CModemAtSession::CheckAndCompleteExistingRequest Message not supported"));
            _LIT( KApplicationName, "Modematconroller.exe");
            aMessage.Panic( KApplicationName, KErrNotSupported );
            break;
        }
   
    if( RequestAlreadyPending ) 
        {
        OstTrace0( TRACE_NORMAL, DUP16_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - Complete KErrAlreadyExists" );
        C_TRACE (( _T("Complete KErrAlreadyExists") ));
        aMessage.Complete( KErrAlreadyExists );
        }
            
    OstTrace1( TRACE_NORMAL, DUP17_CMODEMATSESSION_CHECKANDCOMPLETEEXISTINGREQUEST, "CModemAtSession::CheckAndCompleteExistingRequest - return;RequestAlreadyPending=%x", RequestAlreadyPending );
    C_TRACE (( _T("return %d"),RequestAlreadyPending ));
    return RequestAlreadyPending;
    }

void CModemAtSession::DoServiceL( const RMessage2& aMessage )
    {
    OstTraceExt3( TRACE_NORMAL, CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL;aMessage.Function()=%d;aMessage=%p;aMessage.Handle()=%x", aMessage.Function(), &aMessage, (TUint)aMessage.Handle() );
    C_TRACE (( _T("CModemAtSession::DoServiceL() function: %d message: 0x%x handle: 0x%x"), aMessage.Function(), &aMessage, aMessage.Handle() ));

    if( CheckAndCompleteExistingRequest( aMessage ) )
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - return" );
        C_TRACE (( _T("CModemAtSession::DoServiceL() - return") ));
        return;
        }

    switch ( aMessage.Function() )
        {
        case EATExtSetExtensionInterface:
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EATExtSetExtensionInterface" );
            C_TRACE(_L("EATExtSetExtensionInterface"));
            iPluginType = (TATPluginInterface)aMessage.Int0();
            if(iClientName) 
               {
                delete iClientName;
                iClientName = NULL;
               }
            iClientName = HBufC8::New( aMessage.GetDesLength( KATModemResponseArgumentIndex ));
            iClientNamePtr.Set( iClientName->Des() );
            aMessage.Read( KATModemResponseArgumentIndex, iClientNamePtr );
            iConnectReq = aMessage;
            iServer.ConnectToModemL( this, iPluginType );
            break;

        case EReceiveUnsolicitedResult:
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EReceiveUnsolicitedResult" );
            C_TRACE(_L("EReceiveUnsolicitedResult"));
            iUnsolicitedDataReq = aMessage;
            if( iServer.IsUnsolicitedDataInBuffer() )
                {
                // there is data that can be completed
                iServer.GetUnsolicitedDataInBuffer( iTemporaryUnsolicitedData );
                if( iTemporaryUnsolicitedData.Length() > 0 )
                    {
                    UnsolicitedData( iTemporaryUnsolicitedData );
                    iTemporaryUnsolicitedData.Zero();
                    iServer.ResetUnsolicitedDataInBuffer();
                    }
                break;
                }
            break;

        case EReceiveUnsolicitedResultCancel:
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EReceiveUnsolicitedResultCancel" );
            OstTrace0( TRACE_NORMAL, DUP5_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - Complete" );
            C_TRACE(_L("EReceiveUnsolicitedResultCancel"));
            C_TRACE(_L("Complete"));
            iUnsolicitedDataReq.Complete(KErrCancel);    
            aMessage.Complete(KErrNone);
            break;

        case EModemATHandleCommandCancel:
            OstTrace0( TRACE_NORMAL, DUP6_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemATHandleCommandCancel" );
            C_TRACE(_L("EModemATHandleCommandCancel"));
            // actual message is canceled from  fifo
            iServer.CancelFifoRequests( this );
            aMessage.Complete(KErrNone);
            break;

        case EModemATCommandModeCancel:
            OstTrace0( TRACE_NORMAL, DUP7_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemATCommandModeCancel" );
            C_TRACE (( _T("EModemATCommandModeCancel") ));
            iCommandModeReq.Complete(KErrCancel);
            aMessage.Complete(KErrNone);
            break;

        case EModemAtRemovePipe:
            OstTrace0( TRACE_NORMAL, DUP8_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemAtRemovePipe" );
            C_TRACE (( _T("EModemAtRemovePipe") ));
            iServer.RemovePipe( iHandler );
            aMessage.Complete(KErrNone);
            break;

        case EModemATReceiveSignalIndCancel:
            OstTrace0( TRACE_NORMAL, DUP9_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemATReceiveSignalIndCancel" );
            OstTrace0( TRACE_NORMAL, DUP10_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - Complete" );
            C_TRACE(_L("EModemATReceiveSignalIndCancel"));
            C_TRACE(_L("Complete"));
            iSignalIndReq.Complete(KErrCancel);
            aMessage.Complete(KErrNone);
            break;
           
        case EModemATReceiveSignalInd:
            OstTrace0( TRACE_NORMAL, DUP11_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemATReceiveSignalInd" );
            C_TRACE(_L("EModemATReceiveSignalInd"));
            iSignalIndReq = aMessage;           
            break;
            
        case EModemATHandleCommand:
            {
            OstTrace0( TRACE_NORMAL, DUP12_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemATHandleCommand" );
            C_TRACE(_L("EModemATHandleCommand"));
            CAtMessage* message = new CAtMessage( EAtCmd, this, aMessage );
            TRACE_ASSERT( message );
            OstTrace0( TRACE_NORMAL, DUP13_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - iServer.AddToSendFifo EAtCmd" );
            C_TRACE(_L("iServer.AddToSendFifo EAtCmd"));
            iServer.AddToSendFifo( iPluginType, message  );
            }
            break;
        
        case EModemATCommandMode:
            {
            OstTrace0( TRACE_NORMAL, DUP14_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemATCommandMode" );
            C_TRACE(_L("EModemATCommandMode"));
            iCommandModeReq = aMessage;    
            }
            break;
        
        case EModemATGetNvramStatus:
            {
            OstTrace0( TRACE_NORMAL, DUP15_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - EModemATGetNvramStatus" );
            C_TRACE(_L("EModemATGetNvramStatus"));
            if( iServer.IsPipeInDataMode( iHandler ) ||
                iServer.IsTextMessageMode( iHandler ) )
                {
                // AT&V should not be sent from common plugin in data or text message mode
                aMessage.Complete( KErrGeneral );
                break;
                }

            CAtMessage* message = new CAtMessage( EGetNvramStatus, this, aMessage );
            TRACE_ASSERT( message );
            OstTrace0( TRACE_NORMAL, DUP16_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - iServer.AddToSendFifo EGetNvramStatus" );
            C_TRACE(_L("iServer.AddToSendFifo EGetNvramStatus"));
            iServer.AddToSendFifo( iPluginType, message  );
            break;
            }
        default:
            {
            OstTrace0( TRACE_NORMAL, DUP17_CMODEMATSESSION_DOSERVICEL, "CModemAtSession::DoServiceL - message not supported" );
            C_TRACE(_L("CModemAtSession::DoServiceL message not supported"));
            aMessage.Complete( KErrNotSupported );
            _LIT( KApplicationName, "Modematconroller.exe");
            aMessage.Panic( KApplicationName, KErrNotSupported );
            break;
            }
        }  
    }

CModemAtSession::CModemAtSession( CModemAtSrv& aServer,
    const TVersion& aVersion ) :
    iServer( aServer ),
    iVersion( aVersion ),
    iDteId( 0xFF ),
    iClientName( NULL ),
    iClientNamePtr( NULL, 0 ),
    iHandler( NULL )
    {
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATSESSION_CMODEMATSESSION, "CModemAtSession::CModemAtSession" );
    C_TRACE (( _T("CModemAtSession::CModemAtSession()") ));
    }

void CModemAtSession::UnsolicitedData( const TDesC8& aData ) 
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSESSION_UNSOLICITEDDATA, "CModemAtSession::UnsolicitedData" );
    C_TRACE (( _T("CModemAtSession::UnsolicitedData()") ));
    if(!iUnsolicitedDataReq.IsNull())
        {

        TInt length = iUnsolicitedDataReq.GetDesMaxLength( KATModemUnsolicitedDataIndex );
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSESSION_UNSOLICITEDDATA, "CModemAtSession::UnsolicitedData - iUnsolicitedDataReq;length=%d", length );
        C_TRACE((_L("iUnsolicitedDataReq length: %d"), length ));

        HBufC8* unsolicitedBuffer = HBufC8::New( length );
        TPtr8 ptr = unsolicitedBuffer->Des();
        iUnsolicitedDataReq.Read( KATModemUnsolicitedDataIndex, ptr );
        unsolicitedBuffer->Des().Append( aData );
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATSESSION_UNSOLICITEDDATA, "CModemAtSession::UnsolicitedData - unsolicitedBuffer->Des() contents:;ptr=%x", &ptr );
        OstTraceExt1( TRACE_NORMAL, DUP3_CMODEMATSESSION_UNSOLICITEDDATA, "CModemAtSession::UnsolicitedData - dump;aData=%s", aData );
        C_TRACE((_L("unsolicitedBuffer->Des() contents: 0x%x"), &ptr ));
        DUMP_MESSAGE( aData );
        
        TInt err = iUnsolicitedDataReq.Write( KATModemUnsolicitedDataIndex, unsolicitedBuffer->Des() );
        delete unsolicitedBuffer;

        OstTrace1( TRACE_NORMAL, DUP4_CMODEMATSESSION_UNSOLICITEDDATA, "CModemAtSession::UnsolicitedData - Complete;err=%d", err );
        C_TRACE (( _T("Complete %d"),err ));
        iUnsolicitedDataReq.Complete( err );

        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    }

TUint8 CModemAtSession::GetDteId() 
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSESSION_GETDTEID, "CModemAtSession::GetDteId" );
    C_TRACE (( _T("CModemAtSession::GetDteId()") ));
    return iDteId;    
    }


TATPluginInterface CModemAtSession::GetPluginType()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_GETPLUGINTYPE, "CModemAtSession::GetPluginType;iPluginType=%d", iPluginType );
    C_TRACE (( _T("CCModemAtSession::GetPluginType(%d)"),iPluginType ));
    return iPluginType;
    }

TDesC8& CModemAtSession::GetName() 
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSESSION_GETNAME, "CModemAtSession::GetName" );
    C_TRACE( _T("CModemAtSession::GetName()") ); 
    return *iClientName;
    }

void CModemAtSession::SetDteId( const TUint8 aDteId ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_SETDTEID, "CModemAtSession::SetDteId;aDteId=%d", aDteId );
    C_TRACE(( _T("CModemAtSession::SetDteId( %d )"), aDteId ));
    iDteId = aDteId;   
    }

void CModemAtSession::ModemConnected( const TInt aErr ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_MODEMCONNECTED, "CModemAtSession::ModemConnected;aErr=%d", aErr );
    C_TRACE(( _T("CModemAtSession::ModemConnected( %d )"), aErr ));
    if( !iConnectReq.IsNull() ) 
        {
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSESSION_MODEMCONNECTED, "CModemAtSession::ModemConnected - Complete iConnectReq;aErr=%d", aErr );
        C_TRACE (( _T("Complete iConnectReq( %d )"), aErr ));
        iConnectReq.Complete(aErr);
        }
    }

void CModemAtSession::SignalIndReceived( const TInt aCompleteCode ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_SIGNALINDRECEIVED, "CModemAtSession::SignalIndReceived;aCompleteCode=%d", aCompleteCode );
    C_TRACE(( _T("CModemAtSession::SignalIndReceived( %d )"), aCompleteCode ));
    if(!iSignalIndReq.IsNull())
        {
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSESSION_SIGNALINDRECEIVED, "CModemAtSession::SignalIndReceived;iSignalIndReq=%x", &iSignalIndReq );
        C_TRACE (( _T("Complete iSignalIndReq( %d ) iSignalIndReq: 0x%x"), aCompleteCode, &iSignalIndReq ));
        iSignalIndReq.Complete( aCompleteCode );
        }
    }

void CModemAtSession::CommandModeChanged( TCommandMode aMode ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_COMMANDMODECHANGED, "CModemAtSession::CommandModeChanged;aMode=%d", (TInt)aMode );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATSESSION_COMMANDMODECHANGED, "CModemAtSession::CommandModeChanged - Writing mode to iCommandModeReq;aMode=%d", (TInt)aMode );
    C_TRACE((_L("CModemAtSession::CommandModeChanged( %d )"), (TInt) aMode ));
    C_TRACE((_L("Writing mode to iCommandModeReq %d"), (TInt) aMode ));

    if( !iCommandModeReq.IsNull() )
        {
        TInt length = iCommandModeReq.GetDesMaxLength( KATModemCommandArgumentIndex );
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATSESSION_COMMANDMODECHANGED, "CModemAtSession::CommandModeChanged - iCommandModeReq;length=%d", length );
        C_TRACE((_L("iCommandModeReq length: %d"), length ));

        HBufC8* modeBuffer = HBufC8::New( length );
        TPtr8 ptr = modeBuffer->Des();
        iCommandModeReq.Read( KATModemCommandArgumentIndex, ptr );
        modeBuffer->Des().AppendNum( (TInt) aMode );
        OstTrace1( TRACE_NORMAL, DUP3_CMODEMATSESSION_COMMANDMODECHANGED, "CModemAtSession::CommandModeChanged - modeBuffer->Des() contents:;ptr=%x", &ptr );
        OstTraceExt1( TRACE_NORMAL, DUP4_CMODEMATSESSION_COMMANDMODECHANGED, "CModemAtSession::CommandModeChanged - dump;ptr=%s", ptr );
        C_TRACE((_L("modeBuffer->Des() contents: 0x%x"), &ptr ));
        DUMP_MESSAGE( ptr );
        
        TInt err = iCommandModeReq.Write( KATModemCommandArgumentIndex, modeBuffer->Des() );
        delete modeBuffer;

        OstTrace1( TRACE_NORMAL, DUP5_CMODEMATSESSION_COMMANDMODECHANGED, "CModemAtSession::CommandModeChanged - Complete;err=%d", err );
        C_TRACE (( _T("Complete %d"),err ));
        iCommandModeReq.Complete( err );
        }
    else
        {
        OstTrace1( TRACE_NORMAL, DUP6_CMODEMATSESSION_COMMANDMODECHANGED, "CModemAtSession::CommandModeChanged - CommandModeChanged Ignored!!!;aMode=%d", aMode );
        C_TRACE((_L("CommandModeChanged Ignored!!! Mode: %d"), aMode ));
        }
    }

TBool CModemAtSession::IsUnsolicitedDataReqActive()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_ISUNSOLICITEDDATAREQACTIVE, "CModemAtSession::IsUnsolicitedDataReqActive - ret:;%d", (TInt) !iUnsolicitedDataReq.IsNull() );
    C_TRACE(( _T("CModemAtSession::IsUnsolicitedDataReqActive ret: %d"), (TInt) !iUnsolicitedDataReq.IsNull() ));
    return !iUnsolicitedDataReq.IsNull();
    }

TBool CModemAtSession::IsSignalIndReqActive()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_ISSIGNALINDREQACTIVE, "CModemAtSession::IsSignalIndReqActive - ret:;%d", (TInt) !iSignalIndReq.IsNull() );
    C_TRACE(( _T("CModemAtSession::IsSignalIndReqActive ret: %d"), (TInt) !iSignalIndReq.IsNull() ));
    return !iSignalIndReq.IsNull();
    }

TBool CModemAtSession::IsCommandModeReqActive()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_ISCOMMANDMODEREQACTIVE, "CModemAtSession::IsCommandModeReqActive - ret:;%d", (TInt) !iCommandModeReq.IsNull() );
    C_TRACE(( _T("CModemAtSession::IsCommandModeReqActive ret: %d"), (TInt) !iCommandModeReq.IsNull() ));
    return !iCommandModeReq.IsNull();
    }

TBool CModemAtSession::IsConnectReqActive()
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_ISCONNECTREQACTIVE, "CModemAtSession::IsConnectReqActive - ret:;%d", (TInt) !iConnectReq.IsNull() );
    C_TRACE(( _T("CModemAtSession::IsConnectReqActive ret: %d"), (TInt) !iConnectReq.IsNull() ));
    return !iConnectReq.IsNull();
    }

void CModemAtSession::SetHandler( CModemAtHandler* aHandler )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATSESSION_SETHANDLER, "CModemAtSession::SetHandler;aHandler=%x", aHandler );
    C_TRACE (( _T("CModemAtSession::SetHandler() 0x%x"), aHandler ));
    TRACE_ASSERT( NULL != aHandler );
    iHandler = aHandler;
    }

CModemAtHandler* CModemAtSession::GetHandler()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATSESSION_GETHANDLER, "CModemAtSession::GetHandler" );
    C_TRACE (( _T("CModemAtSession::GetHandler()") ));
    return iHandler;
    }


