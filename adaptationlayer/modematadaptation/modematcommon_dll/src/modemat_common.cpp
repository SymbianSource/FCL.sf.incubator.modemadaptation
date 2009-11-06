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


#include "modemat_common.h"
#include "modemattrace.h"

_LIT8(KAtNvramStatusCmd, "at&v");
_LIT8(KEchoMode, "*E? *");
_LIT8(KVerboseMode, "* V? *");
_LIT8(KQuietMode, "* Q? *");
_LIT8(KCarriageChar, "*S03:??? *");
_LIT8(KLineFeedChar, "*S04:??? *");
_LIT8(KBackspaceChar, "*S05:??? *");
const TUint8 EchoModeOffset(1);
const TUint8 VerboseModeOffset(2);
const TUint8 QuietModeOffset(2);
const TUint8 CarriageCharOffset(4);
const TUint8 LineFeedCharOffset(4);
const TUint8 BackspaceCharOffset(4);

const TInt KNvramCommandLength(20);
const TInt KNvramBufLength(1024);

#define KDefaultBackSpace    8
#define KDefaultLineFeed     10
#define KDefaultCarriageChar 13
#define KDefaultEchoMode     1
#define KDefaultQuietMode    0
#define KDefaultVerboseMode  1

CModemAtCommon* CModemAtCommon::NewL()
    {
    C_TRACE((_L("CModemAtCommon::NewL")));
    CModemAtCommon* self = new (ELeave) CModemAtCommon();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    C_TRACE((_L("return 0x%x"), self));
    return self;
    }

CModemAtCommon::~CModemAtCommon()
    {
    C_TRACE((_L("CModemAtCommon::~CModemAtCommon")));
    iRModemAt.Disconnect();
    delete iNvramStatus;
    iNvramStatus = NULL;
    }

CModemAtCommon::CModemAtCommon() :
    iReceiveModeStatusChange( 0 ),
    iReceiveNvramStatusChange( 0 ),
    iEchoMode( KDefaultEchoMode ),
    iQuietMode( KDefaultQuietMode ),
    iVerboseMode( KDefaultVerboseMode ),
    iCarriageChar( KDefaultCarriageChar ),
    iLineFeedChar( KDefaultLineFeed ),
    iBackspaceChar( KDefaultBackSpace ),
    iCommandMode( EIgnore )
    {
    C_TRACE((_L("CModemAtCommon::CModemAtCommon")));
    }

void CModemAtCommon::ConstructL()
    {
    C_TRACE((_L("CModemAtCommon::ConstructL")));
    iNvramStatus = HBufC8::NewL( KNvramBufLength );
    C_TRACE((_L("KNvramBufLength : %d"), KNvramBufLength ));
    }

void CModemAtCommon::ReportConnectionName( const TDesC8& aName )
    {
    C_TRACE((_L("CModemAtCommon::ReportConnectionName()")));
    iMediaName = aName;
    DUMP_MESSAGE(aName);
    iRModemAt.Connect( ECommonPlugin, iMediaName, this );
    TBuf8<KNvramCommandLength> command;
    command.Copy(KAtNvramStatusCmd);
    command.Append(iCarriageChar);
    TPtr8 tempPtr( iNvramStatus->Des() );
#ifndef __WINSCW__
    iRModemAt.GetNvramStatus(command, tempPtr);
#else
    tempPtr.Copy(_L8("||ACTIVE PROFILE:||E1 Q0 V1 X5 &C1 &D2 &S0 &Y0||+CVHU=1 +DS=0,0,2048,32 +DR=0||S00:000 S01:000 S02:043 S03:013 S04:010 S05:008 S07:060 S08:002||S10:100 S12:050 S25:000||||OK||"));
#endif
    ParseNvramStatus();
    }

TInt CModemAtCommon::GetMode( TUint aMask, TUint& aMode )
    {
    C_TRACE((_L("CModemAtCommon::GetMode()")));
#ifdef _DEBUG
    PrintModes();
#endif    

    aMode = 0;
    if ( aMask & KModeEcho )
        {
        aMode |= iEchoMode * KEchoModeBase;
        C_TRACE((_L("echomode %d"), aMode));
        }
    if ( aMask & KModeQuiet )
        {
        aMode |= iQuietMode * KQuietModeBase;
        C_TRACE((_L("quietmode %d"), aMode));
        }
    if ( aMask & KModeVerbose )
        {
        aMode |= iQuietMode * KVerboseModeBase;
        C_TRACE((_L("verbosemode %d"), aMode));
        }
    if ( aMask & KModeCarriage )
        {
        aMode = iCarriageChar;
        C_TRACE((_L("Carriage return")));
        }
    if ( aMask & KModeLineFeed )
        {
        aMode = iLineFeedChar;
        C_TRACE((_L("line feed")));
        }
    if ( aMask & KModeBackspace )
        {
        C_TRACE((_L("backspace")));
        aMode = iBackspaceChar;
        }
    if( aMode )
        {
        C_TRACE((_L("return KErrNone")));
        return KErrNone;
        }
    C_TRACE((_L("return KErrNotSupported")));
    return KErrNotSupported;
    }

TInt CModemAtCommon::ReceiveModeStatusChange()
    {
    C_TRACE((_L("CModemAtCommon::ReceiveModeStatusChange()")));
    iReceiveModeStatusChange++;
    return KErrNone;
    }

void CModemAtCommon::CancelReceiveModeStatusChange()
    {
    C_TRACE((_L("CModemAtCommon::CancelReceiveModeStatusChange()")));
    if( iReceiveModeStatusChange > 0 )
        {
        iReceiveModeStatusChange--;
        }
    }

TInt CModemAtCommon::GetNvramStatus( RBuf8& aNvram )
    {
    C_TRACE((_L("CModemAtCommon::GetNvramStatus")));
    TInt retVal ( KErrNone );
    TPtr8 response( iNvramStatus->Des() );
    TBuf8<KNvramCommandLength> cmd;
    cmd.Copy(KAtNvramStatusCmd);
    cmd.Append(iCarriageChar);
    retVal = iRModemAt.GetNvramStatus( cmd, response );
    C_TRACE( (_L("CModemAtCommon::GetNvramStatus Creating response for atext")));
    aNvram.Create( response ); //atext deletes message
    return retVal;
    }

TInt CModemAtCommon::ReceiveNvramStatusChange()
    {
    C_TRACE((_L("CModemAtCommon::ReceiveNvramStatusChange()")));
    iReceiveNvramStatusChange++;
    return KErrNone;
    }

void CModemAtCommon::CancelReceiveNvramStatusChange()
    {
    C_TRACE((_L("CModemAtCommon::CancelReceiveNvramStatusChange()")));
    if( iReceiveNvramStatusChange > 0)
        {
        iReceiveNvramStatusChange--;
        }
    }

void CModemAtCommon::HandleATCommandCompleted( TInt /*aErr*/ )
    {
    C_TRACE((_L("CModemAtCommon::HandleATCommandCompleted()")));
    //not used by common plugin
    }


TBool CModemAtCommon::HasNvramStatusChanged()   
    {
    C_TRACE((_L("CModemAtCommon::HasNvramStatusChanged")));
    HBufC8* oldNvramBuf(NULL);
    TRAPD( err, oldNvramBuf = HBufC8::NewL( KNvramBufLength ) );
    ASSERT_PANIC_ALWAYS( err == KErrNone );
    oldNvramBuf->Des().Copy( iNvramStatus->Des() );
   
    C_TRACE((_L("Making AT&V query")));
    TBuf8<KNvramCommandLength> cmd;
    cmd.Copy( KAtNvramStatusCmd );
    cmd.Append( iCarriageChar );

    C_TRACE((_L("Old:")));
    TPtr8 oldNvramPtr( oldNvramBuf->Des() );
    DUMP_MESSAGE( oldNvramPtr );
    C_TRACE((_L("New:")));
    TPtr8 newNvramPtr( iNvramStatus->Des() );
    iRModemAt.GetNvramStatus( cmd, newNvramPtr );
    DUMP_MESSAGE((newNvramPtr));

    if( oldNvramPtr.Compare( newNvramPtr ) == KErrNone )
        {
        C_TRACE((_L("Nvram status NOT changed")));
        delete oldNvramBuf;
        return EFalse;
        }
    delete oldNvramBuf;
    C_TRACE((_L("Nvram status HAS changed")));
    return ETrue;        
    }

TUint CModemAtCommon::GetChangedMode()
    {
    C_TRACE((_L("CModemAtCommon::GetChangedMode()")));
    TUint newMode( 0 ); //mask for mode changes
    TUint8 oldEchoMode( iEchoMode );
    TUint8 oldQuietMode( iQuietMode );
    TUint8 oldVerboseMode( iVerboseMode );
    TUint8 oldCarriageChar( iCarriageChar );
    TUint8 oldLineFeedChar( iLineFeedChar );
    TUint8 oldBackspaceChar( iBackspaceChar );

    ParseNvramStatus();

    if( iReceiveModeStatusChange )
        {
        C_TRACE((_L("iReceiveModeStatusChange true")));
        if (oldEchoMode != iEchoMode) 
            {
            C_TRACE((_L("KEchoModeChanged")));
            newMode |= KEchoModeChanged;
            newMode |= iEchoMode*KModeEcho;
            }
        if (oldQuietMode != iQuietMode)
            {
            C_TRACE((_L("KQuietModeChanged")));
            newMode |= KQuietModeChanged;
            newMode |= iQuietMode*KModeQuiet;
            }
        if (oldVerboseMode != iVerboseMode)
            {
            C_TRACE((_L("KVerboseModeChanged")));
            newMode |= KVerboseModeChanged;
            newMode |= iVerboseMode*KModeVerbose ;
            }
        if (oldCarriageChar != iCarriageChar)
            {
            C_TRACE((_L("KCarriageChanged")));
            newMode |= KCarriageChanged;
            }
        if (oldLineFeedChar != iLineFeedChar)
            {
            C_TRACE((_L("KLineFeedChanged")));
            newMode |= KLineFeedChanged;
            }
        if (oldBackspaceChar != iBackspaceChar)
            {
            C_TRACE((_L("KBackspaceChanged")));
            newMode |= KBackspaceChanged;
            }
        }
    return newMode;
}

void CModemAtCommon::HandleSignalIndication( TInt aErr )
    {
    C_TRACE((_L("CModemAtCommon::HandleSignalIndication(%d)"), aErr));
    if( aErr != KErrNone )
        {
        C_TRACE((_L("SignalIndication Error %d"), aErr ));
        return;
        }

    TBool val = HasNvramStatusChanged();
    if( val == EFalse )
        {
		    C_TRACE((_L("HasNvramStatusChanged EFalse")));
        return;
        }
    //receive nvram status changes
    if( iReceiveNvramStatusChange )
        {
        // call base class
		    C_TRACE((_L("iReceiveNvramStatusChange true")));
        SendNvramStatusChange( iNvramStatus->Des() );
        iReceiveNvramStatusChange--;
        }

    TUint newMode = GetChangedMode();
    C_TRACE((_L("Newmode 0x%x"),newMode));
    if( newMode ) //mode is changed
        {
        C_TRACE((_L("Mode changed")));
        SendModeStatusChange( newMode );
        }
    }

void CModemAtCommon::ParseNvramStatus()
    {
    C_TRACE((_L("CModemAtCommon::ParseNvramStatus()")));
    TUint8 value;
    TInt retVal;
    retVal = ParseValue(KEchoMode, EchoModeOffset, value);
    if( retVal == KErrNone )
        {
        iEchoMode = value;
        }
    retVal = ParseValue( KVerboseMode, VerboseModeOffset, value);
    if( retVal == KErrNone )
        {
        iVerboseMode = value;
        }
    retVal = ParseValue(KQuietMode, QuietModeOffset, value);
    if( retVal == KErrNone )
        {
        iQuietMode = value;
        }
    retVal = ParseValue(KCarriageChar, CarriageCharOffset, value);
    if( retVal == KErrNone )
        {
        C_TRACE((_L("CModemAtCommon::ParseNvramStatus( ) iCarriageChar %d"), value));
        iCarriageChar = value;
        }
    retVal = ParseValue(KLineFeedChar, LineFeedCharOffset, value);
    if( retVal == KErrNone )
        {
        C_TRACE((_L("CModemAtCommon::ParseNvramStatus( ) iLineFeedChar %d"), value));
        iLineFeedChar = value;
        }
    retVal = ParseValue(KBackspaceChar, BackspaceCharOffset, value);
    if( retVal == KErrNone )
        {
        iBackspaceChar = value;
        }
#ifdef _DEBUG
    PrintModes();
#endif    
    }

TInt CModemAtCommon::ParseValue( const TDesC8& aReg, TInt aPos, TUint8& aValue)
    {
    C_TRACE((_L("CModemAtCommon::ParseValue( aPos: %d, aValue: 0x%x"), aPos, &aValue));
    DUMP_MESSAGE(aReg);

    TPtr8 tempPtr(iNvramStatus->Des());
    TInt start = tempPtr.Match(aReg);
    C_TRACE((_L("Starting %d length %d"), start, tempPtr.Length()));
    if ( start < 0 )
        {
        return start;
        }

    TInt firstValueChar = aReg.Locate('?');
    TInt valueChars = aReg.LocateReverse('?') - firstValueChar + 1;

    C_TRACE((_L("Firstvaluechar %d"), firstValueChar));
    C_TRACE((_L("Valuechars %d"), valueChars));

    if( firstValueChar < 0 || valueChars == 0 || (start + aPos + valueChars >= tempPtr.Length()) )
        {
        TRACE_ASSERT_ALWAYS;
        return KErrGeneral;
        }

    TBuf8<KNvramCommandLength> valueString;
    valueString = tempPtr.Mid( start + aPos, valueChars);
    DUMP_MESSAGE((valueString));

    TLex8 lex( valueString );
    TInt value;

    if( lex.Val(value) == KErrNone )
        {
        C_TRACE((_L("Value %d"), value));
        aValue = value;
        return KErrNone;
        }
    TRACE_ASSERT_ALWAYS;
    return KErrGeneral;
    }

#ifdef _DEBUG
void CModemAtCommon::PrintModes()
    {
    C_TRACE((_L("CModemAtCommon::PrintModes")));
    C_TRACE((_L("echo mode %d"), iEchoMode));
    C_TRACE((_L("quiet mode %d"), iQuietMode));
    C_TRACE((_L("verbose mode %d"), iVerboseMode));
    C_TRACE((_L("Carriagechar mode %d"), iCarriageChar));
    C_TRACE((_L("Linefeed char %d"), iLineFeedChar));
    C_TRACE((_L("Backspace mode %d"), iBackspaceChar));
    C_TRACE((_L("Command mode %d"), (TInt)iCommandMode));
    }
#endif

void CModemAtCommon::HandleUnsolicitedResultReceived(TInt /*aErr*/)
    {
    C_TRACE((_L("CModemAtCommon::HandleUnsolicitedResultReceived - ignored")));
    //unsolicitedresult is handled in atext-plugin
    }

void CModemAtCommon::HandleCommandModeChanged( TInt aErr, TCommandMode aMode )
    {
    C_TRACE((_L("CModemAtCommon::HandleCommandModeChanged( aErr: %d, aMode: %d) "), aErr, aMode));
    ASSERT_PANIC_ALWAYS( aErr == KErrNone );
    ASSERT_PANIC_ALWAYS( aMode != EIgnore );

   iCommandMode = aMode;
   TUint mode( 0 );
   mode |= KCommandModeChanged;
   if( aMode != EDataMode)
       {
       C_TRACE((_L("Not EDataMode")));
       mode |= KModeCommand;
       }
   C_TRACE((_L("COMMON PLUGIN SendModeStatusChange: 0x%x"), mode));
   SendModeStatusChange( mode );
   }


//  End of File
