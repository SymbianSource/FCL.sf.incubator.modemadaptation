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
* Name      : PN_MODEM_INFO [ 0xC5 ] Resource Modem Info Server
* Version   : 001.002
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_INFOISI_H_
#define _SYMBIAN_ISI_INFOISI_H_

#define MODEM_INFO_SERVER_ISI_VERSION_Z	 1
#define MODEM_INFO_SERVER_ISI_VERSION_Y	 2


//CONSTANTS
#define INFO_ANY_SIZE                                     	0x01                                                 	


//TABLES

//ConstantTable for M_INFO_GENERAL_SYM
#define M_INFO_OK                                         	0x00                                                 	
#define M_INFO_FAIL                                       	0x01                                                 	
#define M_INFO_NOT_SUPPORTED                              	0x02                                                 	

//ConstantTable for M_INFO_BATCH_TYPE
#define M_INFO_PP_FEATURE                                 	0x00                                                 	
#define M_INFO_PP_ALL_FEATURES                            	0x01                                                 	

//BitmaskTables for M_INFO_VERSION_TARGETS
#define M_INFO_MODEMSW                                    	0x01                                                 	


//SUBBLOCKS


//Definition for M_INFO_SB_MODEMSW_VERSION
#define M_INFO_SB_MODEMSW_VERSION                         	0x00                                              	
#define M_INFO_SB_MODEMSW_VERSION_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define M_INFO_SB_MODEMSW_VERSION_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define M_INFO_SB_MODEMSW_VERSION_OFFSET_FILLERBYTE1      	2 //size 1 byte(s)
#define M_INFO_SB_MODEMSW_VERSION_OFFSET_STRLEN           	3 //size 1 byte(s)
#define M_INFO_SB_MODEMSW_VERSION_OFFSET_MCUSWVERSNC8     	4 //size 1 byte(s)
#define SIZE_M_INFO_SB_MODEMSW_VERSION                    	5
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for M_INFO_SB_PP_DATA
#define M_INFO_SB_PP_DATA                                 	0x01                                              	
#define M_INFO_SB_PP_DATA_OFFSET_SUBBLOCKID               	0 //size 1 byte(s)
#define M_INFO_SB_PP_DATA_OFFSET_FILLERBYTE1              	1 //size 1 byte(s)
#define M_INFO_SB_PP_DATA_OFFSET_SUBBLOCKLEN              	2 //size 2 byte(s)
#define M_INFO_SB_PP_DATA_OFFSET_FILLERBYTE2              	4 //size 2 byte(s)
#define M_INFO_SB_PP_DATA_OFFSET_PPFEACNT                 	6 //size 2 byte(s)
#define SIZE_M_INFO_SB_PP_DATA                            	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//MESSAGES


//Definition for M_INFO_PP_DATA_READ_REQ
#define M_INFO_PP_DATA_READ_REQ                           	0x02                                              	
#define M_INFO_PP_DATA_READ_REQ_OFFSET_TRANSID            	0 //size 1 byte(s)
#define M_INFO_PP_DATA_READ_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define M_INFO_PP_DATA_READ_REQ_OFFSET_FILLER             	2 //size 1 byte(s)
#define M_INFO_PP_DATA_READ_REQ_OFFSET_SUBBLOCKCOUNT      	3 //size 1 byte(s)
#define SIZE_M_INFO_PP_DATA_READ_REQ                      	4


//Definition for M_INFO_PP_DATA_READ_RESP
#define M_INFO_PP_DATA_READ_RESP                          	0x03                                              	
#define M_INFO_PP_DATA_READ_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define M_INFO_PP_DATA_READ_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define M_INFO_PP_DATA_READ_RESP_OFFSET_STATUS            	2 //size 1 byte(s)
#define M_INFO_PP_DATA_READ_RESP_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_M_INFO_PP_DATA_READ_RESP                     	4


//Definition for M_INFO_PP_DATA_WRITE_REQ
#define M_INFO_PP_DATA_WRITE_REQ                          	0x04                                              	
#define M_INFO_PP_DATA_WRITE_REQ_OFFSET_TRANSID           	0 //size 1 byte(s)
#define M_INFO_PP_DATA_WRITE_REQ_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define M_INFO_PP_DATA_WRITE_REQ_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define M_INFO_PP_DATA_WRITE_REQ_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_M_INFO_PP_DATA_WRITE_REQ                     	4


//Definition for M_INFO_PP_DATA_WRITE_RESP
#define M_INFO_PP_DATA_WRITE_RESP                         	0x05                                              	
#define M_INFO_PP_DATA_WRITE_RESP_OFFSET_TRANSID          	0 //size 1 byte(s)
#define M_INFO_PP_DATA_WRITE_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define M_INFO_PP_DATA_WRITE_RESP_OFFSET_FILLERBYTE1      	2 //size 1 byte(s)
#define M_INFO_PP_DATA_WRITE_RESP_OFFSET_STATUS           	3 //size 1 byte(s)
#define SIZE_M_INFO_PP_DATA_WRITE_RESP                    	4


//Definition for M_INFO_VERSION_READ_REQ
#define M_INFO_VERSION_READ_REQ                           	0x00                                              	
#define M_INFO_VERSION_READ_REQ_OFFSET_TRANSID            	0 //size 1 byte(s)
#define M_INFO_VERSION_READ_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define M_INFO_VERSION_READ_REQ_OFFSET_FILLERBYTE1        	2 //size 2 byte(s)
#define M_INFO_VERSION_READ_REQ_OFFSET_TARGET             	4 //size 4 byte(s)
#define SIZE_M_INFO_VERSION_READ_REQ                      	8


//Definition for M_INFO_VERSION_READ_RESP
#define M_INFO_VERSION_READ_RESP                          	0x01                                              	
#define M_INFO_VERSION_READ_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define M_INFO_VERSION_READ_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define M_INFO_VERSION_READ_RESP_OFFSET_STATUS            	2 //size 1 byte(s)
#define M_INFO_VERSION_READ_RESP_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_M_INFO_VERSION_READ_RESP                     	4

#endif
