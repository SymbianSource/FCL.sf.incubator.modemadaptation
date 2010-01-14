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
* Name      : PN_NVD [ 0x43 ] Resource Non-Volatile Data Server
* Version   : 000.001
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_NVDISI_H_
#define _SYMBIAN_ISI_NVDISI_H_

#define NON_VOLATILE_DATA_SERVER_ISI_VERSION_Z	 0
#define NON_VOLATILE_DATA_SERVER_ISI_VERSION_Y	 1


//TABLES

//ConstantTable for NVD_STATUS
#define NVD_STATUS_OK                                     	0x00                                                 	
#define NVD_STATUS_FAIL                                   	0x01                                                 	
#define NVD_STATUS_NONE                                   	0x02                                                 	


//MESSAGES


//Definition for NVD_SET_DEFAULT_REQ
#define NVD_SET_DEFAULT_REQ                               	0x01                                                 	
#define NVD_SET_DEFAULT_REQ_OFFSET_TRANSACTIONID          	0 //size 1 byte(s)
#define NVD_SET_DEFAULT_REQ_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define NVD_SET_DEFAULT_REQ_OFFSET_FILLERBYTE1            	2 //size 2 byte(s)
#define SIZE_NVD_SET_DEFAULT_REQ                          	4


//Definition for NVD_SET_DEFAULT_RESP
#define NVD_SET_DEFAULT_RESP                              	0x02                                                 	
#define NVD_SET_DEFAULT_RESP_OFFSET_TRANSACTIONID         	0 //size 1 byte(s)
#define NVD_SET_DEFAULT_RESP_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define NVD_SET_DEFAULT_RESP_OFFSET_STATUS                	2 //size 1 byte(s)
#define NVD_SET_DEFAULT_RESP_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_NVD_SET_DEFAULT_RESP                         	4


#endif