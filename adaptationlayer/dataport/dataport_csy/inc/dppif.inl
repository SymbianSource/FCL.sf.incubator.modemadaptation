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
// CDpPif::PipeState
// This method returns pipe state.
// ---------------------------------------------------------
//
inline CDpPif::TDpPipeState CDpPif::PipeState()
    {
    return iPipeState;
    }

// ---------------------------------------------------------
// CDpPif::PipeHandle
// This method returns pipe handle.
// ---------------------------------------------------------
//
inline TUint8 CDpPif::PipeHandle()
    {
    return iPipeHandle;
    }

// ---------------------------------------------------------
// CDpPif::SetPipeHandle
// This method sets pipe handle.
// ---------------------------------------------------------
//
inline void CDpPif::SetPipeHandle(
    const TUint8 aPipeHandle ) 
    {
    iPipeHandle = aPipeHandle;
    }

// ---------------------------------------------------------
// CDpPif::SetPipeState
// This method sets pipe state.
// ---------------------------------------------------------
//
inline void CDpPif::SetPipeState(
    const TDpPipeState aPipeState ) 
    {
    iPipeState = aPipeState;Notify();
    }

// ---------------------------------------------------------
// CDpPif::PifState
// This method returns pif state.
// ---------------------------------------------------------
//
inline CDpPif::TDpPifState CDpPif::PifState()
    {
    return iPifState;
    }

//  End of File
