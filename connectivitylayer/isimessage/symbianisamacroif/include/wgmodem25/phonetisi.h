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
* Name      : phonet header definitions
* Version   : 004.000
*
*/


#ifndef PHONETISI_H
#define PHONETISI_H


// INCLUDE FILES
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
//None


// FORWARD DECLARATIONS

#define ISI_HEADER_OFFSET_MEDIA          0
#define ISI_HEADER_OFFSET_RECEIVERDEVICE 1
#define ISI_HEADER_OFFSET_SENDERDEVICE   2
#define ISI_HEADER_OFFSET_RESOURCEID     3
#define ISI_HEADER_OFFSET_LENGTH         4
#define ISI_HEADER_OFFSET_RECEIVEROBJECT 6
#define ISI_HEADER_OFFSET_SENDEROBJECT   7
#define ISI_HEADER_OFFSET_TRANSID        8
#define ISI_HEADER_OFFSET_MESSAGEID      9
#define ISI_HEADER_OFFSET_SUBMESSAGEID  10

#define ISI_HEADER_SIZE                  8

// For extended resourc id handling
#define ISI_HEADER_OFFSET_TYPE          10
#define ISI_HEADER_OFFSET_SUBTYPE       11

#endif
