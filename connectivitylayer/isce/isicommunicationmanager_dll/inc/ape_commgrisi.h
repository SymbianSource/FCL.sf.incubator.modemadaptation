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

#ifndef _SYMBIAN_ISI_APE_COMMGRISI_H_
#define _SYMBIAN_ISI_APE_COMMGRISI_H_

#define APE_COMMUNICATION_MANAGER_SERVER_ISI_VERSION_Z	 0
#define APE_COMMUNICATION_MANAGER_SERVER_ISI_VERSION_Y	 1


//TABLES

//ConstantTable for APE_COMMGR_ERROR_CODES
#define APE_COMMGR_NOT_ALLOWED                            	0x00                                                 	
#define APE_COMMGR_OK                                     	0x01                                                 	


//SUBBLOCKS


//Definition for APE_COMMGR_SUBSCRIBE_SB
#define APE_COMMGR_SUBSCRIBE_SB                           	0x00                                                 	
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_SUBBLOCKLENGTH     	1 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_DEVICEID           	2 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_RESOURCEID         	3 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_FILLERBYTE1        	4 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_FILLERBYTE2        	5 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_FILLERBYTE3        	6 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_SB_OFFSET_MESSAGEID          	7 //size 1 byte(s)
#define SIZE_APE_COMMGR_SUBSCRIBE_SB                      	8


//MESSAGES


//Definition for APE_COMMGR_SUBSCRIBE_REQ
#define APE_COMMGR_SUBSCRIBE_REQ                          	0x00                                                 	
#define APE_COMMGR_SUBSCRIBE_REQ_OFFSET_TRANSID           	0 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_REQ_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_REQ_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_REQ_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_APE_COMMGR_SUBSCRIBE_REQ                     	4


//Definition for APE_COMMGR_SUBSCRIBE_RESP
#define APE_COMMGR_SUBSCRIBE_RESP                         	0x01                                                 	
#define APE_COMMGR_SUBSCRIBE_RESP_OFFSET_TRANSID          	0 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define APE_COMMGR_SUBSCRIBE_RESP_OFFSET_RESULT           	2 //size 1 byte(s)
#define SIZE_APE_COMMGR_SUBSCRIBE_RESP                    	3

#endif
