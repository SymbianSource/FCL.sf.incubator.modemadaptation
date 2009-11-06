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
* Name      : PN_CSD [ 0x16 ] Resource Circuit Switched Data Server
* Version   : 012.000
*
*/


#ifndef _SYMBIAN_ISI_CSDISI_H_
#define _SYMBIAN_ISI_CSDISI_H_

#define CIRCUIT_SWITCHED_DATA_SERVER_ISI_VERSION_Z	 12
#define CIRCUIT_SWITCHED_DATA_SERVER_ISI_VERSION_Y	 0


//CONSTANTS
#define CSD_NO_LINK_ID                                    	0x00                                                 	
#define CSD_ANY_SIZE                                      	0x01                                                 	


//TABLES

//ConstantTable for CSD_CALL_OPERATION_ID
#define CSD_CALL_CREATE                                   	0x00                                                 	
#define CSD_CALL_ANSWER                                   	0x01                                                 	
#define CSD_CALL_RELEASE                                  	0x02                                                 	
#define CSD_CALL_REJECT                                   	0x03                                                 	

//ConstantTable for CSD_CALL_CAUSE
#define CSD_CALL_CAUSE_NONE                               	0x00                                                 	
#define CSD_CALL_CAUSE_BEARER_RESERVATION_FAILED          	0x01                                                 	
#define CSD_CALL_CAUSE_NOT_ALLOWED                        	0x03                                                 	
#define CSD_CALL_CAUSE_NO_VIDEO_CALL                      	0x04                                                 	
#define CSD_CALL_CAUSE_OTHER_CALL_ACTIVE                  	0x05                                                 	
#define CSD_CALL_CAUSE_USER_ABORT                         	0x06                                                 	
#define CSD_CALL_CAUSE_INVALID_SYSTEM                     	0x07                                                 	
#define CSD_CALL_CAUSE_INVALID_CBST_PARAMS                	0x08                                                 	

//ConstantTable for CSD_BEARER_TYPE
#define CSD_BEARER_NOT_VALID                              	0x00                                                 	
#define CSD_BEARER_CSD                                    	0x01                                                 	

//ConstantTable for CSD_RESOURCE_OPERATION_ID
#define CSD_RESOURCE_RESERVE                              	0x00                                                 	
#define CSD_RESOURCE_RELEASE                              	0x01                                                 	

//ConstantTable for CSD_RESOURCE_STATUS_VALUE
#define CSD_RESOURCE_STATUS_NONE                          	0x00                                                 	
#define CSD_RESOURCE_STATUS_ALLOWED                       	0x01                                                 	
#define CSD_RESOURCE_STATUS_DENIED                        	0x02                                                 	

//ConstantTable for CSD_RESOURCE_VALUE
#define CSD_RESOURCE_ERROR                                	0x00                                                 	
#define CSD_RESOURCE_OK                                   	0x01                                                 	

//ConstantTable for CSD_VIDEO_CALL_STATUS_VALUE
#define CSD_VIDEO_CALL_STATUS_CONNECT                     	0x00                                                 	
#define CSD_VIDEO_CALL_STATUS_DISCONNECT                  	0x01                                                 	

//BitmaskTables for CSD_ADDRESS_TYPE
#define CSD_NBR_PLAN_UNKNOWN                              	0x00                                                 	
#define CSD_NBR_PLAN_ISDN_TELEPHONY                       	0x01                                                 	
#define CSD_NBR_PLAN_DATA                                 	0x03                                                 	
#define CSD_NBR_PLAN_TELEX                                	0x04                                                 	
#define CSD_NBR_PLAN_PRIVATE                              	0x09                                                 	
#define CSD_GSM_NBR_PLAN_NATIONAL                         	0x08                                                 	
#define CSD_NBR_TYPE_UNKNOWN                              	0x00                                                 	
#define CSD_NBR_TYPE_INTERNATIONAL                        	0x10                                              	
#define CSD_NBR_TYPE_NATIONAL                             	0x20                                              	
#define CSD_NBR_TYPE_NETWORK_SPECIFIC                     	0x30                                              	
#define CSD_GSM_NBR_TYPE_DEDICATED                        	0x80                                              	

//BitmaskTables for CSD_PRESENTATION_SCREEN_INDICATOR
#define CSD_PRESENTATION_ALLOWED                          	0x00                                                 	
#define CSD_PRESENTATION_RESTRICTED                       	0x01                                                 	
#define CSD_PRESENTATION_UNAVAILABLE                      	0x02                                                 	
#define CSD_GSM_PRESENTATION_DEFAULT                      	0x07                                                 	
#define CSD_USER_NOT_SCREENED                             	0x00                                                 	
#define CSD_USER_SCREENED_PASSED                          	0x10                                              	
#define CSD_USER_SCREENED_FAILED                          	0x20                                              	
#define CSD_SCREEN_NETW_PROVIDED                          	0x30                                              	

//BitmaskTables for CSD_ADDITIONAL_ADDRESS_INFO
#define CSD_P_ADDR_PRESENTATION_ALLOWED                   	0x01                                                 	

//BitmaskTables for CSD_PRESENTATION_INDICATOR
//#define CSD_PRESENTATION_ALLOWED                        	0x00                   	 //Already defined as 0x00
//#define CSD_PRESENTATION_RESTRICTED                     	0x00                   	 //Already defined as 0x01
//#define CSD_PRESENTATION_UNAVAILABLE                    	0x00                   	 //Already defined as 0x02
//#define CSD_GSM_PRESENTATION_DEFAULT                    	0x00                   	 //Already defined as 0x07


//SUBBLOCKS


//Definition for CSD_SB_CONN_SETTING
#define CSD_SB_CONN_SETTING                               	0x0D                                                 	
#define CSD_SB_CONN_SETTING_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define CSD_SB_CONN_SETTING_OFFSET_SUBBLOCKLENGTH         	1 //size 1 byte(s)
#define CSD_SB_CONN_SETTING_OFFSET_CONNSETTINGLENGTH      	2 //size 1 byte(s)
#define CSD_SB_CONN_SETTING_OFFSET_CONNSETTINGDATA        	3 //size 1 byte(s)
#define SIZE_CSD_SB_CONN_SETTING                          	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CSD_SB_RESOURCE_ID
#define CSD_SB_RESOURCE_ID                                	0x0E                                                 	
#define CSD_SB_RESOURCE_ID_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define CSD_SB_RESOURCE_ID_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define CSD_SB_RESOURCE_ID_OFFSET_RESOURCEID              	2 //size 1 byte(s)
#define CSD_SB_RESOURCE_ID_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_CSD_SB_RESOURCE_ID                           	4


//Definition for CSD_DESTINATION_ADDRESS
#define CSD_DESTINATION_ADDRESS                           	0x0F                                                 	
#define CSD_DESTINATION_ADDRESS_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define CSD_DESTINATION_ADDRESS_OFFSET_SUBBLOCKLEN        	1 //size 1 byte(s)
#define CSD_DESTINATION_ADDRESS_OFFSET_ADDRTYPE           	2 //size 1 byte(s)
#define CSD_DESTINATION_ADDRESS_OFFSET_PRESENTATION       	3 //size 1 byte(s)
#define CSD_DESTINATION_ADDRESS_OFFSET_FILLERBYTE1        	4 //size 1 byte(s)
#define CSD_DESTINATION_ADDRESS_OFFSET_ADDRLEN            	5 //size 1 byte(s)
#define CSD_DESTINATION_ADDRESS_OFFSET_ADDR               	6 //size 2 byte(s)
#define SIZE_CSD_DESTINATION_ADDRESS                      	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CSD_DESTINATION_PRE_ADDRESS
#define CSD_DESTINATION_PRE_ADDRESS                       	0x10                                              	
#define CSD_DESTINATION_PRE_ADDRESS_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define CSD_DESTINATION_PRE_ADDRESS_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define CSD_DESTINATION_PRE_ADDRESS_OFFSET_ADDRINFO       	2 //size 1 byte(s)
#define CSD_DESTINATION_PRE_ADDRESS_OFFSET_ADDRLEN        	3 //size 1 byte(s)
#define CSD_DESTINATION_PRE_ADDRESS_OFFSET_ADDR           	4 //size 2 byte(s)
#define CSD_DESTINATION_PRE_ADDRESS_OFFSET_FILLERBYTE1    	6 //size 1 byte(s)
#define CSD_DESTINATION_PRE_ADDRESS_OFFSET_FILLERBYTE2    	7 //size 1 byte(s)
#define SIZE_CSD_DESTINATION_PRE_ADDRESS                  	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CSD_DESTINATION_POST_ADDRESS
#define CSD_DESTINATION_POST_ADDRESS                      	0x11                                              	
#define CSD_DESTINATION_POST_ADDRESS_OFFSET_SUBBLOCKID    	0 //size 1 byte(s)
#define CSD_DESTINATION_POST_ADDRESS_OFFSET_SUBBLOCKLEN   	1 //size 1 byte(s)
#define CSD_DESTINATION_POST_ADDRESS_OFFSET_ADDRINFO      	2 //size 1 byte(s)
#define CSD_DESTINATION_POST_ADDRESS_OFFSET_ADDRLEN       	3 //size 1 byte(s)
#define CSD_DESTINATION_POST_ADDRESS_OFFSET_ADDR          	4 //size 2 byte(s)
#define CSD_DESTINATION_POST_ADDRESS_OFFSET_FILLERBYTE1   	6 //size 1 byte(s)
#define CSD_DESTINATION_POST_ADDRESS_OFFSET_FILLERBYTE2   	7 //size 1 byte(s)
#define SIZE_CSD_DESTINATION_POST_ADDRESS                 	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CSD_ORIGIN_INFO
#define CSD_ORIGIN_INFO                                   	0x12                                              	
#define CSD_ORIGIN_INFO_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define CSD_ORIGIN_INFO_OFFSET_SUBBLOCKLEN                	1 //size 1 byte(s)
#define CSD_ORIGIN_INFO_OFFSET_PRESENTATION               	2 //size 1 byte(s)
#define CSD_ORIGIN_INFO_OFFSET_FILLERBYTE1                	3 //size 1 byte(s)
#define CSD_ORIGIN_INFO_OFFSET_LINKID                     	4 //size 1 byte(s)
#define CSD_ORIGIN_INFO_OFFSET_NAMELEN                    	5 //size 1 byte(s)
#define CSD_ORIGIN_INFO_OFFSET_NAME                       	6 //size 2 byte(s)
#define SIZE_CSD_ORIGIN_INFO                              	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CSD_CBST_PARAMS
#define CSD_CBST_PARAMS                                   	0x15                                              	
#define CSD_CBST_PARAMS_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define CSD_CBST_PARAMS_OFFSET_SUBBLOCKLEN                	1 //size 1 byte(s)
#define CSD_CBST_PARAMS_OFFSET_SPEED                      	2 //size 1 byte(s)
#define CSD_CBST_PARAMS_OFFSET_NAME                       	3 //size 1 byte(s)
#define CSD_CBST_PARAMS_OFFSET_CE                         	4 //size 1 byte(s)
#define CSD_CBST_PARAMS_OFFSET_FILLERBYTE1                	5 //size 3 byte(s)
#define SIZE_CSD_CBST_PARAMS                              	8


//MESSAGES


//Definition for CSD_MULTIMEDIA_DATA_RATE_IND
#define CSD_MULTIMEDIA_DATA_RATE_IND                      	0x06                                                 	
#define CSD_MULTIMEDIA_DATA_RATE_IND_OFFSET_TID           	0 //size 1 byte(s)
#define CSD_MULTIMEDIA_DATA_RATE_IND_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define CSD_MULTIMEDIA_DATA_RATE_IND_OFFSET_FILLERBYTE1   	2 //size 2 byte(s)
#define CSD_MULTIMEDIA_DATA_RATE_IND_OFFSET_RATE          	4 //size 4 byte(s)
#define SIZE_CSD_MULTIMEDIA_DATA_RATE_IND                 	8


//Definition for CSD_CALL_CONTROL_REQ
#define CSD_CALL_CONTROL_REQ                              	0x07                                                 	
#define CSD_CALL_CONTROL_REQ_OFFSET_TID                   	0 //size 1 byte(s)
#define CSD_CALL_CONTROL_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define CSD_CALL_CONTROL_REQ_OFFSET_OPERID                	2 //size 1 byte(s)
#define CSD_CALL_CONTROL_REQ_OFFSET_SUBBLOCKS             	3 //size 1 byte(s)
#define SIZE_CSD_CALL_CONTROL_REQ                         	4


//Definition for CSD_CALL_CONTROL_RESP
#define CSD_CALL_CONTROL_RESP                             	0x08                                                 	
#define CSD_CALL_CONTROL_RESP_OFFSET_TID                  	0 //size 1 byte(s)
#define CSD_CALL_CONTROL_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define CSD_CALL_CONTROL_RESP_OFFSET_CAUSE                	2 //size 1 byte(s)
#define CSD_CALL_CONTROL_RESP_OFFSET_OPERID               	3 //size 1 byte(s)
#define SIZE_CSD_CALL_CONTROL_RESP                        	4


//Definition for CSD_RESOURCE_IND
#define CSD_RESOURCE_IND                                  	0x0C                                                 	
#define CSD_RESOURCE_IND_OFFSET_TID                       	0 //size 1 byte(s)
#define CSD_RESOURCE_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define CSD_RESOURCE_IND_OFFSET_OPERID                    	2 //size 1 byte(s)
#define CSD_RESOURCE_IND_OFFSET_BEARERTYPE                	3 //size 1 byte(s)
#define CSD_RESOURCE_IND_OFFSET_FILLERBYTE1               	4 //size 3 byte(s)
#define CSD_RESOURCE_IND_OFFSET_SUBBLOCKS                 	7 //size 1 byte(s)
#define SIZE_CSD_RESOURCE_IND                             	8


//Definition for CSD_RESOURCE_REQ
#define CSD_RESOURCE_REQ                                  	0x0D                                                 	
#define CSD_RESOURCE_REQ_OFFSET_TID                       	0 //size 1 byte(s)
#define CSD_RESOURCE_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define CSD_RESOURCE_REQ_OFFSET_OPERID                    	2 //size 1 byte(s)
#define CSD_RESOURCE_REQ_OFFSET_RESERVATIONID             	3 //size 1 byte(s)
#define CSD_RESOURCE_REQ_OFFSET_STATUS                    	4 //size 1 byte(s)
#define CSD_RESOURCE_REQ_OFFSET_FILLERBYTE1               	5 //size 3 byte(s)
#define SIZE_CSD_RESOURCE_REQ                             	8


//Definition for CSD_RESOURCE_RESP
#define CSD_RESOURCE_RESP                                 	0x0E                                                 	
#define CSD_RESOURCE_RESP_OFFSET_TID                      	0 //size 1 byte(s)
#define CSD_RESOURCE_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define CSD_RESOURCE_RESP_OFFSET_RESULT                   	2 //size 1 byte(s)
#define CSD_RESOURCE_RESP_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_CSD_RESOURCE_RESP                            	4


//Definition for CSD_VIDEO_CALL_STATUS_IND
#define CSD_VIDEO_CALL_STATUS_IND                         	0x0F                                                 	
#define CSD_VIDEO_CALL_STATUS_IND_OFFSET_TID              	0 //size 1 byte(s)
#define CSD_VIDEO_CALL_STATUS_IND_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define CSD_VIDEO_CALL_STATUS_IND_OFFSET_STATUS           	2 //size 1 byte(s)
#define CSD_VIDEO_CALL_STATUS_IND_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_CSD_VIDEO_CALL_STATUS_IND                    	4

#endif