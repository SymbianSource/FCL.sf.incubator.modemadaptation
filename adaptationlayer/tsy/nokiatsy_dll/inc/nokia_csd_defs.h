/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef NOKIA_CSD_DEFS_H
#define NOKIA_CSD_DEFS_H

// INCLUDES
#include <etelmm.h>
#include <e32std.h>

// CONSTANTS
#if defined (__WINS__)
    _LIT( KPddName, "ECDRV" );
    _LIT( KLddName, "ECOMM" );
#else
    _LIT( KPddName, "EUART1" );
    _LIT( KLddName, "ECOMM" );
#endif // (__WINS__)

    // Dataport names
    _LIT( KDataPortPort0, "DATAPORT::0" );
    _LIT( KDataPortPort1, "DATAPORT::1" );
    _LIT( KDataPortPort4, "DATAPORT::4" );
    _LIT( KDataPortPort5, "DATAPORT::5" );
    _LIT( KDataPortPortDUN, "DATAPORT::DUN" );
    _LIT( KDataPortName, "DATAPORT");

    // Receive buffer length for dataport
    const TInt KDPReceiveBufLen = 5760;

    // At command buffer limit.
    const TInt KSentAtCommandLimit = 10;

    // Buffer lenght used in data calls
    const TInt KDataBufLength = 50;

    // HSCSD parameter codings default value
    const TInt KHSCSDCodingsDefault = 8;

    // HSCSD parameter codings all supported -value
    const TInt KHSCSDCodingsAllSupport = 0;

    // Common string constants for AT commands
    _LIT8( KReturn, "\r" );
    _LIT8( KLinefeed, "\n" ); // Used for dataports linefeed scanning (length=1)

    // Constants for basic responses
    _LIT8( KModemOk, "OK*" );
    _LIT8( KModemError, "ERROR*" );
    _LIT8( KModemNoDialTone, "NO DIALTONE*" );
    _LIT8( KModemDelayed, "DELAYED*" );
    _LIT8( KModemBlacklisted, "BLACKLISTED*" );
    _LIT8( KModemBusy, "BUSY*" );
    _LIT8( KModemNoAnswer, "NO ANSWER*" );
    _LIT8( KModemNoCarrier, "NO CARRIER*" );

    // Constants for +CRING responses
    _LIT8( KModemCRing, "+CRING:*" );
    _LIT8( KModemCRingAsync, "+CRING: ASYNC*" );
    _LIT8( KModemCRingSync, "+CRING: SYNC*" );
    _LIT8( KModemCRingRelAsync, "+CRING: REL ASYNC*" );
    _LIT8( KModemCRingRelSync, "+CRING: REL SYNC*" );
    _LIT8( KModemCRingFax, "+CRING: FAX*" );
    _LIT8( KModemCRingVoice, "+CRING: VOICE*" );
    _LIT8( KModemCRingVoiceAsync, "+CRING: VOICE/ASYNC*" );
    _LIT8( KModemCRingVoiceSync, "+CRING: VOICE/SYNC*" );
    _LIT8( KModemCRingVoiceRelAsync, "+CRING: VOICE/REL ASYNC*" );
    _LIT8( KModemCRingVoiceRelSync, "+CRING: VOICE/REL SYNC*" );
    _LIT8( KModemCRingAltVoiceAsync, "+CRING: ALT VOICE/ASYNC*" );
    _LIT8( KModemCRingAltVoiceSync, "+CRING: ALT VOICE/SYNC*" );
    _LIT8( KModemCRingAltVoiceRelAsync, "+CRING: ALT VOICE/REL ASYNC*" );
    _LIT8( KModemCRingAltVoiceRelSync, "+CRING: ALT VOICE/REL SYNC*" );
    _LIT8( KModemCRingAltAsyncVoice, "+CRING: ALT ASYNC/VOICE*" );
    _LIT8( KModemCRingAltSyncVoice, "+CRING: ALT SYNC/VOICE*" );
    _LIT8( KModemCRingAltRelAsyncVoice, "+CRING: ALT REL ASYNC/VOICE*" );
    _LIT8( KModemCRingAltRelSyncVoice, "+CRING: ALT REL SYNC/VOICE*" );
    _LIT8( KModemCRingAltVoiceFax, "+CRING: ALT VOICE/FAX*" );
    _LIT8( KModemCRingAltFaxVoice, "+CRING: ALT FAX/VOICE*" );

    // Constants for CONNECT responses
    _LIT8( KModemConnect, "CONNECT*" );
    _LIT8( KModemConnect300, "*300*" );
    _LIT8( KModemConnect1200, "*1200*" );
    _LIT8( KModemConnect2400, "*2400*" );
    _LIT8( KModemConnect4800, "*4800*" );
    _LIT8( KModemConnect7200, "*7200*" );
    _LIT8( KModemConnect9600, "*9600*" );
    _LIT8( KModemConnect12000, "*12000*" );
    _LIT8( KModemConnect14400, "*14400*" );
    _LIT8( KModemConnect19200, "*19200*" );
    _LIT8( KModemConnect28800, "*28800*" );
    _LIT8( KModemConnect31200, "*31200*" );
    _LIT8( KModemConnect32000, "*32000*" );
    _LIT8( KModemConnect33600, "*33600*" );
    _LIT8( KModemConnect38400, "*38400*" );
    _LIT8( KModemConnect43200, "*43200*" );
    _LIT8( KModemConnect48000, "*48000*" );
    _LIT8( KModemConnect56000, "*56000*" );
    _LIT8( KModemConnect57600, "*57600*" );
    _LIT8( KModemConnect64000, "*64000*" );

    // Constants for COMPRESSION responses
    _LIT8( KModemCompression, "COMPRESSION*" );
    _LIT8( KModemCompressionV42Bis, "COMPRESSION V42BIS*" );
    _LIT8( KModemCompressionClass5, "COMPRESSION CLASS5*" );
    _LIT8( KModemCompressionNone, "COMPRESSION NONE*" );
    _LIT8( KModemCompressionUnknown, "COMPRESSION UNKNOWN*" );

    // Constants for PROTOCOL responses
    _LIT8( KModemProtocol, "PROTOCOL*" );
    _LIT8( KModemProtocolLapm, "PROTOCOL LAPM*" );
    _LIT8( KModemProtocolCellular, "PROTOCOL ALT CELLULAR*" );
    _LIT8( KModemProtocolAlt, "PROTOCOL ALT*" );
    _LIT8( KModemProtocolNone, "PROTOCOL NONE*" );
    _LIT8( KModemProtocolUnknown, "PROTOCOL UNKNOWN*" );

    // Constatnts for baud rates
    const TUint KBps2400 = 2400;
    const TUint KBps4800 = 4800;
    const TUint KBps9600 = 9600;
    const TUint KBps14400 = 14400;
    const TUint KBps19200 = 19200;
    const TUint KBps28800 = 28800;
    const TUint KBps32000 = 32000;
    const TUint KBps33600 = 33600;
    const TUint KBps38400 = 38400;
    const TUint KBps43200 = 43200;
    const TUint KBps48000 = 48000;
    const TUint KBps56000 = 56000;
    const TUint KBps57600 = 57600;
    const TUint KBps64000 = 64000;

// MACROS
    //none

// DATA TYPES
    // Call types
    enum TCallType
        {
        ENormalVoice,
        ENormalData,
        ENormalHscsdData,
        ENormalFax,
        EVoiceFollowedByData,
        EAltVoiceData,
        EAltDataVoice,
        EAltVoiceFax,
        EAltFaxVoice,
        ECallTypeUnknown
        };

    // Communication server connection status
    enum TCommsStatus
        {
        ECommsNotConnected,
        ECommsServerConnected,
        ECommsModuleLoaded,
        ECommsDataportOpened,
        ECommsDataPortUsedByLine,
        ECommsDataPortUsedByCall
        };

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
    //none

// CLASS DECLARATION
    //none

#endif // NOKIA_CSD_DEFS_H

// End of File
