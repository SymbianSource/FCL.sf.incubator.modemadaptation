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



// ---------------------------------------------------------
// CDpDataConfig::DteTypeOfBreakSignal
// This method gets DTE interface type of break signal.
// ---------------------------------------------------------
//
inline TUint8 CDpDataConfig::DteTypeOfBreakSignal()
    {
    return iDteTypeOfBreakSignal;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetDteTypeOfBreakSignal
// This method sets DTE interface type of break signal.
// ---------------------------------------------------------
//
inline void CDpDataConfig::SetDteTypeOfBreakSignal(
    const TUint8 aType) // Type of break signal
    {
    iDteTypeOfBreakSignal = aType;
    }

// ---------------------------------------------------------
// CDpDataConfig::DteParityBitWhen7Mode
// This method gets DTE interface parity bit mode in 7 bit
// data mode.
// ---------------------------------------------------------
//
inline TUint8 CDpDataConfig::DteParityBitWhen7Mode()
    {
    return iDteParityBitWhen7Mode;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetDteParityBitWhen7Mode
// This method sets DTE interface parity bit mode in 7 bit
// data mode.
// ---------------------------------------------------------
//
inline void CDpDataConfig::SetDteParityBitWhen7Mode(
    const TUint8 aParity) // Parity bit mode
    {
    iDteParityBitWhen7Mode = aParity;
    }

// ---------------------------------------------------------
// CDpDataConfig::DteAutoBauding
// This method gets DTE interface auto bauding mode.
// ---------------------------------------------------------
//
inline TUint8 CDpDataConfig::DteAutoBauding()
    {
    return iDteAutoBauding;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetDteAutoBauding
// This method sets DTE interface auto bauding mode.
// ---------------------------------------------------------
//
inline void CDpDataConfig::SetDteAutoBauding(
    const TUint8 aAutoBauding) // Auto bauding mode
    {
    iDteAutoBauding = aAutoBauding;
    }

// ---------------------------------------------------------
// CDpDataConfig::EscChar
// This method gets DTE interface escape character.
// ---------------------------------------------------------
//
inline TUint8 CDpDataConfig::EscChar()
    {
    return iEscChar;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetEscChar
// This method sets DTE interface escape character.
// ---------------------------------------------------------
//
inline void CDpDataConfig::SetEscChar(
    const TUint8 aEscChar) // Escape character
    {
    iEscChar = aEscChar;
    }

// ---------------------------------------------------------
// CDpDataConfig::EscTime
// This method gets DTE interface escape time.
// ---------------------------------------------------------
//
inline TUint8 CDpDataConfig::EscTime()
    {
    return iEscTime;
    }

// ---------------------------------------------------------
// CDpDataConfig::SetEscTime
// This method sets DTE interface escape time.
// ---------------------------------------------------------
//
inline void CDpDataConfig::SetEscTime(
    const TUint8 aEscTime) // Escape time in 20 milliseconds
    {
    iEscTime = aEscTime;
    }

// ---------------------------------------------------------
// CDpDataConfig::IsFlowCtrlCharExtraction
// This method informs, whether DTE interface flow control
// extraction character is used.
// ---------------------------------------------------------
//
inline TBool CDpDataConfig::IsFlowCtrlCharExtraction()
    {
    return iFlowCtrlCharExtraction;
    }

// ---------------------------------------------------------
// CDpDataConfig::CommConfig
// This method gets DTE interface port configuration settings.
// ---------------------------------------------------------
//
inline const TCommConfigV01& CDpDataConfig::CommConfig()
    {
    return iConfig;
    }

// ---------------------------------------------------------
// CDpDataConfig::Handshake
// This method gets DTE interface handshake mode.
// ---------------------------------------------------------
//
inline TUint CDpDataConfig::Handshake()
    {
    return iConfig.iHandshake;
    }

// ---------------------------------------------------------
// CDpDataConfig::DataBits
// This method gets DTE interface data bits mode.
// ---------------------------------------------------------
//
inline TDataBits CDpDataConfig::DataBits()
    {
    return iConfig.iDataBits;
    }

// ---------------------------------------------------------
// CDpDataConfig::Parity
// This method gets DTE interface parity setting.
// ---------------------------------------------------------
//
inline TParity CDpDataConfig::Parity()
    {
    return iConfig.iParity;
    }

// ---------------------------------------------------------
// CDpDataConfig::XonChar
// This method gets DTE interface XON character.
// ---------------------------------------------------------
//
inline TText8 CDpDataConfig::XonChar()
    {
    return iConfig.iXonChar;
    }

// ---------------------------------------------------------
// CDpDataConfig::XoffChar
// This method gets DTE interface XOFF character.
// ---------------------------------------------------------
//
inline TText8 CDpDataConfig::XoffChar()
    {
    return iConfig.iXoffChar;
    }

// ---------------------------------------------------------
// CDpDataConfig::IsEchoDte
// This method gets DTE interface echo mode.
// ---------------------------------------------------------
//
inline TBool CDpDataConfig::IsEchoDte()
    {
    return iEchoDte;
    }

// ---------------------------------------------------------
// CDpDataConfig::TerminatorCount
// This method gets DTE interface terminator count.
// ---------------------------------------------------------
//
inline TInt CDpDataConfig::TerminatorCount()
    {
    return iConfig.iTerminatorCount;
    }

//  End of File
