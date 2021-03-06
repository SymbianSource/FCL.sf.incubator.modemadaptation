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

#ifdef PIPECAMP_DATAPORT_PNS_PEP_STATUS_IND_PHONET_ADDRESS_FROM_PNS_PEP_CTRL_REQ // 20100523_1300    
inline void CDpPif::SetPipeControllerDeviceIdentifier(
    const TUint8 aDeviceId )
    {
    iPipeControllerDevId = aDeviceId;
    }

inline void CDpPif::SetPipeControllerObjectIdentifier(
    const TUint8 aObjectId )
    {
    iPipeControllerObjId = aObjectId;
    }

inline TUint8 CDpPif::GetPipeControllerDeviceIdentifier()
    {
    return iPipeControllerDevId;
    }

inline TUint8 CDpPif::GetPipeControllerObjectIdentifier()
    {
    return iPipeControllerObjId;
    }
#endif
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
