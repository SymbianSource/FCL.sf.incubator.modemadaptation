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
* Name      : [ 0x00 ] Resource Common Messages (Extended)
* Version   : 000.031
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_COMM_EXTISI_H_
#define _SYMBIAN_ISI_COMM_EXTISI_H_

#define COMMON_MESSAGES_EXTENDED_ISI_VERSION_Z	 0
#define COMMON_MESSAGES_EXTENDED_ISI_VERSION_Y	 31


//CONSTANTS
#define COMMON_MESSAGE                                    	0xF0                                              	
#define COMM_MAX_FTD_STRING_LENGTH                        	0x7C                                              	
#define COMM_MAX_FTD_DATA_LENGTH                          	0xF8                                              	
#define COMM_MAX_FTD_DATA_ITEMS                           	0xFF                                              	


//TABLES

//ConstantTable for COMM_STRING_TYPE
#define COMM_STRING_ASCII                                 	0x00                                                 	
#define COMM_STRING_UNICODE                               	0x01                                                 	

//ConstantTable for COMM_ISA_ENTITY_NOT_REACHABLE_STATUS
#define COMM_ISA_ENTITY_NOT_AVAILABLE                     	0x00                                                 	
#define COMM_ISA_ENTITY_NOT_EXISTING                      	0x01                                                 	


//SUBBLOCKTEMPLATES


//Definition for COMM_FTD_DATA_BYTE
#define COMM_FTD_DATA_BYTE                                	                                                  	
#define COMM_FTD_DATA_BYTE_OFFSET_SUBBLOCKID              	0 //size 2 byte(s)
#define COMM_FTD_DATA_BYTE_OFFSET_SUBBLOCKLENGTH          	2 //size 1 byte(s)
#define COMM_FTD_DATA_BYTE_OFFSET_DATA                    	3 //size 1 byte(s)
#define SIZE_COMM_FTD_DATA_BYTE                           	4


//Definition for COMM_FTD_DATA_WORD
#define COMM_FTD_DATA_WORD                                	                                                  	
#define COMM_FTD_DATA_WORD_OFFSET_SUBBLOCKID              	0 //size 2 byte(s)
#define COMM_FTD_DATA_WORD_OFFSET_SUBBLOCKLENGTH          	2 //size 1 byte(s)
#define COMM_FTD_DATA_WORD_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define COMM_FTD_DATA_WORD_OFFSET_DATA                    	4 //size 2 byte(s)
#define COMM_FTD_DATA_WORD_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define COMM_FTD_DATA_WORD_OFFSET_FILLERBYTE3             	7 //size 1 byte(s)
#define SIZE_COMM_FTD_DATA_WORD                           	8


//Definition for COMM_FTD_DATA_DWORD
#define COMM_FTD_DATA_DWORD                               	                                                  	
#define COMM_FTD_DATA_DWORD_OFFSET_SUBBLOCKID             	0 //size 2 byte(s)
#define COMM_FTD_DATA_DWORD_OFFSET_SUBBLOCKLENGTH         	2 //size 1 byte(s)
#define COMM_FTD_DATA_DWORD_OFFSET_FILLERBYTE1            	3 //size 1 byte(s)
#define COMM_FTD_DATA_DWORD_OFFSET_DATA                   	4 //size 4 byte(s)
#define SIZE_COMM_FTD_DATA_DWORD                          	8


//Definition for COMM_FTD_DATA_STRING
#define COMM_FTD_DATA_STRING                              	                                                  	
#define COMM_FTD_DATA_STRING_OFFSET_SUBBLOCKID            	0 //size 2 byte(s)
#define COMM_FTD_DATA_STRING_OFFSET_SUBBLOCKLENGTH        	2 //size 1 byte(s)
#define COMM_FTD_DATA_STRING_OFFSET_STRINGLENGHT          	3 //size 1 byte(s)
#define COMM_FTD_DATA_STRING_OFFSET_DATA                  	4 //size 1 byte(s)
#define COMM_FTD_DATA_STRING_OFFSET_FILLERBYTE1           	5 //size 1 byte(s)
#define COMM_FTD_DATA_STRING_OFFSET_FILLERBYTE2           	6 //size 1 byte(s)
#define COMM_FTD_DATA_STRING_OFFSET_FILLERBYTE3           	7 //size 1 byte(s)
#define SIZE_COMM_FTD_DATA_STRING                         	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for COMM_FTD_DATA_TABLE
#define COMM_FTD_DATA_TABLE                               	                                                  	
#define COMM_FTD_DATA_TABLE_OFFSET_SUBBLOCKID             	0 //size 2 byte(s)
#define COMM_FTD_DATA_TABLE_OFFSET_SUBBLOCKLENGTH         	2 //size 1 byte(s)
#define COMM_FTD_DATA_TABLE_OFFSET_DATALENGTH             	3 //size 1 byte(s)
#define COMM_FTD_DATA_TABLE_OFFSET_DATA                   	4 //size 1 byte(s)
#define COMM_FTD_DATA_TABLE_OFFSET_FILLERBYTE1            	5 //size 1 byte(s)
#define COMM_FTD_DATA_TABLE_OFFSET_FILLERBYTE2            	6 //size 1 byte(s)
#define COMM_FTD_DATA_TABLE_OFFSET_FILLERBYTE3            	7 //size 1 byte(s)
#define SIZE_COMM_FTD_DATA_TABLE                          	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//MESSAGES


//SERVICE TYPES
#define COMM_ISA_ENTITY_NOT_REACHABLE_RESP                	0x14                                              	
#define COMM_SERVICE_NOT_IDENTIFIED_RESP                  	0x01                                                 	
#define COMM_ISI_VERSION_GET_REQ                          	0x12                                              	
#define COMM_ISI_VERSION_GET_RESP                         	0x13                                              	


//Definition for COMMON_MESSAGE
#define COMMON_MESSAGE                                    	0xF0                                              	


//Definition for COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP	0xF0                                              	
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_TRANSID	0 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_RESOURCEID	2 //size 2 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_SUBMESSAGEID	4 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_NOTDELIVEREDMESSAGEID	5 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_STATUS	6 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP_OFFSET_FILLERBYTE1	7 //size 1 byte(s)
#define SIZE_COMMON_MESSAGE_EXTENDED_COMM_ISA_ENTITY_NOT_REACHABLE_RESP	8


//Definition for COMMON_MESSAGE
#define COMMON_MESSAGE                                    	0xF0                                              	


//Definition for COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP	0xF0                                              	
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_TRANSID	0 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_RESOURCEID	2 //size 2 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_SUBMESSAGEID	4 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDMESSAGEID	5 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_FILLERBYTE1	6 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP_OFFSET_NOTSERVEDSUBMESSAGEID	7 //size 1 byte(s)
#define SIZE_COMMON_MESSAGE_EXTENDED_COMM_SERVICE_NOT_IDENTIFIED_RESP	8


//Definition for COMMON_MESSAGE
#define COMMON_MESSAGE                                    	0xF0                                              	


//Definition for COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ  	0xF0                                              	
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ_OFFSET_TRANSID	0 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ_OFFSET_RESOURCEID	2 //size 2 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ_OFFSET_SUBMESSAGEID	4 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ_OFFSET_FILLERBYTE1	5 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ_OFFSET_FILLERBYTE2	6 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ_OFFSET_FILLERBYTE3	7 //size 1 byte(s)
#define SIZE_COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_REQ	8


//Definition for COMMON_MESSAGE
#define COMMON_MESSAGE                                    	0xF0                                              	


//Definition for COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP 	0xF0                                              	
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP_OFFSET_TRANSID	0 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP_OFFSET_RESOURCEID	2 //size 2 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP_OFFSET_SUBMESSAGEID	4 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONZZZ	5 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP_OFFSET_ISIVERSIONYYY	6 //size 1 byte(s)
#define COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP_OFFSET_FILLERBYTE1	7 //size 1 byte(s)
#define SIZE_COMMON_MESSAGE_EXTENDED_COMM_ISI_VERSION_GET_RESP	8

#endif