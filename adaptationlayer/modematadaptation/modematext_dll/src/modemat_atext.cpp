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



#include "modemat_atext.h"
#include "modemattrace.h"

const TInt KMaxCommandCount(137);

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
    ("ATS25"),
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
    ("ATT"),
    ("ATP"),
    ("AT+DS"),
    ("AT+DR"),
    ("ATB"),
    ("ATB0"),
    ("ATB1"),
    ("ATB2"),
    ("AT+CSTA"),
    ("AT+CHUP"),
    ("AT+CR"),
    ("AT+CEER"),
    ("AT+CRC"),
    ("AT+CSNS"),
    ("AT+CVHU"),
    ("AT+CREG"),
    ("AT+COPS"),
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
    ("AT+CBCIE"),
    ("AT"),
    ("LAST")
    };
 const TInt KPartLength = KDefaultCmdBufLength;
 const TInt KPackets = 10;

_LIT8(KATDCommand,"ATD");
_LIT8(KATICommand,"ATI");
_LIT8(KLastCommand,"LAST");


TPtrC8 CModemATExt::GetAtCommand( TInt aNumber )  //list of supported commands for this plugin
    {
    C_TRACE((_L("CModemATExt::GetAtCommand(%d)"), aNumber));
    const TUint8 **keys = (const TUint8**) atCommands;
    TPtrC8 keyword( aNumber < KMaxCommandCount ? keys[ aNumber ] : keys[ KMaxCommandCount - 1 ] );
    return keyword;
    }

CModemATExt* CModemATExt::NewL()
    {
    C_TRACE((_L("CModemATExt::NewL")));
    CModemATExt* self = new (ELeave) CModemATExt();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CModemATExt::~CModemATExt()
    {
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
    C_TRACE((_L("CModemATExt::IsCommandSupported")));
    DUMP_MESSAGE((aCmd));

    if(aCmd.Find( KATDCommand )!=KErrNotFound)  
        {
        C_TRACE((_L("ATD")));
        return ETrue;
        }
    if(aCmd.Find(KATICommand)!=KErrNotFound) 
        {
        C_TRACE((_L("ATI")));
        return ETrue;
        }
    for ( TInt i = 0; i < KMaxCommandCount; i++ ) 
        {
        C_TRACE((_L("round %d"),i));
        if( GetAtCommand(i).Find(KLastCommand) != KErrNotFound) 
            {
            // All messages have been searched so the command is not supported
            C_TRACE((_L("********* COMMAND IS NOT SUPPORTED ********* ")));
            DUMP_MESSAGE(aCmd);
            return EFalse;
            }
        
        if(aCmd.Find(GetAtCommand(i))!=KErrNotFound)  
            {
            //just command
            C_TRACE((_L("Command"),i));
            if(aCmd.Length()==GetAtCommand(i).Length())
                {
                C_TRACE((_L("Ok"),i));
                return ETrue;
                }
            
            if(aCmd.Length()>GetAtCommand(i).Length()) //setting command 
                {
                C_TRACE((_L("Setting")));
                if( aCmd[GetAtCommand(i).Length()] == '=' ||
                   aCmd[GetAtCommand(i).Length()] == '?' )
                   {
                    C_TRACE((_L("Ok"),i));
                   return ETrue;
                   }
                }
            }
        }
    C_TRACE((_L("return EFalse")));
    return EFalse; 
    }

void CModemATExt::HandleCommand( const TDesC8& aCmd, RBuf8& aReply, TBool aReplyNeeded )
    {
    C_TRACE((_L("CModemATExt::HandleCommand()" )));
    DUMP_MESSAGE(aCmd);

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

void CModemATExt::HandleCommandCancel()
    {
    C_TRACE((_L("CModemATExt::HandleCommandCancel()")));
    iRModemAt.HandleATCommandCancel();
    }

TInt CModemATExt::NextReplyPartLength()
    {
    C_TRACE((_L("CModemATExt::NextReplyPartLength()")));
    return Min( iReplyPtr.Length(), KPartLength);
    }

TInt CModemATExt::GetNextPartOfReply( RBuf8& aNextReply )
    {
    C_TRACE((_L("CModemATExt::GetNextPartOfReply()")));
    aNextReply.Create(iReplyPtr,KPartLength);
    iReplyPtr.Delete(0,KPartLength);
    return KErrNone;
    }

void CModemATExt::ReceiveUnsolicitedResult()
    {
    C_TRACE((_L("CModemATExt::ReceiveUnsolicitedResult()")));
    iRModemAt.ReceiveUnsolicitedResult(iUnsolicitedPtr);
    }

void CModemATExt::ReceiveUnsolicitedResultCancel()
    {
    C_TRACE((_L("CModemATExt::ReceiveUnsolicitedResultCancel")));
    iRModemAt.ReceiveUnsolicitedResultCancel();
    }

TBool CModemATExt::HandleEditorModeInput( const TDesC8& /*aInput*/,
                                                RBuf8& /*aReply*/,
                                                TBool& /*aEchoOn*/)
    {
    C_TRACE((_L("CModemATExt::HandleEditorModeInput()")));
    return EFalse;  //commands are execute commands, this function is newer called
    }

void CModemATExt::ReportNvramStatusChange( const TDesC8& /*aNvram*/ ) 
    {
    C_TRACE((_L("CModemATExt::ReportNvramStatusChange()")));
    //Ignored in this plugin
    }


CModemATExt::CModemATExt() : 
    iReplyBuffer(NULL), 
    iReplyPtr(0, 0), 
    iUnsolicitedPtr(0, 0), 
    iUnsolicitedBuffer(NULL),
    iReply(NULL),
    iReplyNeeded(EFalse)
    {
    C_TRACE((_L("CModemATExt::CModemATExt()")));
    }
     
void CModemATExt::ConstructL()
    {
    C_TRACE((_L("CModemATExt::ConstructL()")));
    iReplyBuffer = HBufC8::NewL( KDefaultCmdBufLength * KPackets ); 
    iReplyPtr.Set( iReplyBuffer->Des() );
    iUnsolicitedBuffer = HBufC8::NewL(KDefaultCmdBufLength);
    }

void CModemATExt::ReportConnectionName( const TDesC8& aName ) 
    {
    C_TRACE((_L("CModemATExt::ReportConnectionName()")));
    DUMP_MESSAGE((aName));
    iName.Copy(aName);
    iRModemAt.Connect(EATExtPlugin, iName, this);
    }

void CModemATExt::HandleATCommandCompleted( TInt aErr ) 
    {
    C_TRACE((_L("CModemATExt::HandleATCommandCompleted()")));
    
    if(iReplyNeeded) 
        {
        C_TRACE((_L("Reply needed")));
        iReply->Create( iReplyPtr,KPartLength );
        }
    
    iReplyPtr.Delete(0, KPartLength);
    C_TRACE((_L("Error status %d"),aErr));
    
    if( aErr == KErrNone) 
        {
        C_TRACE((_L("No errors")));
        HandleCommandCompleted(aErr, EReplyTypeOk);
        } 
    else 
        {
        C_TRACE((_L("Reply Error")));
        HandleCommandCompleted(aErr, EReplyTypeError);
        }
    }

void CModemATExt::HandleUnsolicitedResultReceived( TInt /*aErr*/ )
    {
    C_TRACE((_L("CModemATExt::HandleUnsolicitedResultReceived")));
    SendUnsolicitedResult( iUnsolicitedPtr );
    }

void CModemATExt::HandleSignalIndication( TInt /*aErr*/ )
    {
    C_TRACE((_L("CModemATExt::HandleSignalIndication")));
    //COMMON plugin handles signal indications
    }

void CModemATExt::ReportExternalHandleCommandError()
    {
    C_TRACE((_L("CModemATExt::ReportExternalHandleCommandError()")));
    //external error occurred, cancel AT-command
    iRModemAt.HandleATCommandCancel();
    }

void CModemATExt::HandleCommandModeChanged( TInt /*aErr*/, TCommandMode /*aMode*/ )
    {
    C_TRACE((_L("CModemATExt::HandleCommandModeChanged()")));
    //COMMON plugin handles mode change requests
    }
//  End of File  
