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



// INCLUDE FILES
// none

// EXTERNAL DATA STRUCTURES
// none

// EXTERNAL FUNCTION PROTOTYPES  
// none

// CONSTANTS
// none

// MACROS
// none

// LOCAL CONSTANTS AND MACROS
// none

// MODULE DATA STRUCTURES
// none

// LOCAL FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// ============================= LOCAL FUNCTIONS ===============================
// none

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDpFlowCtrl::FlowCtrlDp2Dcs
// This method returns Dp2Dcs flow control status.
// -----------------------------------------------------------------------------
//
inline TFlowControl CDpFlowCtrl::FlowCtrlDp2Dcs()
    {
    return iFlowCtrlDp2Dcs;
    }

// -----------------------------------------------------------------------------
// CDpFlowCtrl::FlowCtrlDcs2Dp
// This method returns Dcs2Dp flow control status.
// -----------------------------------------------------------------------------
//
inline TFlowControl CDpFlowCtrl::FlowCtrlDcs2Dp()
    {
    return iFlowCtrlDcs2Dp;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// none

//  End of File
