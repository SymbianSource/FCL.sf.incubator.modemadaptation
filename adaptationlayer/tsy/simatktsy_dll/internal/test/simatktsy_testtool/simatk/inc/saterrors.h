/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Header contain Error codes and their descriptions.
*
*/



#ifndef SATERRORS_H
#define SATERRORS_H

// CONSTANTS
struct TSatError
    {
    const TInt iErrorCode;
    const TText* iErrorString;
    };
    
TSatError const KSatErrorList[] = {
    { KErrNone,                     _S( "KErrNone" ) },
    { KErrNotFound,                 _S( "KErrNotFound" ) },
    { KErrGeneral,                  _S( "KErrGeneral" ) },
    { KErrCancel,                   _S( "KErrCancel" ) },
    { KErrNotSupported,             _S( "KErrNotSupported" ) },
    { KErrArgument,                 _S( "KErrArgument" ) },
    { KErrTotalLossOfPrecision,     _S( "KErrTotalLossOfPrecision" ) },
    { KErrBadHandle,                _S( "KErrBadHandle" ) },
    { KErrOverflow,                 _S( "KErrOverflow" ) },
    { KErrUnderflow,                _S( "KErrUnderflow" ) },
    { KErrAlreadyExists,            _S( "KErrAlreadyExists" ) },
    { KErrPathNotFound,             _S( "KErrPathNotFound" ) },
    { KErrDied,                     _S( "KErrDied" ) },
    { KErrInUse,                    _S( "KErrInUse" ) },
    { KErrServerTerminated,         _S( "KErrServerTerminated" ) },
    { KErrServerBusy,               _S( "KErrServerBusy" ) },
    { KErrCompletion,               _S( "KErrCompletion" ) },
    { KErrUnknown,                  _S( "KErrUnknown" ) },
    { KErrCorrupt,                  _S( "KErrCorrupt" ) },
    { KErrAccessDenied,             _S( "KErrAccessDenied" ) },
    { KErrLocked,                   _S( "KErrLocked" ) },
    { KErrWrite,                    _S( "KErrWrite" ) },
    { KErrDisMounted,               _S( "KErrDisMounted" ) },
    { KErrEof,                      _S( "KErrEof" ) },
    { KErrDiskFull,                 _S( "KErrDiskFull" ) },
    { KErrBadDriver,                _S( "KErrBadDriver" ) },
    { KErrBadName,                  _S( "KErrBadName" ) },
    { KErrCommsLineFail,            _S( "KErrCommsLineFail" ) },
    { KErrCommsFrame,               _S( "KErrCommsFrame" ) },
    { KErrCommsOverrun,             _S( "KErrCommsOverrun" ) },
    { KErrTimedOut,                 _S( "KErrTimedOut" ) },
    { KErrCouldNotConnect,          _S( "KErrCouldNotConnect" ) },
    { KErrCouldNotDisconnect,       _S( "KErrCouldNotDisconnect" ) },
    { KErrDisconnected,             _S( "KErrDisconnected" ) },
    { KErrBadLibraryEntryPoint,     _S( "KErrBadLibraryEntryPoint" ) },
    { KErrBadDescriptor,            _S( "KErrBadDescriptor" ) },
    { KErrAbort,                    _S( "KErrAbort" ) },
    { KErrTooBig,                   _S( "KErrTooBig" ) },
    { KErrDivideByZero,             _S( "KErrDivideByZero" ) },
    { KErrBadPower,                 _S( "KErrBadPower" ) },
    { KErrDirFull,                  _S( "KErrDirFull" ) },
    { KErrHardwareNotAvailable,     _S( "KErrHardwareNotAvailable" ) },
    { KErrSessionClosed,            _S( "KErrSessionClosed" ) },
    { KErrPermissionDenied,         _S( "KErrPermissionDenied" ) },
    { KErrExtensionNotSupported,    _S( "KErrExtensionNotSupported" ) },
    { KErrCommsBreak,               _S( "KErrCommsBreak" ) },
    { KErrAlreadyExists,            _S( "KErrAlreadyExists" ) },
    { KErrTimedOut,                 _S( "KErrTimedOut" ) },
    { KErrServerTerminated,         _S( "KErrServerTerminated" ) },
    { -500,                         _S( "Test case missing" ) },
    { -501,                         _S( "Unexpected event" ) },
    { -502,                         _S( "Channel missing" ) },
    { -503,                         _S( "Unexpected message length" ) },
    { -504,                         _S( "Unexpected message content" ) },
    { -505,                         _S( "Unexpected message direction" ) },
    { -506,                         _S( "Unexpected channel ID" ) },
    { -508,                         _S( "Unexpected element type" ) },
    { -509,                         _S( "Unexpected type attribute value" ) },
    { -512,                         _S( "Invalid ID" ) },
    { -1001,                        _S( "Generic format error" ) },
    { -1002,                        _S( "Generic parse error" ) },
    { -1501,                        _S( "ISC Already Running" ) },
    { -1502,                        _S( "ISC Already Synchronizing" ) },
    { -1503,                        _S( "ISC Test Case Missing" ) } };
    
const TUint KSatErrorListSize = static_cast<TUint>
    ( sizeof( KSatErrorList ) / sizeof( TSatError ) ) ; 
    
#endif // SATERRORS_H
