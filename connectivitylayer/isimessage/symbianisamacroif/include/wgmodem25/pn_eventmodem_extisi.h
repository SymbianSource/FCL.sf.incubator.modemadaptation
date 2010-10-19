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
* Name      : pn_event header definitions
* Version   : 001.001
*
*/


#ifndef PNEVENTMODEMEXTISI_H
#define PNEVENTMODEMEXTISI_H

// INCLUDE FILES
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
//None


// FORWARD DECLARATIONS
//Definition for PNS_SUBSCRIBED_RESOURCES_EXTEND_IND
#define PNS_SUBSCRIBED_RESOURCES_EXTEND_IND               	        0x12                                              	
#define PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_TRANSACTIONID	0 //size 1 byte(s)
#define PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_MESSAGEID	    1 //size 1 byte(s)
#define PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCECOUNT	2 //size 1 byte(s)
#define PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_FILLER 	        3 //size 1 byte(s)
#define PNS_SUBSCRIBED_RESOURCES_EXTEND_IND_OFFSET_RESOURCELIST	    4 //size 4 byte(s)
#define SIZE_PNS_SUBSCRIBED_RESOURCES_EXTEND_IND          	        8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.

#endif
