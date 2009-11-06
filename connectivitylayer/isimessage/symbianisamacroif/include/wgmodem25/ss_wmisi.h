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
* Name      : PN_SS [ 0x06 ] Resource Supplementary Services Server
* Version   : 013.000
*
*/


#ifndef _SYMBIAN_ISI_SS_WMISI_H_
#define _SYMBIAN_ISI_SS_WMISI_H_

#define SUPPLEMENTARY_SERVICES_SERVER_ISI_VERSION_Z	 13
#define SUPPLEMENTARY_SERVICES_SERVER_ISI_VERSION_Y	 0


//CONSTANTS
#define MAX_USSD_LENGTH                                   	0xA0                                              	
#define ISI_MSG_OK                                        	0xFF                                              	
#define PASSWORD_LENGTH                                   	0x04                                                 	
#define UNICODE_PASSWORD_LENGTH                           	0x08                                                 	
#define SS_UNDEFINED_TIME                                 	0x00                                                 	
#define SS_UNKNOWN_NUMBER                                 	0x3F                                              	
#define SS_ANY_SIZE                                       	0x01                                                 	


//TABLES

//ConstantTable for SS_GSM_SS_CODES
#define SS_GSM_ALL_SS                                     	0x00                                                 	
#define SS_GSM_ALL_FORWARDINGS                            	0x02                                                 	
#define SS_GSM_ALL_COND_FORWARDINGS                       	0x04                                                 	
#define SS_GSM_FORW_UNCONDITIONAL                         	0x15                                              	
#define SS_GSM_FORW_BUSY                                  	0x43                                              	
#define SS_GSM_FORW_NO_REPLY                              	0x3D                                              	
#define SS_GSM_FORW_NO_REACH                              	0x3E                                              	
#define SS_GSM_ALL_BARRINGS                               	0x14A                                             	
#define SS_GSM_BARR_ALL_OUT                               	0x21                                              	
#define SS_GSM_BARR_OUT_INTER                             	0x14B                                             	
#define SS_GSM_BARR_OUT_INTER_EXC_HOME                    	0x14C                                             	
#define SS_GSM_BARR_ALL_IN                                	0x23                                              	
#define SS_GSM_BARR_ALL_IN_ROAM                           	0x15F                                             	
#define SS_GSM_OUTGOING_BARR_SERV                         	0x14D                                             	
#define SS_GSM_INCOMING_BARR_SERV                         	0x161                                             	
#define SS_GSM_CALL_WAITING                               	0x2B                                              	
#define SS_GSM_CLIP                                       	0x1E                                              	
#define SS_GSM_CLIR                                       	0x1F                                              	
#define SS_GSM_COLP                                       	0x4C                                              	
#define SS_GSM_COLR                                       	0x4D                                              	
#define SS_GSM_CNAP                                       	0x12C                                             	
#define SS_GSM_ECT                                        	0x60                                              	

//ConstantTable for SS_BASIC_SERVICE_CODES
#define SS_ALL_TELE_AND_BEARER                            	0x00                                                 	
#define SS_GSM_ALL_TELE                                   	0x0A                                                 	
#define SS_GSM_TELEPHONY                                  	0x0B                                                 	
#define SS_GSM_ALL_DATA_TELE                              	0x0C                                                 	
#define SS_GSM_FACSIMILE                                  	0x0D                                                 	
#define SS_GSM_SMS                                        	0x10                                              	
#define SS_GSM_VOICE_GROUP                                	0x11                                              	
#define SS_GSM_ALL_TELE_EXC_SMS                           	0x13                                              	
#define SS_GSM_ALL_PLMN_SPEC_TELE_SERV                    	0x32                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_1                      	0x33                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_2                      	0x34                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_3                      	0x35                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_4                      	0x36                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_5                      	0x37                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_6                      	0x38                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_7                      	0x39                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_8                      	0x3A                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_9                      	0x3B                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_A                      	0x3C                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_B                      	0x3D                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_C                      	0x3E                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_D                      	0x3F                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_E                      	0x40                                              	
#define SS_GSM_PLMN_SPEC_TELE_SERV_F                      	0x41                                              	
#define SS_GSM_AUX_TELEPHONY                              	0x59                                              	
#define SS_GSM_ALL_BEARER                                 	0x14                                              	
#define SS_GSM_ALL_ASYNC                                  	0x15                                              	
#define SS_GSM_ALL_SYNC                                   	0x16                                              	
#define SS_GSM_ALL_DATA_CIRCUIT_SYNC                      	0x18                                              	
#define SS_GSM_ALL_DATA_CIRCUIT_ASYNC                     	0x19                                              	
#define SS_GSM_ALL_DATA_PACKET_SYNC                       	0x1A                                              	
#define SS_GSM_ALL_PAD_ACCESS                             	0x1B                                              	
#define SS_GSM_ALL_PLMN_SPEC_BEAR_SERV                    	0x46                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_1                      	0x47                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_2                      	0x48                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_3                      	0x49                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_4                      	0x4A                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_5                      	0x4B                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_6                      	0x4C                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_7                      	0x4D                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_8                      	0x4E                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_9                      	0x4F                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_A                      	0x50                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_B                      	0x51                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_C                      	0x52                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_D                      	0x53                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_E                      	0x54                                              	
#define SS_GSM_PLMN_SPEC_BEAR_SERV_F                      	0x55                                              	
#define SS_UNKNOWN_SERVICE                                	0xFF                                              	

//ConstantTable for SS_GSM_ERROR_CODES
#define SS_GSM_UNKNOWN_ERROR                              	0x00                                                 	
#define SS_GSM_UNKNOWN_SUBSCRIBER                         	0x01                                                 	
#define SS_GSM_ILLEGAL_SUBSCRIBER                         	0x09                                                 	
#define SS_GSM_BEARER_SERV_NOT_PROVISION                  	0x0A                                                 	
#define SS_GSM_TELE_SERV_NOT_PROVISION                    	0x0B                                                 	
#define SS_GSM_ILLEGAL_EQUIPMENT                          	0x0C                                                 	
#define SS_GSM_CALL_BARRED                                	0x0D                                                 	
#define SS_GSM_ILLEGEL_SS_OPERATION                       	0x10                                              	
#define SS_GSM_SS_ERROR_STATUS                            	0x11                                              	
#define SS_GSM_SS_NOT_AVAILABLE                           	0x12                                              	
#define SS_GSM_SS_SUBSCRIPTION_VIOLATION                  	0x13                                              	
#define SS_GSM_SS_INCOMPATIBILITY                         	0x14                                              	
#define SS_GSM_FACILITY_NOT_SUPPORTED                     	0x15                                              	
#define SS_GSM_ABSENT_SUBSCRIBER                          	0x1B                                              	
#define SS_GSM_SHORT_TERM_DENIAL                          	0x1D                                              	
#define SS_GSM_LONG_TERM_DENIAL                           	0x1E                                              	
#define SS_GSM_SYSTEM_FAILURE                             	0x22                                              	
#define SS_GSM_DATA_MISSING                               	0x23                                              	
#define SS_GSM_UNEXPECTED_DATA_VALUE                      	0x24                                              	
#define SS_GSM_PASSWORD_REGIS_FAILURE                     	0x25                                              	
#define SS_GSM_NEGATIVE_PASSWORD_CHECK                    	0x26                                              	
#define SS_GSM_MAXNUM_OF_PW_ATT_VIOLATION                 	0x2B                                              	
#define SS_GSM_UNKNOWN_ALPHABET                           	0x47                                              	
#define SS_GSM_USSD_BUSY                                  	0x48                                              	
#define SS_GSM_NBR_SB_EXCEEDED                            	0x78                                              	
#define SS_GSM_MAXNUM_OF_MPTY_PART_EXCEED                 	0x7E                                              	
#define SS_GSM_RESOURCES_NOT_AVAILABLE                    	0x7F                                              	

//ConstantTable for SS_PROBLEM_TYPE
#define SS_GSM_GENERAL_PROBLEM                            	0x80                                              	
#define SS_GSM_INVOKE_PROBLEM                             	0x81                                              	
#define SS_GSM_RETURN_RESULT_PROBLEM                      	0x82                                              	
#define SS_GSM_RETURN_ERROR_PROBLEM                       	0x83                                              	

//ConstantTable for SS_GSM_GENERAL_PROBLEM
#define SS_GSM_UNRECOGNIZED_COMP                          	0x00                                                 	
#define SS_GSM_MISTYPED_COMP                              	0x01                                                 	
#define SS_GSM_BADLY_STRUCTURED_COMP                      	0x02                                                 	

//ConstantTable for SS_GSM_INVOKE_PROBLEM
#define SS_GSM_DUPLICATE_INVOKE_ID                        	0x00                                                 	
#define SS_GSM_UNRECOGNIZED_OPERATION                     	0x01                                                 	
#define SS_GSM_MISTYPED_INVOKE_PARAMETER                  	0x02                                                 	
#define SS_GSM_RECOURCE_LIMITATION                        	0x03                                                 	
#define SS_GSM_INITIATING_RELEASE                         	0x04                                                 	
#define SS_GSM_UNRECOGNIZED_LINKED_ID                     	0x05                                                 	
#define SS_GSM_LINKED_RESPONSE_UNEXPECTED                 	0x06                                                 	
#define SS_GSM_UNEXPECTED_LINKED_OPERATION                	0x07                                                 	

//ConstantTable for SS_GSM_RETURN_RESULT_PROBLEM
#define SS_GSM_UNRECOGNIZED_INVOKE_ID                     	0x00                                                 	
#define SS_GSM_RETURN_RESULT_UNEXPECTED                   	0x01                                                 	
#define SS_GSM_MISTYPED_RESP_PARAMETER                    	0x02                                                 	

//ConstantTable for SS_GSM_RETURN_ERROR_PROBLEM
//#define SS_GSM_UNRECOGNIZED_INVOKE_ID                   	0x00                   	 //Already defined as 0x00
#define SS_GSM_RETURN_ERROR_UNEXPECTED                    	0x01                                                 	
#define SS_GSM_UNRECOGNIZED_ERROR                         	0x02                                                 	
#define SS_GSM_UNEXPECTED_ERROR                           	0x03                                                 	
#define SS_GSM_MISTYPED_ERR_PARAMETER                     	0x04                                                 	

//ConstantTable for SS_OPERATION
#define SS_ACTIVATION                                     	0x01                                                 	
#define SS_DEACTIVATION                                   	0x02                                                 	
#define SS_REGISTRATION                                   	0x03                                                 	
#define SS_ERASURE                                        	0x04                                                 	
#define SS_INTERROGATION                                  	0x05                                                 	
#define SS_GSM_PASSWORD_REGISTRATION                      	0x06                                                 	

//ConstantTable for SS_GSM_CLI_RESTRICTION_OPTION
#define SS_GSM_CLI_PERMANENT                              	0x00                                                 	
#define SS_GSM_DEFAULT_RESTRICTED                         	0x01                                                 	
#define SS_GSM_CLI_DEFAULT_ALLOWED                        	0x02                                                 	
#define SS_GSM_OVERRIDE_ENABLED                           	0x03                                                 	
#define SS_GSM_OVERRIDE_DISABLED                          	0x04                                                 	

//ConstantTable for SS_GSM_GUIDANCE_INFO
#define SS_GSM_BAD_PASSWORD                               	0x01                                                 	
#define SS_GSM_BAD_PASSWORD_FORMAT                        	0x02                                                 	

//ConstantTable for SS_GSM_MESSAGE_ERRORS
#define SS_GSM_MSG_INCORRECT                              	0x01                                                 	
#define SS_GSM_MSG_UNKNOWN_COMPONENT                      	0x02                                                 	
#define SS_GSM_MSG_INVALID_INVOKE_ID                      	0x03                                                 	

//ConstantTable for SS_GSM_USSD_TYPES
#define SS_GSM_USSD_MT_REPLY                              	0x01                                                 	
#define SS_GSM_USSD_COMMAND                               	0x02                                                 	
#define SS_GSM_USSD_REQUEST                               	0x03                                                 	
#define SS_GSM_USSD_NOTIFY                                	0x04                                                 	
#define SS_GSM_USSD_END                                   	0x05                                                 	

//BitmaskTables for SS_STATUS
#define SS_GSM_ACTIVE                                     	0x01                                                 	
#define SS_GSM_REGISTERED                                 	0x02                                                 	
#define SS_GSM_PROVISIONED                                	0x04                                                 	
#define SS_GSM_QUIESCENT                                  	0x08                                                 	

//ConstantTable for SS_STATUS_INDICATIONS
#define SS_STATUS_REQUEST_SERVICE_START                   	0x00                                                 	
#define SS_STATUS_REQUEST_SERVICE_STOP                    	0x01                                                 	
#define SS_GSM_STATUS_REQUEST_USSD_START                  	0x02                                                 	
#define SS_GSM_STATUS_REQUEST_USSD_STOP                   	0x03                                                 	
#define SS_GSM_STATUS_REQUEST_USSD_FAILED                 	0x04                                                 	
#define SS_STATUS_SERVICE_CANCELLED                       	0x05                                                 	
#define SS_STATUS_REQUEST_SERVICE_BUSY                    	0x06                                                 	
#define SS_STATUS_REQUEST_SERVICE_FAILED                  	0x07                                                 	
#define SS_GSM_STATUS_REQUEST_USSD_BUSY                   	0x08                                                 	
#define SS_STATUS_SERVICE_RELEASED                        	0x09                                                 	

//ConstantTable for SS_MISC_SS_ERRORS
#define SS_SERVICE_BUSY                                   	0x00                                                 	
#define SS_GSM_MM_RELEASE                                 	0x01                                                 	
#define SS_GSM_DATA_ERROR                                 	0x02                                                 	
#define SS_GSM_REQUEST_CANCELLED                          	0x04                                                 	
#define SS_GSM_INDICATE_DEVICE_PASSWORD                   	0x05                                                 	
#define SS_GSM_ACTIVATION_DATA_LOST                       	0x09                                                 	
#define SS_GSM_MM_ERROR                                   	0x0B                                                 	
#define SS_GSM_SERVICE_NOT_ON_FDN_LIST                    	0x0C                                                 	
#define SS_CS_INACTIVE                                    	0x0F                                                 	
//#define SS_GSM_SS_NOT_AVAILABLE                         	0x12                  	 //Already defined as 0x12
#define SS_RESOURCE_CONTROL_DENIED                        	0x13                                              	
#define SS_RESOURCE_CONTROL_FAILURE                       	0x14                                              	
#define SS_RESOURCE_CONTROL_CONF_FAIL                     	0x15                                              	
//#define SS_GSM_USSD_BUSY                                	0x48                  	 //Already defined as 0x48
#define SS_SERVICE_REQUEST_RELEASED                       	0x16                                              	

//ConstantTable for SS_REASON_VALUES
#define SS_UNSPECIFIED_REASON                             	0x00                                                 	
#define SS_UNKNOWN_MMI_SS_CODE                            	0x01                                                 	

//BitmaskTables for SS_GMS_FORWARDING_OPTION
#define SS_GSM_FORW_NOTIF_REASON_1                        	0x04                                                 	
#define SS_GSM_FORW_NOTIF_REASON_2                        	0x08                                                 	
#define SS_GSM_FORW_NOTIF_TO_CALLER                       	0x20                                              	
#define SS_GSM_FORW_NOTIF_TO_FORWARDING                   	0x80                                              	

//ConstantTable for SS_NUMBER_PLAN
#define SS_NBR_PLAN_UNKNOWN                               	0x00                                                 	
#define SS_NBR_PLAN_ISDN_TELEPHONY                        	0x01                                                 	
#define SS_NBR_PLAN_DATA                                  	0x03                                                 	
#define SS_NBR_PLAN_TELEX                                 	0x04                                                 	
#define SS_NBR_PLAN_LAND_MOBILE                           	0x06                                                 	
#define SS_NBR_PLAN_NATIONAL                              	0x08                                                 	
#define SS_NBR_PLAN_PRIVATE                               	0x09                                                 	

//ConstantTable for SS_NUMBER_TYPE
#define SS_NBR_TYPE_UNKNOWN                               	0x00                                                 	
#define SS_NBR_TYPE_INTERNATIONAL                         	0x01                                                 	
#define SS_NBR_TYPE_NATIONAL                              	0x02                                                 	
#define SS_NBR_TYPE_NETWORK_SPECIFIC                      	0x03                                                 	
#define SS_NBR_TYPE_SUBSCRIBER                            	0x04                                                 	
#define SS_NBR_TYPE_ALPHANUMERIC                          	0x05                                                 	
#define SS_NBR_TYPE_ABBREVIATED                           	0x06                                                 	

//ConstantTable for SS_MESSAGE_SENDER_INFORMATION
#define SS_SEND_DECODED_DATA                              	0x00                                                 	
#define SS_SEND_ADDITIONAL_INFO                           	0x01                                                 	

//ConstantTable for SS_MM_CAUSE_VALUES
#define SS_GSM_MM_CAUSE_NO_CAUSE                          	0x00                                                 	
#define SS_GSM_MM_CAUSE_HLR_IMSI_UNKNOWN                  	0x02                                                 	
#define SS_GSM_MM_CAUSE_ILLEGAL_MS                        	0x03                                                 	
#define SS_GSM_MM_CAUSE_VLR_IMSI_UNKNOWN                  	0x04                                                 	
#define SS_GSM_MM_CAUSE_IMEI_NOT_ACCEPTED                 	0x05                                                 	
#define SS_GSM_MM_CAUSE_ILLEGAL_ME                        	0x06                                                 	
#define SS_GSM_MM_CAUSE_PLMN_NOT_ALLOWED                  	0x0B                                                 	
#define SS_GSM_MM_CAUSE_LA_NOT_ALLOWED                    	0x0C                                                 	
#define SS_GSM_MM_CAUSE_ROAM_NOT_ALLOWED_LA               	0x0D                                                 	
#define SS_GSM_MM_CAUSE_NO_SUITABLE_CELLS_LA              	0x0F                                                 	
#define SS_GSM_MM_CAUSE_NETWORK_FAILURE                   	0x11                                              	
#define SS_GSM_MM_CAUSE_MAC_FAILURE                       	0x14                                              	
#define SS_GSM_MM_CAUSE_SYNC_FAILURE                      	0x15                                              	
#define SS_GSM_MM_CAUSE_CONGESTION                        	0x16                                              	
#define SS_GSM_MM_CAUSE_AUTH_UNACCEPTABLE                 	0x17                                              	
#define SS_GSM_MM_CAUSE_NOT_SUPP_S_OPTION                 	0x20                                              	
#define SS_GSM_MM_CAUSE_NOT_SUBS_S_OPTION                 	0x21                                              	
#define SS_GSM_MM_CAUSE_OUT_OF_ORDER_S_OPTION             	0x22                                              	
#define SS_GSM_MM_CAUSE_CALL_NOT_IDENTIFIED               	0x26                                              	
#define SS_GSM_MM_CAUSE_SEMANT_INCORRECT                  	0x5F                                              	
#define SS_GSM_MM_CAUSE_INV_MAND_INFORMATION              	0x60                                              	
#define SS_GSM_MM_CAUSE_NON_EXISTENT_MSG_TYPE             	0x61                                              	
#define SS_GSM_MM_CAUSE_NOT_COMP_MSG_TYPE                 	0x62                                              	
#define SS_GSM_MM_CAUSE_NOT_EXISTENT_IE                   	0x63                                              	
#define SS_GSM_MM_CAUSE_CONDITIONAL_IE_ERROR              	0x64                                              	
#define SS_GSM_MM_CAUSE_MSG_NOT_COMPATIBLE                	0x65                                              	
#define SS_GSM_MM_CAUSE_PROTOCOL_ERROR                    	0x6F                                              	

//ConstantTable for SS_FDN_CHECK_SUPPRESS_VALUES
#define SS_NO_FDN_CHECK_SUPPRESS                          	0x01                                                 	
#define SS_FDN_CHECK_SUPPRESS                             	0x02                                                 	

//ConstantTable for SS_RESOURCE_CONTROL_SUPPRESS_VALUES
#define SS_NO_RESOURCE_CONTROL_SUPPRESS                   	0x01                                                 	
#define SS_RESOURCE_CONTROL_SUPPRESS                      	0x02                                                 	

//ConstantTable for SS_ERROR_ADDITIONAL_INDICATION
#define SS_DEFAULT_VALUE                                  	0x00                                                 	

//BitmaskTables for SS_RESOURCE_IDS
#define SS_RES_ID_MO_SS_OPERATION                         	0x01                                                 	

//BitmaskTables for SS_RESOURCE_IDS_MASK
#define SS_RES_ID_MO_SS_OPERATION_MASK                    	0x01                                                 	

//BitmaskTables for SS_RESOURCE_IDS_CONF_REQ
#define SS_RES_ID_CONF_MO_SS_OPERATION                    	0x01                                                 	
#define SS_RES_ID_CONF_NOT_REQUIRED                       	0x00                                                 	

//ConstantTable for SS_RESOURCE_STATUS
#define SS_RESOURCE_ALLOWED                               	0x01                                                 	
#define SS_RESOURCE_DENIED                                	0x02                                                 	

//ConstantTable for SS_RESOURCE_CONF_OPERATION
#define SS_RESOURCE_CONF_GET                              	0x01                                                 	
#define SS_RESOURCE_CONF_SET                              	0x02                                                 	

//ConstantTable for SS_RESOURCE_CONF_STATUS
#define SS_RESOURCE_CONF_READY                            	0x01                                                 	
#define SS_RESOURCE_CONF_RECONFIGURED                     	0x02                                                 	

//ConstantTable for SS_RELEASE_REQ_RESULT
#define SS_RELEASE_OPERATION_OK                           	0x01                                                 	
#define SS_RELEASE_OPERATION_FAIL                         	0x02                                                 	


//SUBBLOCKS


//Definition for SS_FORWARDING
#define SS_FORWARDING                                     	0x00                                                 	
#define SS_FORWARDING_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_TYPEOFNUMBER                 	2 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_NOREPLYTIME                  	3 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_NUMBERLENGTH                 	4 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_SUBADDRESSLENGTH             	5 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_NUMBER                       	6 //size 2 byte(s)
#define SS_FORWARDING_OFFSET_SUBADDRESS                   	8 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_FILLERBYTE1                  	9 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_FILLERBYTE2                  	10 //size 1 byte(s)
#define SS_FORWARDING_OFFSET_FILLERBYTE3                  	11 //size 1 byte(s)
#define SIZE_SS_FORWARDING                                	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_GSM_PASSWORD
#define SS_GSM_PASSWORD                                   	0x03                                                 	
#define SS_GSM_PASSWORD_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define SS_GSM_PASSWORD_OFFSET_SUBBLOCKLENGTH             	1 //size 1 byte(s)
#define SS_GSM_PASSWORD_OFFSET_OLDPASSWORD                	2 //size 8 byte(s)
#define SS_GSM_PASSWORD_OFFSET_NEWPASSWORD1               	10 //size 8 byte(s)
#define SS_GSM_PASSWORD_OFFSET_NEWPASSWORD2               	18 //size 8 byte(s)
#define SS_GSM_PASSWORD_OFFSET_FILLERBYTE1                	26 //size 2 byte(s)
#define SIZE_SS_GSM_PASSWORD                              	28
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for SS_STATUS_RESULT
#define SS_STATUS_RESULT                                  	0x01                                                 	
#define SS_STATUS_RESULT_OFFSET_SUBBLOCKID                	0 //size 1 byte(s)
#define SS_STATUS_RESULT_OFFSET_SUBBLOCKLENGTH            	1 //size 1 byte(s)
#define SS_STATUS_RESULT_OFFSET_SSSTATUS                  	2 //size 1 byte(s)
#define SS_STATUS_RESULT_OFFSET_FILLERBYTE1               	3 //size 1 byte(s)
#define SIZE_SS_STATUS_RESULT                             	4


//Definition for SS_GSM_FORWARDING_FEATURE
#define SS_GSM_FORWARDING_FEATURE                         	0x05                                                 	
#define SS_GSM_FORWARDING_FEATURE_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_SUBBLOCKLENGTH   	1 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_BASICSERVICECODE 	2 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_SSSTATUS         	3 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_TYPEOFNUMBER     	4 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_NOREPLYTIME      	5 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_FORWARDINGOPTION 	6 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_NUMBERLENGTH     	7 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_SUBADDRESSLENGTH 	8 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_FILLERBYTE1      	9 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_NUMBER           	10 //size 2 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_SUBADDRESS       	12 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_FILLERBYTE2      	13 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_FILLERBYTE3      	14 //size 1 byte(s)
#define SS_GSM_FORWARDING_FEATURE_OFFSET_FILLERBYTE4      	15 //size 1 byte(s)
#define SIZE_SS_GSM_FORWARDING_FEATURE                    	16
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_GSM_FORWARDING_INFO
#define SS_GSM_FORWARDING_INFO                            	0x04                                                 	
#define SS_GSM_FORWARDING_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define SS_GSM_FORWARDING_INFO_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define SS_GSM_FORWARDING_INFO_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define SS_GSM_FORWARDING_INFO_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_SS_GSM_FORWARDING_INFO                       	4


//Definition for SS_GSM_BARRING_FEATURE
#define SS_GSM_BARRING_FEATURE                            	0x07                                                 	
#define SS_GSM_BARRING_FEATURE_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define SS_GSM_BARRING_FEATURE_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define SS_GSM_BARRING_FEATURE_OFFSET_BASICSERVICECODE    	2 //size 1 byte(s)
#define SS_GSM_BARRING_FEATURE_OFFSET_SSSTATUS            	3 //size 1 byte(s)
#define SIZE_SS_GSM_BARRING_FEATURE                       	4


//Definition for SS_GSM_BARRING_INFO
#define SS_GSM_BARRING_INFO                               	0x06                                                 	
#define SS_GSM_BARRING_INFO_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define SS_GSM_BARRING_INFO_OFFSET_SUBBLOCKLENGTH         	1 //size 1 byte(s)
#define SS_GSM_BARRING_INFO_OFFSET_FILLERBYTE1            	2 //size 1 byte(s)
#define SS_GSM_BARRING_INFO_OFFSET_SUBBLOCKCOUNT          	3 //size 1 byte(s)
#define SIZE_SS_GSM_BARRING_INFO                          	4


//Definition for SS_GSM_BSC_INFO
#define SS_GSM_BSC_INFO                                   	0x09                                                 	
#define SS_GSM_BSC_INFO_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define SS_GSM_BSC_INFO_OFFSET_SUBBLOCKLENGTH             	1 //size 1 byte(s)
#define SS_GSM_BSC_INFO_OFFSET_NUMBEROFBSC                	2 //size 1 byte(s)
#define SS_GSM_BSC_INFO_OFFSET_BASICSERVICECODE           	3 //size 1 byte(s)
#define SIZE_SS_GSM_BSC_INFO                              	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_GSM_DATA
#define SS_GSM_DATA                                       	0x08                                                 	
#define SS_GSM_DATA_OFFSET_SUBBLOCKID                     	0 //size 1 byte(s)
#define SS_GSM_DATA_OFFSET_SUBBLOCKLENGTH                 	1 //size 1 byte(s)
#define SS_GSM_DATA_OFFSET_SSSTATUS                       	2 //size 1 byte(s)
#define SS_GSM_DATA_OFFSET_SUBSCRIPTIONOPTION             	3 //size 1 byte(s)
#define SS_GSM_DATA_OFFSET_FILLERBYTE1                    	4 //size 3 byte(s)
#define SS_GSM_DATA_OFFSET_SUBBLOCKCOUNT                  	7 //size 1 byte(s)
#define SIZE_SS_GSM_DATA                                  	8


//Definition for SS_GSM_CLIR_INFO
#define SS_GSM_CLIR_INFO                                  	0x0C                                                 	
#define SS_GSM_CLIR_INFO_OFFSET_SUBBLOCKID                	0 //size 1 byte(s)
#define SS_GSM_CLIR_INFO_OFFSET_SUBBLOCKLENGTH            	1 //size 1 byte(s)
#define SS_GSM_CLIR_INFO_OFFSET_CLIRESTRICTIONOPTION      	2 //size 1 byte(s)
#define SS_GSM_CLIR_INFO_OFFSET_FILLERBYTE1               	3 //size 1 byte(s)
#define SIZE_SS_GSM_CLIR_INFO                             	4


//Definition for SS_GSM_GENERIC_SERVICE_INFO
#define SS_GSM_GENERIC_SERVICE_INFO                       	0x0A                                                 	
#define SS_GSM_GENERIC_SERVICE_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define SS_GSM_GENERIC_SERVICE_INFO_OFFSET_SUBBLOCKLENGTH 	1 //size 1 byte(s)
#define SS_GSM_GENERIC_SERVICE_INFO_OFFSET_SSSTATUS       	2 //size 1 byte(s)
#define SS_GSM_GENERIC_SERVICE_INFO_OFFSET_SUBBLOCKCOUNT  	3 //size 1 byte(s)
#define SIZE_SS_GSM_GENERIC_SERVICE_INFO                  	4


//Definition for SS_GSM_PASSWORD_INFO
#define SS_GSM_PASSWORD_INFO                              	0x0B                                                 	
#define SS_GSM_PASSWORD_INFO_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define SS_GSM_PASSWORD_INFO_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define SS_GSM_PASSWORD_INFO_OFFSET_FILLERBYTE1           	2 //size 2 byte(s)
#define SS_GSM_PASSWORD_INFO_OFFSET_PASSWORD              	4 //size 4 byte(s)
#define SIZE_SS_GSM_PASSWORD_INFO                         	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for SS_GSM_INDICATE_PASSWORD_ERROR
#define SS_GSM_INDICATE_PASSWORD_ERROR                    	0x0D                                                 	
#define SS_GSM_INDICATE_PASSWORD_ERROR_OFFSET_SUBBLOCKID  	0 //size 1 byte(s)
#define SS_GSM_INDICATE_PASSWORD_ERROR_OFFSET_SUBBLOCKLENGTH	1 //size 1 byte(s)
#define SS_GSM_INDICATE_PASSWORD_ERROR_OFFSET_GUIDANCEINFO	2 //size 1 byte(s)
#define SS_GSM_INDICATE_PASSWORD_ERROR_OFFSET_FILLERBYTE1 	3 //size 1 byte(s)
#define SIZE_SS_GSM_INDICATE_PASSWORD_ERROR               	4


//Definition for SS_GSM_INDICATE_ERROR
#define SS_GSM_INDICATE_ERROR                             	0x0E                                                 	
#define SS_GSM_INDICATE_ERROR_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define SS_GSM_INDICATE_ERROR_OFFSET_SUBBLOCKLENGTH       	1 //size 1 byte(s)
#define SS_GSM_INDICATE_ERROR_OFFSET_ERRORCODE            	2 //size 1 byte(s)
#define SS_GSM_INDICATE_ERROR_OFFSET_ADDITIONALINDICATION 	3 //size 1 byte(s)
#define SIZE_SS_GSM_INDICATE_ERROR                        	4


//Definition for SS_GSM_INDICATE_PROBLEM
#define SS_GSM_INDICATE_PROBLEM                           	0x0F                                                 	
#define SS_GSM_INDICATE_PROBLEM_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define SS_GSM_INDICATE_PROBLEM_OFFSET_SUBBLOCKLENGTH     	1 //size 1 byte(s)
#define SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMTYPE        	2 //size 1 byte(s)
#define SS_GSM_INDICATE_PROBLEM_OFFSET_PROBLEMCODE        	3 //size 1 byte(s)
#define SIZE_SS_GSM_INDICATE_PROBLEM                      	4


//Definition for SS_GSM_INDICATE_MSG_ERROR
#define SS_GSM_INDICATE_MSG_ERROR                         	0x10                                              	
#define SS_GSM_INDICATE_MSG_ERROR_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define SS_GSM_INDICATE_MSG_ERROR_OFFSET_SUBBLOCKLENGTH   	1 //size 1 byte(s)
#define SS_GSM_INDICATE_MSG_ERROR_OFFSET_MESSAGEERRORCODE 	2 //size 1 byte(s)
#define SS_GSM_INDICATE_MSG_ERROR_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_SS_GSM_INDICATE_MSG_ERROR                    	4


//Definition for SS_OTHER_ERROR
#define SS_OTHER_ERROR                                    	0x02                                                 	
#define SS_OTHER_ERROR_OFFSET_SUBBLOCKID                  	0 //size 1 byte(s)
#define SS_OTHER_ERROR_OFFSET_SUBBLOCKLENGTH              	1 //size 1 byte(s)
#define SS_OTHER_ERROR_OFFSET_ERRORCODE                   	2 //size 1 byte(s)
#define SS_OTHER_ERROR_OFFSET_FILLERBYTE1                 	3 //size 1 byte(s)
#define SIZE_SS_OTHER_ERROR                               	4


//Definition for SS_GSM_ADDITIONAL_INFO
#define SS_GSM_ADDITIONAL_INFO                            	0x2F                                              	
#define SS_GSM_ADDITIONAL_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define SS_GSM_ADDITIONAL_INFO_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define SS_GSM_ADDITIONAL_INFO_OFFSET_RETURNRESULTLEN     	2 //size 1 byte(s)
#define SS_GSM_ADDITIONAL_INFO_OFFSET_RETURNRESULT        	3 //size 1 byte(s)
#define SIZE_SS_GSM_ADDITIONAL_INFO                       	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_GSM_MM_RELEASED
#define SS_GSM_MM_RELEASED                                	0x30                                              	
#define SS_GSM_MM_RELEASED_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define SS_GSM_MM_RELEASED_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define SS_GSM_MM_RELEASED_OFFSET_CAUSE                   	2 //size 1 byte(s)
#define SS_GSM_MM_RELEASED_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_SS_GSM_MM_RELEASED                           	4


//Definition for SS_GSM_USSD_STRING
#define SS_GSM_USSD_STRING                                	0x32                                              	
#define SS_GSM_USSD_STRING_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define SS_GSM_USSD_STRING_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define SS_GSM_USSD_STRING_OFFSET_CODINGINFO              	2 //size 1 byte(s)
#define SS_GSM_USSD_STRING_OFFSET_LENGTHOFUSSDSTRING      	3 //size 1 byte(s)
#define SS_GSM_USSD_STRING_OFFSET_USSDSTRING              	4 //size 1 byte(s)
#define SS_GSM_USSD_STRING_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define SS_GSM_USSD_STRING_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define SS_GSM_USSD_STRING_OFFSET_FILLERBYTE3             	7 //size 1 byte(s)
#define SIZE_SS_GSM_USSD_STRING                           	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_SB_RESOURCE
#define SS_SB_RESOURCE                                    	0x41                                              	
#define SS_SB_RESOURCE_OFFSET_SUBBLOCKID                  	0 //size 1 byte(s)
#define SS_SB_RESOURCE_OFFSET_SUBBLOCKLEN                 	1 //size 1 byte(s)
#define SS_SB_RESOURCE_OFFSET_RES                         	2 //size 2 byte(s)
#define SIZE_SS_SB_RESOURCE                               	4


//Definition for SS_SB_RESOURCE_SEQ_ID
#define SS_SB_RESOURCE_SEQ_ID                             	0x42                                              	
#define SS_SB_RESOURCE_SEQ_ID_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define SS_SB_RESOURCE_SEQ_ID_OFFSET_SUBBLOCKLEN          	1 //size 1 byte(s)
#define SS_SB_RESOURCE_SEQ_ID_OFFSET_SEQUENCEID           	2 //size 1 byte(s)
#define SS_SB_RESOURCE_SEQ_ID_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_SS_SB_RESOURCE_SEQ_ID                        	4


//Definition for SS_SB_SS_CONTROL
#define SS_SB_SS_CONTROL                                  	0x44                                              	
#define SS_SB_SS_CONTROL_OFFSET_SUBBLOCKID                	0 //size 1 byte(s)
#define SS_SB_SS_CONTROL_OFFSET_SUBBLOCKLEN               	1 //size 1 byte(s)
#define SS_SB_SS_CONTROL_OFFSET_SSSTRINGLENGTH            	2 //size 1 byte(s)
#define SS_SB_SS_CONTROL_OFFSET_SSSTRING                  	3 //size 1 byte(s)
#define SIZE_SS_SB_SS_CONTROL                             	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for SS_SB_USSD_CONTROL
#define SS_SB_USSD_CONTROL                                	0x45                                              	
#define SS_SB_USSD_CONTROL_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define SS_SB_USSD_CONTROL_OFFSET_SUBBLOCKLEN             	1 //size 1 byte(s)
#define SS_SB_USSD_CONTROL_OFFSET_CODINGINFO              	2 //size 1 byte(s)
#define SS_SB_USSD_CONTROL_OFFSET_USSDSTRINGLENGTH        	3 //size 1 byte(s)
#define SS_SB_USSD_CONTROL_OFFSET_USSDSTRING              	4 //size 1 byte(s)
#define SS_SB_USSD_CONTROL_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define SS_SB_USSD_CONTROL_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define SS_SB_USSD_CONTROL_OFFSET_FILLERBYTE3             	7 //size 1 byte(s)
#define SIZE_SS_SB_USSD_CONTROL                           	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_SB_RESOURCE_STATUS
#define SS_SB_RESOURCE_STATUS                             	0x43                                              	
#define SS_SB_RESOURCE_STATUS_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define SS_SB_RESOURCE_STATUS_OFFSET_SUBBLOCKLEN          	1 //size 1 byte(s)
#define SS_SB_RESOURCE_STATUS_OFFSET_RESSTATUS            	2 //size 1 byte(s)
#define SS_SB_RESOURCE_STATUS_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_SS_SB_RESOURCE_STATUS                        	4


//Definition for SS_SB_RESOURCE_CONTROL_INFO
#define SS_SB_RESOURCE_CONTROL_INFO                       	0x46                                              	
#define SS_SB_RESOURCE_CONTROL_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define SS_SB_RESOURCE_CONTROL_INFO_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define SS_SB_RESOURCE_CONTROL_INFO_OFFSET_DATALENGTH     	2 //size 1 byte(s)
#define SS_SB_RESOURCE_CONTROL_INFO_OFFSET_DATA           	3 //size 1 byte(s)
#define SIZE_SS_SB_RESOURCE_CONTROL_INFO                  	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_SB_CHECK_INFO
#define SS_SB_CHECK_INFO                                  	0x47                                              	
#define SS_SB_CHECK_INFO_OFFSET_SUBBLOCKID                	0 //size 1 byte(s)
#define SS_SB_CHECK_INFO_OFFSET_SUBBLOCKLENGTH            	1 //size 1 byte(s)
#define SS_SB_CHECK_INFO_OFFSET_FDNSUPPRESSSTATUS         	2 //size 1 byte(s)
#define SS_SB_CHECK_INFO_OFFSET_RESOURCECONTROLSTATUS     	3 //size 1 byte(s)
#define SIZE_SS_SB_CHECK_INFO                             	4


//Definition for SS_SB_RESOURCE_CONF_REQUIRED
#define SS_SB_RESOURCE_CONF_REQUIRED                      	0x48                                              	
#define SS_SB_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKID    	0 //size 1 byte(s)
#define SS_SB_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKLEN   	1 //size 1 byte(s)
#define SS_SB_RESOURCE_CONF_REQUIRED_OFFSET_RESPP         	2 //size 2 byte(s)
#define SIZE_SS_SB_RESOURCE_CONF_REQUIRED                 	4


//Definition for SS_SB_RESOURCE_CONF
#define SS_SB_RESOURCE_CONF                               	0x49                                              	
#define SS_SB_RESOURCE_CONF_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define SS_SB_RESOURCE_CONF_OFFSET_SUBBLOCKLEN            	1 //size 1 byte(s)
#define SS_SB_RESOURCE_CONF_OFFSET_RES                    	2 //size 2 byte(s)
#define SS_SB_RESOURCE_CONF_OFFSET_RESMASK                	4 //size 2 byte(s)
#define SS_SB_RESOURCE_CONF_OFFSET_FILLERBYTE1            	6 //size 1 byte(s)
#define SS_SB_RESOURCE_CONF_OFFSET_FILLERBYTE2            	7 //size 1 byte(s)
#define SIZE_SS_SB_RESOURCE_CONF                          	8


//MESSAGES


//Definition for SS_SERVICE_REQ
#define SS_SERVICE_REQ                                    	0x00                                                 	
#define SS_SERVICE_REQ_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define SS_SERVICE_REQ_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define SS_SERVICE_REQ_OFFSET_OPERATION                   	2 //size 1 byte(s)
#define SS_SERVICE_REQ_OFFSET_BASICSERVICECODE            	3 //size 1 byte(s)
#define SS_SERVICE_REQ_OFFSET_SSCODE                      	4 //size 2 byte(s)
#define SS_SERVICE_REQ_OFFSET_RESPONSEDATATOSEND          	6 //size 1 byte(s)
#define SS_SERVICE_REQ_OFFSET_SUBBLOCKCOUNT               	7 //size 1 byte(s)
#define SIZE_SS_SERVICE_REQ                               	8


//Definition for SS_SERVICE_COMPLETED_RESP
#define SS_SERVICE_COMPLETED_RESP                         	0x01                                                 	
#define SS_SERVICE_COMPLETED_RESP_OFFSET_TRANSID          	0 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_RESP_OFFSET_OPERATION        	2 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_RESP_OFFSET_BASICSERVICECODE 	3 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_RESP_OFFSET_SSCODE           	4 //size 2 byte(s)
#define SS_SERVICE_COMPLETED_RESP_OFFSET_FILLERBYTE1      	6 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_RESP_OFFSET_SUBBLOCKCOUNT    	7 //size 1 byte(s)
#define SIZE_SS_SERVICE_COMPLETED_RESP                    	8


//Definition for SS_SERVICE_COMPLETED_IND
#define SS_SERVICE_COMPLETED_IND                          	0x10                                              	
#define SS_SERVICE_COMPLETED_IND_OFFSET_TRANSID           	0 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_IND_OFFSET_OPERATION         	2 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_IND_OFFSET_BASICSERVICECODE  	3 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_IND_OFFSET_SSCODE            	4 //size 2 byte(s)
#define SS_SERVICE_COMPLETED_IND_OFFSET_FILLERBYTE1       	6 //size 1 byte(s)
#define SS_SERVICE_COMPLETED_IND_OFFSET_SUBBLOCKCOUNT     	7 //size 1 byte(s)
#define SIZE_SS_SERVICE_COMPLETED_IND                     	8


//Definition for SS_SERVICE_FAILED_RESP
#define SS_SERVICE_FAILED_RESP                            	0x02                                                 	
#define SS_SERVICE_FAILED_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define SS_SERVICE_FAILED_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define SS_SERVICE_FAILED_RESP_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define SS_SERVICE_FAILED_RESP_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_SS_SERVICE_FAILED_RESP                       	4


//Definition for SS_SERVICE_NOT_SUPPORTED_RESP
#define SS_SERVICE_NOT_SUPPORTED_RESP                     	0x03                                                 	
#define SS_SERVICE_NOT_SUPPORTED_RESP_OFFSET_TRANSID      	0 //size 1 byte(s)
#define SS_SERVICE_NOT_SUPPORTED_RESP_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define SS_SERVICE_NOT_SUPPORTED_RESP_OFFSET_REASON       	2 //size 1 byte(s)
#define SS_SERVICE_NOT_SUPPORTED_RESP_OFFSET_FILLERBYTE1  	3 //size 1 byte(s)
#define SIZE_SS_SERVICE_NOT_SUPPORTED_RESP                	4


//Definition for SS_GSM_USSD_SEND_REQ
#define SS_GSM_USSD_SEND_REQ                              	0x04                                                 	
#define SS_GSM_USSD_SEND_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define SS_GSM_USSD_SEND_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SS_GSM_USSD_SEND_REQ_OFFSET_USSDTYPE              	2 //size 1 byte(s)
#define SS_GSM_USSD_SEND_REQ_OFFSET_SUBBLOCKCOUNT         	3 //size 1 byte(s)
#define SIZE_SS_GSM_USSD_SEND_REQ                         	4


//Definition for SS_GSM_USSD_SEND_RESP
#define SS_GSM_USSD_SEND_RESP                             	0x05                                                 	
#define SS_GSM_USSD_SEND_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define SS_GSM_USSD_SEND_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SS_GSM_USSD_SEND_RESP_OFFSET_CODINGINFO           	2 //size 1 byte(s)
#define SS_GSM_USSD_SEND_RESP_OFFSET_USSDTYPE             	3 //size 1 byte(s)
#define SS_GSM_USSD_SEND_RESP_OFFSET_LENGTHOFUSSDSTRING   	4 //size 1 byte(s)
#define SS_GSM_USSD_SEND_RESP_OFFSET_USSDSTRING           	5 //size 1 byte(s)
#define SS_GSM_USSD_SEND_RESP_OFFSET_FILLERBYTE1          	6 //size 1 byte(s)
#define SS_GSM_USSD_SEND_RESP_OFFSET_FILLERBYTE2          	7 //size 1 byte(s)
#define SIZE_SS_GSM_USSD_SEND_RESP                        	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_GSM_USSD_RECEIVE_IND
#define SS_GSM_USSD_RECEIVE_IND                           	0x06                                                 	
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_TRANSID            	0 //size 1 byte(s)
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_CODINGINFO         	2 //size 1 byte(s)
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_USSDTYPE           	3 //size 1 byte(s)
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_LENGTHOFUSSDSTRING 	4 //size 1 byte(s)
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_USSDSTRING         	5 //size 1 byte(s)
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_FILLERBYTE1        	6 //size 1 byte(s)
#define SS_GSM_USSD_RECEIVE_IND_OFFSET_FILLERBYTE2        	7 //size 1 byte(s)
#define SIZE_SS_GSM_USSD_RECEIVE_IND                      	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SS_STATUS_IND
#define SS_STATUS_IND                                     	0x09                                                 	
#define SS_STATUS_IND_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define SS_STATUS_IND_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define SS_STATUS_IND_OFFSET_SSSTATUSINDICATION           	2 //size 1 byte(s)
#define SS_STATUS_IND_OFFSET_SUBBLOCKCOUNT                	3 //size 1 byte(s)
#define SIZE_SS_STATUS_IND                                	4


//Definition for SS_CANCEL_REQ
#define SS_CANCEL_REQ                                     	0x11                                              	
#define SS_CANCEL_REQ_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define SS_CANCEL_REQ_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define SS_CANCEL_REQ_OFFSET_FILLERBYTE1                  	2 //size 2 byte(s)
#define SIZE_SS_CANCEL_REQ                                	4


//Definition for SS_CANCEL_RESP
#define SS_CANCEL_RESP                                    	0x12                                              	
#define SS_CANCEL_RESP_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define SS_CANCEL_RESP_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define SS_CANCEL_RESP_OFFSET_FILLERBYTE1                 	2 //size 2 byte(s)
#define SIZE_SS_CANCEL_RESP                               	4


//Definition for SS_RESOURCE_CONTROL_IND
#define SS_RESOURCE_CONTROL_IND                           	0x21                                              	
#define SS_RESOURCE_CONTROL_IND_OFFSET_TRANSID            	0 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_IND_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_IND_OFFSET_FILLERBYTE1        	2 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_IND_OFFSET_SUBBLOCKCOUNT      	3 //size 1 byte(s)
#define SIZE_SS_RESOURCE_CONTROL_IND                      	4


//Definition for SS_RESOURCE_CONTROL_REQ
#define SS_RESOURCE_CONTROL_REQ                           	0x22                                              	
#define SS_RESOURCE_CONTROL_REQ_OFFSET_TRANSID            	0 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_REQ_OFFSET_FILLERBYTE1        	2 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_REQ_OFFSET_SUBBLOCKCOUNT      	3 //size 1 byte(s)
#define SIZE_SS_RESOURCE_CONTROL_REQ                      	4


//Definition for SS_RESOURCE_CONTROL_RESP
#define SS_RESOURCE_CONTROL_RESP                          	0x23                                              	
#define SS_RESOURCE_CONTROL_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_RESP_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define SS_RESOURCE_CONTROL_RESP_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_SS_RESOURCE_CONTROL_RESP                     	4


//Definition for SS_RESOURCE_CONF_REQ
#define SS_RESOURCE_CONF_REQ                              	0x25                                              	
#define SS_RESOURCE_CONF_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define SS_RESOURCE_CONF_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SS_RESOURCE_CONF_REQ_OFFSET_CONFOPERATION         	2 //size 1 byte(s)
#define SS_RESOURCE_CONF_REQ_OFFSET_SUBBLOCKCOUNT         	3 //size 1 byte(s)
#define SIZE_SS_RESOURCE_CONF_REQ                         	4


//Definition for SS_RESOURCE_CONF_RESP
#define SS_RESOURCE_CONF_RESP                             	0x26                                              	
#define SS_RESOURCE_CONF_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define SS_RESOURCE_CONF_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SS_RESOURCE_CONF_RESP_OFFSET_CONFOPERATION        	2 //size 1 byte(s)
#define SS_RESOURCE_CONF_RESP_OFFSET_SUBBLOCKCOUNT        	3 //size 1 byte(s)
#define SIZE_SS_RESOURCE_CONF_RESP                        	4


//Definition for SS_RESOURCE_CONF_IND
#define SS_RESOURCE_CONF_IND                              	0x24                                              	
#define SS_RESOURCE_CONF_IND_OFFSET_TRANSID               	0 //size 1 byte(s)
#define SS_RESOURCE_CONF_IND_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SS_RESOURCE_CONF_IND_OFFSET_CONFSTATUS            	2 //size 1 byte(s)
#define SS_RESOURCE_CONF_IND_OFFSET_SUBBLOCKS             	3 //size 1 byte(s)
#define SIZE_SS_RESOURCE_CONF_IND                         	4


//Definition for SS_RELEASE_REQ
#define SS_RELEASE_REQ                                    	0x15                                              	
#define SS_RELEASE_REQ_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define SS_RELEASE_REQ_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define SS_RELEASE_REQ_OFFSET_REQTRID                     	2 //size 1 byte(s)
#define SS_RELEASE_REQ_OFFSET_FILLERBYTE1                 	3 //size 1 byte(s)
#define SIZE_SS_RELEASE_REQ                               	4


//Definition for SS_RELEASE_RESP
#define SS_RELEASE_RESP                                   	0x16                                              	
#define SS_RELEASE_RESP_OFFSET_TRANSID                    	0 //size 1 byte(s)
#define SS_RELEASE_RESP_OFFSET_MESSAGEID                  	1 //size 1 byte(s)
#define SS_RELEASE_RESP_OFFSET_RELEASERESULT              	2 //size 1 byte(s)
#define SS_RELEASE_RESP_OFFSET_FILLERBYTE1                	3 //size 1 byte(s)
#define SIZE_SS_RELEASE_RESP                              	4

#endif