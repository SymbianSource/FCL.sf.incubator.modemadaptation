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



#include "modemat_atext.h"
#include "modemattrace.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "modemat_atextTraces.h"
#endif

const TInt KReplyTypeEditorStart( 1 );
const TInt KReplyTypeEditorEnd( 2 );
const TInt KReplyTypeOther( 3 );
const TInt KReplyTypeEditor( 4 );
const TUint8 KEscChar( 0x1B ); // from ascii table
const TUint8 KCtrlZChar( 0x1A ); // from ascii table
const TUint8 KEscDataSize( 2 );
const TUint8 KErrorSize( 9 );

const char* const atCommands[] =
    {
    ("ATS0"),
    ("ATS1"),
    ("ATS2"),
    ("ATS3"),
    ("ATS4"),
    ("ATS5"),
    ("ATS6"),
    ("ATS7"),
    ("ATS8"),
    ("ATS10"),
    ("ATS12"),
    ("ATS25"),
    ("ATE"),
    ("ATE0"),
    ("ATE1"),
    ("ATQ"),
    ("ATQ0"),
    ("ATQ1"),
    ("ATV"),
    ("ATV0"),
    ("ATV1"),
    ("ATX"),
    ("ATX0"),
    ("ATX1"),
    ("ATX2"),
    ("ATX3"),
    ("ATX4"),
    ("ATX5"),
    ("AT&C"),
    ("AT&C0"),
    ("AT&C1"),
    ("AT&D"),
    ("AT&D0"),
    ("AT&D1"),
    ("AT&D2"),
    ("AT&D3"),
    ("AT+IFC"),
    ("AT+ILRR"),
    ("AT&S"),
    ("AT&S0"),
    ("AT&S1"),
    ("AT&K"),
    ("AT&K0"),
    ("AT&K3"),
    ("AT&K4"),
    ("ATZ"),
    ("ATZ0"),
    ("ATZ1"),
    ("AT&F"),
    ("AT&F0"),
    ("ATI"), //ati0-255 accepted
    ("AT+GMI"),
    ("AT+GMM"),
    ("AT+GMR"),
    ("AT+GSN"),
    ("AT+GCAP"),
    ("AT&V"),
    ("AT&V0"),
    ("AT&V1"),
    ("AT&V2"),
    ("AT&W"),
    ("AT&W0"),
    ("AT&W1"),
    ("AT&Y"),
    ("AT&Y0"),
    ("AT&Y1"),
    ("AT+CGMI"),
    ("AT+CGMM"),
    ("AT+CGMR"),
    ("AT+CGSN"),
    ("AT+CSCS"),
    ("AT+CIMI"),
    ("AT+WS46"),
    ("ATD"),
    ("ATA"),
    ("ATH"),
    ("ATH0"),
    ("ATO"),
    ("ATO0"),
    ("AT+DS"),
    ("AT+DR"),
    ("AT+CSTA"),
    ("AT+CHUP"),
    ("AT+CR"),
    ("AT+CEER"),
    ("AT+CRC"),
    ("AT+CSNS"),
    ("AT+CVHU"),
    ("AT+CREG"),
    ("AT+CLIP"),
    ("AT+CLIR"),
    ("AT+COLP"),
    ("AT+CCFC"),
    ("AT+CCWA"),
    ("AT+CHLD"),
    ("AT+CLCC"),
    ("AT+CSSN"),
    ("AT+CPAS"),
    ("AT+CSQ"),
    ("AT+CMEE"),
    ("AT+CSMS"),
    ("AT+CPMS"),
    ("AT+CMGF"),
    ("AT+CSCA"),
    ("AT+CSMP"),
    ("AT+CSDH"),
    ("AT+CSAS"),
    ("AT+CRES"),
    ("AT+CMGS"),
    ("AT+CMGC"),
    ("AT+CMMS"),
    ("AT+CGSMS"),
    ("AT+FCLASS"),
    ("AT+VTS"),
    ("AT+CGDCONT"),
    ("AT+CGEQREQ"),
    ("AT+CGEQMIN"),
    ("AT+CGEQNEG"),
    ("AT+CGREG"),
    ("AT+CGATT"),
    ("AT+CGACT"),
    ("AT+CGDATA"),
    ("AT+CGDSCONT"),
    ("AT+CGTFT"),
    ("AT+CGCMOD"),
    ("AT+CSIM"),
    ("AT+CPNET"),
    ("AT+CPNSTAT"),
    ("AT+CGPADDR"),
    ("AT"),
    ("LAST")
    };

const TInt KMaxCommandCount(sizeof(atCommands)/sizeof(const char*));

const TInt KPartLength = KDefaultCmdBufLength;
const TInt KPackets = 10;

_LIT8(KATDCommand,"ATD");
_LIT8(KATICommand,"ATI");
_LIT8(KLastCommand,"LAST");


TPtrC8 CModemATExt::GetAtCommand( TInt aNumber )  //list of supported commands for this plugin
    {
    OstTrace1( TRACE_NORMAL, CMODEMATEXT_GETATCOMMAND, "CModemATExt::GetAtCommand;aNumber=%d", aNumber );
    C_TRACE((_L("CModemATExt::GetAtCommand(%d)"), aNumber));
    const TUint8 **keys = (const TUint8**) atCommands;
    TPtrC8 keyword( aNumber < KMaxCommandCount ? keys[ aNumber ] : keys[ KMaxCommandCount - 1 ] );
    return keyword;
    }

CModemATExt* CModemATExt::NewL()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_NEWL, "CModemATExt::NewL" );
    C_TRACE((_L("CModemATExt::NewL")));
    CModemATExt* self = new (ELeave) CModemATExt();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CModemATExt::~CModemATExt()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_CMODEMATEXT, "CModemATExt::~CModemATExt" );
    C_TRACE((_L("CModemATExt::~CModemATExt()")));
    if (iReplyBuffer) 
        {
        delete iReplyBuffer;
        iReplyBuffer = NULL;
        }
    
     if(iUnsolicitedBuffer) 
         {
         delete iUnsolicitedBuffer;
         iUnsolicitedBuffer = NULL;
         } 
    if( iCommandBuf ) 
         {
         delete iCommandBuf;
         iCommandBuf = NULL;
         }
    iRModemAt.Disconnect();
    }

TBool CModemATExt::IsCommandSupported( const TDesC8& aCmd)
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported" );
    C_TRACE((_L("CModemATExt::IsCommandSupported")));
    DUMP_MESSAGE((aCmd));

    if(aCmd.Find( KATDCommand )!=KErrNotFound)  
        {
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - ATD" );
        C_TRACE((_L("ATD")));
        return ETrue;
        }
    if(aCmd.Find(KATICommand)!=KErrNotFound) 
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - ATI" );
        C_TRACE((_L("ATI")));
        return ETrue;
        }
    for ( TInt i = 0; i < KMaxCommandCount; i++ ) 
        {
        OstTrace1( TRACE_NORMAL, DUP3_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - round;i=%d", i );
        C_TRACE((_L("round %d"),i));
        if( GetAtCommand(i).Find(KLastCommand) != KErrNotFound) 
            {
            // All messages have been searched so the command is not supported
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - ********* COMMAND IS NOT SUPPORTED ********* " );
            OstTraceExt1( TRACE_NORMAL, DUP5_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported;aCmd=%s", aCmd );
            C_TRACE((_L("********* COMMAND IS NOT SUPPORTED ********* ")));
            DUMP_MESSAGE(aCmd);
            return EFalse;
            }
        
        if(aCmd.Find(GetAtCommand(i))!=KErrNotFound)  
            {
            //just command
            OstTrace1( TRACE_NORMAL, DUP6_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - Command;i=%d", i );
            C_TRACE((_L("Command"),i));
            if(aCmd.Length()==GetAtCommand(i).Length())
                {
                OstTrace1( TRACE_NORMAL, DUP7_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - Ok;i=%d", i );
                C_TRACE((_L("Ok"),i));
                return ETrue;
                }
            
            if(aCmd.Length()>GetAtCommand(i).Length()) //setting command 
                {
                OstTrace0( TRACE_NORMAL, DUP8_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - Setting" );
                C_TRACE((_L("Setting")));
                if( aCmd[GetAtCommand(i).Length()] == '=' ||
                   aCmd[GetAtCommand(i).Length()] == '?' )
                   {
                   OstTrace1( TRACE_NORMAL, DUP9_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - Ok;i=%d", i );
                   C_TRACE((_L("Ok"),i));
                   return ETrue;
                   }
                }
            }
        }
    OstTrace0( TRACE_NORMAL, DUP10_CMODEMATEXT_ISCOMMANDSUPPORTED, "CModemATExt::IsCommandSupported - return EFalse" );
    C_TRACE((_L("return EFalse")));
    return EFalse; 
    }

void CModemATExt::HandleCommand( const TDesC8& aCmd, RBuf8& aReply, TBool aReplyNeeded )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATEXT_HANDLECOMMAND, "CModemATExt::HandleCommand() - is reply needed;aReplyNeeded=%d", aReplyNeeded );
    OstTraceExt1( TRACE_NORMAL, DUP1_CMODEMATEXT_HANDLECOMMAND, "CModemATExt::HandleCommand - dump;aCmd=%s", aCmd );
    C_TRACE((_L("CModemATExt::HandleCommand() is reply needed: %d" ), aReplyNeeded));
    DUMP_MESSAGE(aCmd);

    if( iIsTextInputMode )
        {
        OstTrace0( TRACE_NORMAL, DUP2_CMODEMATEXT_HANDLECOMMAND, "CModemATExt::HandleCommand - text mode on" );
        C_TRACE((_L("CModemATExt::HandleCommand: text mode on") ));
        AddDataToBuffer( aCmd, aReply );
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATEXT_HANDLECOMMAND, "CModemATExt::HandleCommand - normal mode on" );
        C_TRACE((_L("CModemATExt::HandleCommand: normal mode on") ));
        iReply = &aReply;
        iReplyNeeded = aReplyNeeded;
        if( iCommandBuf ) 
             {
             delete iCommandBuf;
             iCommandBuf = NULL;
             }
        iCommandBuf = HBufC8::New( aCmd.Length() + 1 );
        TPtr8 ptr = iCommandBuf->Des();
        ptr.Append( aCmd );
        ptr.Append( iCarriageReturn );
        iRModemAt.HandleATCommand( ptr, iReplyPtr );
        }
    }

void CModemATExt::HandleCommandCancel()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_HANDLECOMMANDCANCEL, "CModemATExt::HandleCommandCancel" );
    C_TRACE((_L("CModemATExt::HandleCommandCancel()")));
    iRModemAt.HandleATCommandCancel();
    }

TInt CModemATExt::NextReplyPartLength()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_NEXTREPLYPARTLENGTH, "CModemATExt::NextReplyPartLength" );
    C_TRACE((_L("CModemATExt::NextReplyPartLength()")));
    return Min( iReplyPtr.Length(), KPartLength);
    }

TInt CModemATExt::GetNextPartOfReply( RBuf8& aNextReply )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_GETNEXTPARTOFREPLY, "CModemATExt::GetNextPartOfReply" );
    C_TRACE((_L("CModemATExt::GetNextPartOfReply()")));
    aNextReply.Create(iReplyPtr,KPartLength);
    iReplyPtr.Delete(0,KPartLength);
    return KErrNone;
    }

void CModemATExt::ReceiveUnsolicitedResult()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_RECEIVEUNSOLICITEDRESULT, "CModemATExt::ReceiveUnsolicitedResult" );
    C_TRACE((_L("CModemATExt::ReceiveUnsolicitedResult()")));
    iRModemAt.ReceiveUnsolicitedResult( iUnsolicitedPtr );
    }

void CModemATExt::ReceiveUnsolicitedResultCancel()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_RECEIVEUNSOLICITEDRESULTCANCEL, "CModemATExt::ReceiveUnsolicitedResultCancel" );
    C_TRACE((_L("CModemATExt::ReceiveUnsolicitedResultCancel")));
    iRModemAt.ReceiveUnsolicitedResultCancel();
    }

TBool CModemATExt::HandleEditorModeInput( const TDesC8& /*aInput*/,
                                                RBuf8& /*aReply*/,
                                                TBool& /*aEchoOn*/)
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_HANDLEEDITORMODEINPUT, "CModemATExt::HandleEditorModeInput" );
    C_TRACE((_L("CModemATExt::HandleEditorModeInput()")));
    return EFalse;  //commands are execute commands, this function is newer called
    }

void CModemATExt::ReportNvramStatusChange( const TDesC8& /*aNvram*/ ) 
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_REPORTNVRAMSTATUSCHANGE, "CModemATExt::ReportNvramStatusChange" );
    C_TRACE((_L("CModemATExt::ReportNvramStatusChange()")));
    //Ignored in this plugin
    }


CModemATExt::CModemATExt() : 
    iReplyBuffer(NULL), 
    iReplyPtr(0, 0), 
    iUnsolicitedPtr(0, 0), 
    iUnsolicitedBuffer(NULL),
    iReply(NULL),
    iReplyNeeded(EFalse),
    iIsTextInputMode( EFalse ),
    iDiscardNextReply( EFalse )
    {
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATEXT_CMODEMATEXT, "CModemATExt::CModemATExt" );
    C_TRACE((_L("CModemATExt::CModemATExt()")));
    }
     
void CModemATExt::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_CONSTRUCTL, "CModemATExt::ConstructL" );
    C_TRACE((_L("CModemATExt::ConstructL()")));
    iReplyBuffer = HBufC8::NewL( KDefaultCmdBufLength * KPackets ); 
    iReplyPtr.Set( iReplyBuffer->Des() );
    iUnsolicitedBuffer = HBufC8::NewL( KDefaultCmdBufLength );
    iUnsolicitedPtr.Set( iUnsolicitedBuffer->Des() );
    }

void CModemATExt::ReportConnectionName( const TDesC8& aName ) 
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_REPORTCONNECTIONNAME, "CModemATExt::ReportConnectionName" );
    C_TRACE((_L("CModemATExt::ReportConnectionName()")));
    DUMP_MESSAGE((aName));
    iName.Copy(aName);
    iRModemAt.Connect(EATExtPlugin, iName, this);
    }

void CModemATExt::HandleATCommandCompleted( TInt aErr ) 
    {
    OstTrace1( TRACE_NORMAL, CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted;aErr=%d", aErr );
    C_TRACE((_L("CModemATExt::HandleATCommandCompleted( %d )"), aErr));	

    if( !iDiscardNextReply )
        {
        if(iReplyNeeded) 
            {
            OstTrace0( TRACE_NORMAL, DUP1_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted - Reply needed" );
            C_TRACE((_L("Reply needed")));
            iReply->Create( iReplyPtr,KPartLength );
            }
        iReplyPtr.Delete(0, KPartLength);

        if( aErr == KErrNone) 
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted - No errors" );
            C_TRACE((_L("No errors")));
            if( iIsTextInputMode )
                {
                iIsTextInputMode = EFalse;
                }
            HandleCommandCompleted( aErr, EReplyTypeOk);
            }
	    else if( aErr == KErrGeneral )
            {
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted - Not supported (EReplyTypeUndefined)" );
            C_TRACE((_L("Not supported (EReplyTypeUndefined)")));
			if( iIsTextInputMode )
                {
                iIsTextInputMode = EFalse;
                }
            HandleCommandCompleted( aErr, EReplyTypeUndefined );
            }
        else if( aErr == KReplyTypeOther )
            {
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted - No errors (EReplyTypeOther)" );
            C_TRACE((_L("No errors (EReplyTypeOther)")));
            if( iIsTextInputMode )
                {
                iIsTextInputMode = EFalse;
                }
            HandleCommandCompleted( KErrNone, EReplyTypeOther);
            }
        else if( aErr == KReplyTypeEditorStart ) 
            {
            OstTrace0( TRACE_NORMAL, DUP5_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted - EReplyTypeEditorStart" );
            C_TRACE((_L("EReplyTypeEditorStart")));
            if( !iIsTextInputMode )
                {
                iIsTextInputMode = ETrue;
                HandleCommandCompleted( KErrNone, static_cast<TATExtensionReplyType>( KReplyTypeEditor ) );
                }
            }
        else if( aErr == KReplyTypeEditorEnd ) 
            {
            OstTrace0( TRACE_NORMAL, DUP6_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted - EReplyTypeEditorEnd" );
            C_TRACE((_L("EReplyTypeEditorEnd")));
            iIsTextInputMode = EFalse;
            HandleCommandCompleted( KErrNone, EReplyTypeOk);
            }
        else 
            {
            OstTrace0( TRACE_NORMAL, DUP7_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted - Reply Error" );
            C_TRACE((_L("Reply Error")));
            if( iIsTextInputMode )
                {
                iIsTextInputMode = EFalse;
                }
            HandleCommandCompleted( aErr, EReplyTypeError );
            }
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP8_CMODEMATEXT_HANDLEATCOMMANDCOMPLETED, "CModemATExt::HandleATCommandCompleted -  reply discarded" );
        C_TRACE((_L("CModemATExt::HandleATCommandCompleted: reply discarded")));
        iDiscardNextReply = EFalse;
        }
    }

void CModemATExt::HandleUnsolicitedResultReceived( TInt aErr )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATEXT_HANDLEUNSOLICITEDRESULTRECEIVED, "CModemATExt::HandleUnsolicitedResultReceived;aErr=%d", aErr );
    C_TRACE((_L("CModemATExt::HandleUnsolicitedResultReceived > ( %d )"), aErr));
    if( KErrNone == aErr )
        {
        SendUnsolicitedResult( iUnsolicitedPtr );
        TInt length = iUnsolicitedPtr.Length();
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATEXT_HANDLEUNSOLICITEDRESULTRECEIVED, "CModemATExt::HandleUnsolicitedResultReceived -  reply discarded;length=%d", length );
        C_TRACE((_L("clear unsolicited data buffer, length %d"), length));
        iUnsolicitedPtr.Delete( 0, length );
        }
    C_TRACE((_L("CModemATExt::HandleUnsolicitedResultReceived <")));
    }

void CModemATExt::HandleSignalIndication( TInt /*aErr*/ )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_HANDLESIGNALINDICATION, "CModemATExt::HandleSignalIndication" );
    C_TRACE((_L("CModemATExt::HandleSignalIndication")));
    //COMMON plugin handles signal indications
    }

void CModemATExt::ReportExternalHandleCommandError()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_REPORTEXTERNALHANDLECOMMANDERROR, "CModemATExt::ReportExternalHandleCommandError" );
    C_TRACE((_L("CModemATExt::ReportExternalHandleCommandError()")));
    //external error occurred, cancel AT-command
    iRModemAt.HandleATCommandCancel();
    }

void CModemATExt::HandleCommandModeChanged( TInt /*aErr*/, TCommandMode /*aMode*/ )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_HANDLECOMMANDMODECHANGED, "CModemATExt::HandleCommandModeChanged" );
    C_TRACE((_L("CModemATExt::HandleCommandModeChanged()")));
    //COMMON plugin handles mode change requests
    }

void CModemATExt::AddDataToBuffer( 
    const TDesC8& aCmd, 
    RBuf8& aReply )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer" );
    C_TRACE((_L("CModemATExt::AddDataToBuffer()")));

    TBool bufferingCanEnd( EFalse );
    TBool bufferExceeded( EFalse );

    TInt offsetEscChar = aCmd.Locate( KEscChar );
    TInt offsetCtrlZChar = aCmd.Locate( KCtrlZChar );
	OstTrace1( TRACE_NORMAL, DUP1_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer: offsetEscChar: %d", offsetEscChar );
	OstTrace1( TRACE_NORMAL, DUP2_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer: offsetCtrlZChar: %d", offsetCtrlZChar );
    C_TRACE(( _L("CModemATExt::AddDataToBuffer: offsets: esc: %d, ctrl-z: %d"), offsetEscChar, offsetCtrlZChar ));

    if( KErrNotFound != offsetEscChar || 
        KErrNotFound != offsetCtrlZChar )
        {
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer - end char detected" );
        C_TRACE((_L("CModemATExt::AddDataToBuffer: end char detected") ));
        bufferingCanEnd = ETrue;
        }

    if( KMaxSmsBufferSize < ( aCmd.Length() +  iSmsBuffer.Length() ) )
        {
        if( bufferingCanEnd )
            {
            TInt offset = 0;
            if( KErrNotFound != offsetEscChar )
                {
                OstTrace0( TRACE_NORMAL, DUP4_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer: (truncated, esc)" );
                C_TRACE((_L("CModemATExt::AddDataToBuffer: buffering data (truncated) (esc char detected)") ));
                offset = offsetEscChar + 1;
                }
            else
                {
				OstTrace0( TRACE_NORMAL, DUP5_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer: (truncated, ctrl-z)" );
                C_TRACE((_L("CModemATExt::AddDataToBuffer: buffering data (truncated) (ctrl-z detected)") ));
                offset = offsetCtrlZChar + 1;
                }
            TInt smsBufferSize = iSmsBuffer.Length() + aCmd.Mid( 0, offset ).Length();
            OstTrace1( TRACE_NORMAL, DUP6_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer: current + received: %d", smsBufferSize );
			C_TRACE((_L("CModemATExt::AddDataToBuffer: current + received buffer length: %d"), smsBufferSize ));
            if( KMaxSmsBufferSize >= smsBufferSize )
                {
                iSmsBuffer.Append( aCmd.Mid( 0, offset ) );
                C_TRACE((_L("CModemATExt::AddDataToBuffer: sending data (end char detected)") ));
                SendSmsBuffer( aReply, ETrue, EFalse );
                }
            else
                {
                bufferExceeded = ETrue;
                }
            }

        if( !bufferingCanEnd ||
            bufferExceeded )
            {
			OstTrace0( TRACE_NORMAL, DUP7_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer - too big data or esc" );
            C_TRACE((_L("CModemATExt::AddDataToBuffer: data is too big to buffer or esc char detected") ));

            iDiscardNextReply = ETrue;
            iIsTextInputMode = EFalse;

            SendSmsBuffer( aReply, EFalse, ETrue );

            TBuf8<KErrorSize> reply;
            reply.Append( iCarriageReturn );
            reply.Append( iLineFeed );
            reply.Append( _L( "ERROR" ) );
            reply.Append( iCarriageReturn );
            reply.Append( iLineFeed );

            aReply.Create( reply, reply.Length() );
            HandleCommandCompleted( KErrNone, EReplyTypeOk );
            }
        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP8_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer - buffering data" );
        C_TRACE((_L("CModemATExt::AddDataToBuffer: buffering data") ));
        iSmsBuffer.Append( aCmd );
        if( bufferingCanEnd )
            {
            OstTrace0( TRACE_NORMAL, DUP9_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer - sending data in text mode" );
            C_TRACE((_L("CModemATExt::AddDataToBuffer: sending data in text mode") ));
            SendSmsBuffer( aReply, ETrue, EFalse );
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP10_CMODEMATEXT_ADDDATATOBUFFER, "CModemATExt::AddDataToBuffer - data is not sent yet, completing request" );
            C_TRACE((_L("CModemATExt::AddDataToBuffer: data is not sent yet, let's complete request") ));
            HandleCommandCompleted( KErrNone, static_cast<TATExtensionReplyType>( KReplyTypeEditor ) );
            }
        }
    }

void CModemATExt::SendSmsBuffer(
    RBuf8& aReply,
    TBool aReplyNeeded,
    TBool aCancel )
    {
    OstTrace0( TRACE_NORMAL, CMODEMATEXT_SENDSMSBUFFER, "CModemATExt::SendSmsBuffer()" );
	C_TRACE((_L("CModemATExt::SendSmsBuffer() replyNeeded: %d, cancel: %d"), (TInt) aReplyNeeded, (TInt) aCancel ));
    iReply = &aReply;
    iReplyNeeded = aReplyNeeded;
    if( iCommandBuf ) 
        {
        delete iCommandBuf;
        iCommandBuf = NULL;
        }
    if( !aCancel )
        {
        iCommandBuf = HBufC8::New( iSmsBuffer.Length() + 1 );
        TPtr8 ptr = iCommandBuf->Des();
        ptr.Append( iSmsBuffer );
        ptr.Append( iCarriageReturn );
        iRModemAt.HandleATCommand( ptr, iReplyPtr );
        }
    else
        {
        iCommandBuf = HBufC8::New( KEscDataSize );
        TPtr8 ptr = iCommandBuf->Des();
        ptr.Append( KEscChar );
        ptr.Append( iCarriageReturn );
        iRModemAt.HandleATCommand( ptr, iReplyPtr );
        }
    iSmsBuffer.Zero();
	OstTrace0( TRACE_NORMAL, DUP1_CMODEMATEXT_SENDSMSBUFFER, "CModemATExt::SendSmsBuffer() done" );
    }

//  End of File

