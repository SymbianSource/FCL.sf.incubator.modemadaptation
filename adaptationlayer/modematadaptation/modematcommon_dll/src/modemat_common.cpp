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
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "modemat_commonTraces.h"
#endif


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
const TInt KReplyTypeOther( 3 );

const TInt KEchoOn( 1 );
const TInt KEchoOff( 2 );
const TInt KVerboseOn( 3 );
const TInt KVerboseOff( 4 );
const TInt KQuietOn( 5 );
const TInt KQuietOff( 6 );

#define KDefaultBackSpace    8
#define KDefaultLineFeed     10
#define KDefaultCarriageChar 13
#define KDefaultEchoMode     1
#define KDefaultQuietMode    0
#define KDefaultVerboseMode  1
#define KValueOn 1
#define KValueOff 0

CModemAtCommon* CModemAtCommon::NewL()
    {
    C_TRACE((_L("CModemAtCommon::NewL")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_NEWL, "CModemAtCommon::NewL" );
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
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_CMODEMATCOMMON, "CModemAtCommon::~CModemAtCommon" );
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
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATCOMMON_CMODEMATCOMMON, "CModemAtCommon::CModemAtCommon" );
    }

void CModemAtCommon::ConstructL()
    {
    C_TRACE((_L("CModemAtCommon::ConstructL")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_CONSTRUCTL, "CModemAtCommon::ConstructL" );
    iNvramStatus = HBufC8::NewL( KNvramBufLength );
    C_TRACE((_L("KNvramBufLength : %d"), KNvramBufLength ));
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATCOMMON_CONSTRUCTL, "CModemAtCommon::ConstructL;KNvramBufLength=%d", KNvramBufLength );
    }

void CModemAtCommon::ReportConnectionName( const TDesC8& aName )
    {
    C_TRACE((_L("CModemAtCommon::ReportConnectionName()")));
    OstTraceExt1( TRACE_NORMAL, CMODEMATCOMMON_REPORTCONNECTIONNAME, "CModemAtCommon::ReportConnectionName;aName=%s", aName );
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
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_GETMODE, "CModemAtCommon::GetMode" );
#ifdef _DEBUG
    PrintModes();
#endif    

    aMode = 0;
    if ( aMask & KModeEcho )
        {
        aMode |= iEchoMode * KEchoModeBase;
        C_TRACE((_L("echomode %d"), aMode));
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATCOMMON_GETMODE, "echomode;aMode=%u", aMode );
        }
    if ( aMask & KModeQuiet )
        {
        aMode |= iQuietMode * KQuietModeBase;
        C_TRACE((_L("quietmode %d"), aMode));
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATCOMMON_GETMODE, "quietmode;aMode=%u", aMode );
        }
    if ( aMask & KModeVerbose )
        {
        aMode |= iVerboseMode * KVerboseModeBase;
        C_TRACE((_L("verbosemode %d"), aMode));
        OstTrace1( TRACE_NORMAL, DUP3_CMODEMATCOMMON_GETMODE, "verbosemode;aMode=%u", aMode );
        }
    if ( aMask & KModeCarriage )
        {
        aMode = iCarriageChar;
        C_TRACE((_L("Carriage return")));
        OstTrace0( TRACE_NORMAL, DUP4_CMODEMATCOMMON_GETMODE, "carriage return" );
        }
    if ( aMask & KModeLineFeed )
        {
        aMode = iLineFeedChar;
        C_TRACE((_L("line feed")));
        OstTrace0( TRACE_NORMAL, DUP5_CMODEMATCOMMON_GETMODE, "line feed" );
        }
    if ( aMask & KModeBackspace )
        {
        C_TRACE((_L("backspace")));
        OstTrace0( TRACE_NORMAL, DUP6_CMODEMATCOMMON_GETMODE, "backspace" );
        aMode = iBackspaceChar;
        }
    if( aMode )
        {
        C_TRACE((_L("return KErrNone")));
        OstTrace0( TRACE_NORMAL, DUP7_CMODEMATCOMMON_GETMODE, "return KErrNone" );
        return KErrNone;
        }
    C_TRACE((_L("return KErrNotSupported")));
    OstTrace0( TRACE_NORMAL, DUP8_CMODEMATCOMMON_GETMODE, "return KErrNotSupported" );
    return KErrNotSupported;
    }

TInt CModemAtCommon::ReceiveModeStatusChange()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_RECEIVEMODESTATUSCHANGE, "CModemAtCommon::ReceiveModeStatusChange" );
    C_TRACE((_L("CModemAtCommon::ReceiveModeStatusChange()")));
    iReceiveModeStatusChange++;
    return KErrNone;
    }

void CModemAtCommon::CancelReceiveModeStatusChange()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_CANCELRECEIVEMODESTATUSCHANGE, "CModemAtCommon::CancelReceiveModeStatusChange" );
    C_TRACE((_L("CModemAtCommon::CancelReceiveModeStatusChange()")));
    if( iReceiveModeStatusChange > 0 )
        {
        iReceiveModeStatusChange--;
        }
    }

TInt CModemAtCommon::GetNvramStatus( RBuf8& aNvram )
    {
    C_TRACE((_L("CModemAtCommon::GetNvramStatus")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_GETNVRAMSTATUS, "CModemAtCommon::GetNvramStatus" );
    TInt retVal ( KErrNone );
    TPtr8 response( iNvramStatus->Des() );
    TBuf8<KNvramCommandLength> cmd;
    cmd.Copy(KAtNvramStatusCmd);
    cmd.Append(iCarriageChar);
    retVal = iRModemAt.GetNvramStatus( cmd, response );
    C_TRACE( (_L("CModemAtCommon::GetNvramStatus Creating response for atext")));
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATCOMMON_GETNVRAMSTATUS, "CModemAtCommon::GetNvramStatus Creating response for atext" );
    aNvram.Create( response ); //atext deletes message
    return retVal;
    }

TInt CModemAtCommon::ReceiveNvramStatusChange()
    {
    C_TRACE((_L("CModemAtCommon::ReceiveNvramStatusChange()")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_RECEIVENVRAMSTATUSCHANGE, "CModemAtCommon::ReceiveNvramStatusChange" );
    iReceiveNvramStatusChange++;
    return KErrNone;
    }

void CModemAtCommon::CancelReceiveNvramStatusChange()
    {
    C_TRACE((_L("CModemAtCommon::CancelReceiveNvramStatusChange()")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_CANCELRECEIVENVRAMSTATUSCHANGE, "CModemAtCommon::CancelReceiveNvramStatusChange" );
    if( iReceiveNvramStatusChange > 0)
        {
        iReceiveNvramStatusChange--;
        }
    }

void CModemAtCommon::HandleATCommandCompleted( TInt /*aErr*/ )
    {
    C_TRACE((_L("CModemAtCommon::HandleATCommandCompleted()")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_HANDLEATCOMMANDCOMPLETED, "CModemAtCommon::HandleATCommandCompleted" );
    //not used by common plugin
    }


TBool CModemAtCommon::HasNvramStatusChanged()   
    {
    C_TRACE((_L("CModemAtCommon::HasNvramStatusChanged")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_HASNVRAMSTATUSCHANGED, "CModemAtCommon::HasNvramStatusChanged" );
    HBufC8* oldNvramBuf(NULL);
    TRAPD( err, oldNvramBuf = HBufC8::NewL( KNvramBufLength ) );
    ASSERT_PANIC_ALWAYS( err == KErrNone );
    oldNvramBuf->Des().Copy( iNvramStatus->Des() );
   
    C_TRACE((_L("Making AT&V query")));
    OstTrace0( TRACE_NORMAL, DUP1_CMODEMATCOMMON_HASNVRAMSTATUSCHANGED, "Making AT&V query" );
    TBuf8<KNvramCommandLength> cmd;
    cmd.Copy( KAtNvramStatusCmd );
    cmd.Append( iCarriageChar );

    C_TRACE((_L("Old:")));
    TPtr8 oldNvramPtr( oldNvramBuf->Des() );
    DUMP_MESSAGE( oldNvramPtr );
    OstTraceExt1( TRACE_NORMAL, DUP2_CMODEMATCOMMON_HASNVRAMSTATUSCHANGED, "Old:;oldNvramPtr=%s", oldNvramPtr );

    TPtr8 newNvramPtr( iNvramStatus->Des() );
    TInt nvramStatus = iRModemAt.GetNvramStatus( cmd, newNvramPtr );
    if( nvramStatus != KErrNone && 
    	  nvramStatus != KReplyTypeOther )
        {
        // at&v can't be handled in data mode. Then this is ignored.
        C_TRACE((_L("Nvram status NOT changed, pipe is in data mode")));
        OstTrace0( TRACE_NORMAL, DUP3_CMODEMATCOMMON_HASNVRAMSTATUSCHANGED, "Nvram status NOT changed, pipe is in data mode" );
        delete oldNvramBuf;
        return EFalse;
        }
    C_TRACE((_L("New:")));
    DUMP_MESSAGE((newNvramPtr));
    OstTraceExt1( TRACE_NORMAL, DUP4_CMODEMATCOMMON_HASNVRAMSTATUSCHANGED, "New:;newNvramPtr=%s", newNvramPtr );

    if( oldNvramPtr.Compare( newNvramPtr ) == KErrNone )
        {
        C_TRACE((_L("Nvram status NOT changed")));
        OstTrace0( TRACE_NORMAL, DUP5_CMODEMATCOMMON_HASNVRAMSTATUSCHANGED, "Nvram status NOT changed" );
        delete oldNvramBuf;
        return EFalse;
        }
    delete oldNvramBuf;
    C_TRACE((_L("Nvram status HAS changed")));
    OstTrace0( TRACE_NORMAL, DUP6_CMODEMATCOMMON_HASNVRAMSTATUSCHANGED, "Nvram status HAS changed" );
    return ETrue;        
    }

TUint CModemAtCommon::GetChangedMode()
    {
    C_TRACE((_L("CModemAtCommon::GetChangedMode()")));
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_GETCHANGEDMODE, "CModemAtCommon::GetChangedMode" );
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
        OstTrace0( TRACE_NORMAL, DUP1_CMODEMATCOMMON_GETCHANGEDMODE, "iReceiveModeStatusChange true" );
        if (oldEchoMode != iEchoMode) 
            {
            OstTrace0( TRACE_NORMAL, DUP2_CMODEMATCOMMON_GETCHANGEDMODE, "KEchoModeChanged" );
            C_TRACE((_L("KEchoModeChanged")));
            newMode |= KEchoModeChanged;
            newMode |= iEchoMode*KModeEcho;
            }
        if (oldQuietMode != iQuietMode)
            {
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATCOMMON_GETCHANGEDMODE, "KQuietModeChanged" );
            C_TRACE((_L("KQuietModeChanged")));
            newMode |= KQuietModeChanged;
            newMode |= iQuietMode*KModeQuiet;
            }
        if (oldVerboseMode != iVerboseMode)
            {
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATCOMMON_GETCHANGEDMODE, "KVerboseModeChanged" );
            C_TRACE((_L("KVerboseModeChanged")));
            newMode |= KVerboseModeChanged;
            newMode |= iVerboseMode*KModeVerbose;
            }
        if (oldCarriageChar != iCarriageChar)
            {
            OstTrace0( TRACE_NORMAL, DUP5_CMODEMATCOMMON_GETCHANGEDMODE, "KCarriageChanged" );
            C_TRACE((_L("KCarriageChanged")));
            newMode |= KCarriageChanged;
            newMode |= iCarriageChar;
            }
        if (oldLineFeedChar != iLineFeedChar)
            {
            OstTrace0( TRACE_NORMAL, DUP6_CMODEMATCOMMON_GETCHANGEDMODE, "KLineFeedChanged" );
            C_TRACE((_L("KLineFeedChanged")));
            newMode |= KLineFeedChanged;
            newMode |= iLineFeedChar;
            }
        if (oldBackspaceChar != iBackspaceChar)
            {
            OstTrace0( TRACE_NORMAL, DUP7_CMODEMATCOMMON_GETCHANGEDMODE, "KBackspaceChanged" );
            C_TRACE((_L("KBackspaceChanged")));
            newMode |= KBackspaceChanged;
            newMode |= iBackspaceChar;
            }
        }
    return newMode;
    }

void CModemAtCommon::HandleSignalIndication( TInt aErr )
    {
    OstTrace1( TRACE_NORMAL, CMODEMATCOMMON_HANDLESIGNALINDICATION, "CModemAtCommon::HandleSignalIndication;aErr=%d", aErr );
    C_TRACE((_L("CModemAtCommon::HandleSignalIndication(%d)"), aErr));
    if( aErr < KErrNone )
        {
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATCOMMON_HANDLESIGNALINDICATION, "SignalIndication Error;aErr=%d", aErr );
        C_TRACE((_L("SignalIndication Error %d"), aErr ));
        return;
        }
    else if( aErr > KErrNone )
        {
        TBool indicationHandled( EFalse );
        TUint newMode( 0 );
        TUint8 oldEchoMode( iEchoMode );
        TUint8 oldQuietMode( iQuietMode );
        TUint8 oldVerboseMode( iVerboseMode );

        switch( aErr )
            {
            case KEchoOff:
                {
                iEchoMode = KValueOff;
                indicationHandled = ETrue;
                break;
                }
            case KEchoOn:
                {
                iEchoMode = KValueOn;
                indicationHandled = ETrue;
                break;
                }
            case KVerboseOn:
                {
                iVerboseMode = KValueOn;
                indicationHandled = ETrue;
                break;
                }
            case KVerboseOff:
                {
                iVerboseMode = KValueOff;
                indicationHandled = ETrue;
                break;
                }
            case KQuietOn:
                {
                iQuietMode = KValueOn;
                indicationHandled = ETrue;
                break;
                }
            case KQuietOff:
                {
                iQuietMode = KValueOff;
                indicationHandled = ETrue;
                break;
                }
            default:
                {
                OstTrace1( TRACE_NORMAL, DUP2_CMODEMATCOMMON_HANDLESIGNALINDICATION, "SignalIndication Error;aErr=%d", aErr );
                C_TRACE((_L("SignalIndication Error %d"), aErr ));
                indicationHandled = EFalse;
                break;
                }
            }

        if( oldEchoMode != iEchoMode )
            {
            OstTrace0( TRACE_NORMAL, DUP3_CMODEMATCOMMON_HANDLESIGNALINDICATION, "SignalIndication KEchoModeChanged" );
            C_TRACE((_L("SignalIndication KEchoModeChanged")));
            newMode |= KEchoModeChanged;
            newMode |= iEchoMode * KModeEcho;
            }
        if( oldQuietMode != iQuietMode ) 
            {
            OstTrace0( TRACE_NORMAL, DUP4_CMODEMATCOMMON_HANDLESIGNALINDICATION, "SignalIndication KModeQuietChanged" );
            C_TRACE((_L("SignalIndication KQuietModeChanged")));
            newMode |= KQuietModeChanged;
            newMode |= iQuietMode * KModeQuiet;
            }
        if( oldVerboseMode != iVerboseMode ) 
            {
            OstTrace0( TRACE_NORMAL, DUP5_CMODEMATCOMMON_HANDLESIGNALINDICATION, "SignalIndication KVerboseModeChanged" );
            C_TRACE((_L("SignalIndication KVerboseModeChanged")));
            newMode |= KVerboseModeChanged;
            newMode |= iVerboseMode * KModeVerbose;
            }

        if( indicationHandled )
            {
            OstTrace0( TRACE_NORMAL, DUP6_CMODEMATCOMMON_HANDLESIGNALINDICATION, "Indication handled by common plug-in." );
            C_TRACE((_L("Indication handled by common plug-in.")));
            if( newMode )
                {
                OstTrace0( TRACE_NORMAL, DUP7_CMODEMATCOMMON_HANDLESIGNALINDICATION, "Mode changed" );
                C_TRACE((_L("Mode changed")));
                SendModeStatusChange( newMode );
                }
            return;
            }
        }
    
    OstTrace0( TRACE_NORMAL, DUP8_CMODEMATCOMMON_HANDLESIGNALINDICATION, "Indication is handled by common plug-in, by using AT&V query." );
    C_TRACE((_L("Indication is handled by common plug-in, by using AT&V query.")));
    
    TBool val = HasNvramStatusChanged();
    if( val == EFalse )
        {
		OstTrace0( TRACE_NORMAL, DUP9_CMODEMATCOMMON_HANDLESIGNALINDICATION, "HasNvramStatusChanged EFalse" );
        C_TRACE((_L("HasNvramStatusChanged EFalse")));
        return;
        }
    //receive nvram status changes
    if( iReceiveNvramStatusChange )
        {
        // call base class
		OstTrace0( TRACE_NORMAL, DUP10_CMODEMATCOMMON_HANDLESIGNALINDICATION, "iReceiveNvramStatusChange true" );
        C_TRACE((_L("iReceiveNvramStatusChange true")));
        SendNvramStatusChange( iNvramStatus->Des() );
        iReceiveNvramStatusChange--;
        }

    TUint newMode = GetChangedMode();
    OstTrace1( TRACE_NORMAL, DUP11_CMODEMATCOMMON_HANDLESIGNALINDICATION, "newMode;newMode=%x", newMode );
    C_TRACE((_L("Newmode 0x%x"),newMode));
    if( newMode ) //mode is changed
        {
        OstTrace0( TRACE_NORMAL, DUP12_CMODEMATCOMMON_HANDLESIGNALINDICATION, "Mode changed" );
        C_TRACE((_L("Mode changed")));
        SendModeStatusChange( newMode );
        }
    }

void CModemAtCommon::ParseNvramStatus()
    {
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_PARSENVRAMSTATUS, "CModemAtCommon::ParseNvramStatus" );
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
        OstTrace1( TRACE_NORMAL, DUP1_CMODEMATCOMMON_PARSENVRAMSTATUS, "CModemAtCommon::ParseNvramStatus - iCarriageChar;value=%d", value );
        C_TRACE((_L("CModemAtCommon::ParseNvramStatus( ) iCarriageChar %d"), value));
        iCarriageChar = value;
        }
    retVal = ParseValue(KLineFeedChar, LineFeedCharOffset, value);
    if( retVal == KErrNone )
        {
        OstTrace1( TRACE_NORMAL, DUP2_CMODEMATCOMMON_PARSENVRAMSTATUS, "CModemAtCommon::ParseNvramStatus - iLineFeedChar;value=%d", value );
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
    OstTraceExt3( TRACE_NORMAL, CMODEMATCOMMON_PARSEVALUE, "CModemAtCommon::ParseValue;aReg=%s;aPos=%d;aValue=%p", aReg, aPos, &aValue );
    C_TRACE((_L("CModemAtCommon::ParseValue( aPos: %d, aValue: 0x%x"), aPos, &aValue));
    DUMP_MESSAGE(aReg);

    TPtr8 tempPtr(iNvramStatus->Des());
    TInt start = tempPtr.Match(aReg);
    OstTraceExt2( TRACE_NORMAL, DUP1_CMODEMATCOMMON_PARSEVALUE, "Starting;start=%d;length=%d", start, tempPtr.Length() );
    C_TRACE((_L("Starting %d length %d"), start, tempPtr.Length()));
    if ( start < 0 )
        {
        return start;
        }

    TInt firstValueChar = aReg.Locate('?');
    TInt valueChars = aReg.LocateReverse('?') - firstValueChar + 1;

    OstTrace1( TRACE_NORMAL, DUP2_CMODEMATCOMMON_PARSEVALUE, "Firstvaluechar;firstValueChar=%d", firstValueChar );
    OstTrace1( TRACE_NORMAL, DUP3_CMODEMATCOMMON_PARSEVALUE, "Valuechars;Valuechars=%d", valueChars );
    C_TRACE((_L("Firstvaluechar %d"), firstValueChar));
    C_TRACE((_L("Valuechars %d"), valueChars));

    if( firstValueChar < 0 || valueChars == 0 || (start + aPos + valueChars >= tempPtr.Length()) )
        {
        TRACE_ASSERT_ALWAYS;
        return KErrGeneral;
        }

    TBuf8<KNvramCommandLength> valueString;
    valueString = tempPtr.Mid( start + aPos, valueChars);
    OstTraceExt1( TRACE_NORMAL, DUP4_CMODEMATCOMMON_PARSEVALUE, "valueString dump;valueString=%s", valueString );
    DUMP_MESSAGE((valueString));

    TLex8 lex( valueString );
    TInt value;

    if( lex.Val(value) == KErrNone )
        {
        OstTrace1( TRACE_NORMAL, DUP5_CMODEMATCOMMON_PARSEVALUE, "Value;value=%d", value );
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
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_PRINTMODES, "CModemAtCommon::PrintModes" );
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATCOMMON_PRINTMODES, "echo mode;iEchoMode=%d", iEchoMode );
    OstTrace1( TRACE_NORMAL, DUP2_CMODEMATCOMMON_PRINTMODES, "quiet mode;iQuietMode=%d", iQuietMode );
    OstTrace1( TRACE_NORMAL, DUP3_CMODEMATCOMMON_PRINTMODES, "verbose mode;iVerboseMode=%d", iVerboseMode );
    OstTrace1( TRACE_NORMAL, DUP4_CMODEMATCOMMON_PRINTMODES, "Carriagechar mode;iCarriageChar=%d", iCarriageChar );
    OstTrace1( TRACE_NORMAL, DUP5_CMODEMATCOMMON_PRINTMODES, "Linefeed char;iLineFeedChar=%d", iLineFeedChar );
    OstTrace1( TRACE_NORMAL, DUP7_CMODEMATCOMMON_PRINTMODES, "Backspace mode;iBackspaceChar=%d", iBackspaceChar );
    OstTrace1( TRACE_NORMAL, DUP8_CMODEMATCOMMON_PRINTMODES, "Command mode;iCommandMode=%d", (TInt)iCommandMode );
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
    OstTrace0( TRACE_NORMAL, CMODEMATCOMMON_HANDLEUNSOLICITEDRESULTRECEIVED, "CModemAtCommon::HandleUnsolicitedResultReceived - ignored" );
    C_TRACE((_L("CModemAtCommon::HandleUnsolicitedResultReceived - ignored")));
    //unsolicitedresult is handled in atext-plugin
    }

void CModemAtCommon::HandleCommandModeChanged( TInt aErr, TCommandMode aMode )
    {
    OstTraceExt2( TRACE_NORMAL, CMODEMATCOMMON_HANDLECOMMANDMODECHANGED, "CModemAtCommon::HandleCommandModeChanged;aErr=%d;aMode=%d", aErr, aMode );
    C_TRACE((_L("CModemAtCommon::HandleCommandModeChanged( aErr: %d, aMode: %d) "), aErr, aMode));
    iCommandMode = aMode;
    TUint mode( 0 );
    mode |= KCommandModeChanged;
    if( aMode != EDataMode)
        {
        C_TRACE((_L("Not EDataMode")));
        mode |= KModeCommand;
        }
    OstTrace1( TRACE_NORMAL, DUP1_CMODEMATCOMMON_HANDLECOMMANDMODECHANGED, "COMMON PLUGIN SendModeStatusChange:;mode=%x", mode );
    C_TRACE((_L("COMMON PLUGIN SendModeStatusChange: 0x%x"), mode));
    SendModeStatusChange( mode );
    }


//  End of File
