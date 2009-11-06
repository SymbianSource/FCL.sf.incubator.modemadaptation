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
* Name      : PN_MODEM_CALL [ 0xC9 ] Resource Modem Call Server
* Version   : 001.004
*
*/


#ifndef _SYMBIAN_ISI_CALL_MODEMISI_H_
#define _SYMBIAN_ISI_CALL_MODEMISI_H_

#define MODEM_CALL_SERVER_ISI_VERSION_Z	 1
#define MODEM_CALL_SERVER_ISI_VERSION_Y	 4


//CONSTANTS
#define CALL_MODEM_CUG_DEFAULT                            	0xFFFF                                            	
#define CALL_MODEM_DTMF_P_DIGIT                           	0x70                                              	
#define CALL_MODEM_DTMF_W_DIGIT                           	0x77                                              	
#define CALL_MODEM_DTMF_NO_TONE                           	0x00                                                 	
#define CALL_MODEM_NO_LINK_ID                             	0x00                                                 	
#define CALL_MODEM_DTMF_DEFAULT                           	0x00                                                 	
#define CALL_MODEM_DTMF_NO_DELAY                          	0x01                                                 	


//TABLES

//BitmaskTables for CALL_MODEM_ADDRESS_TYPE
#define CALL_MODEM_NBR_PLAN_UNKNOWN                       	0x00                                                 	
#define CALL_MODEM_NBR_PLAN_ISDN_TELEPHONY                	0x01                                                 	
#define CALL_MODEM_NBR_PLAN_DATA                          	0x03                                                 	
#define CALL_MODEM_NBR_PLAN_TELEX                         	0x04                                                 	
#define CALL_MODEM_NBR_PLAN_PRIVATE                       	0x09                                                 	
#define CALL_MODEM_NBR_PLAN_NATIONAL                      	0x08                                                 	
#define CALL_MODEM_NBR_TYPE_UNKNOWN                       	0x00                                                 	
#define CALL_MODEM_NBR_TYPE_INTERNATIONAL                 	0x10                                              	
#define CALL_MODEM_NBR_TYPE_NATIONAL                      	0x20                                              	
#define CALL_MODEM_NBR_TYPE_NETWORK_SPECIFIC              	0x30                                              	

//BitmaskTables for CALL_MODEM_PRESENTATION_SCREEN_INDICATOR
#define CALL_MODEM_PRESENTATION_ALLOWED                   	0x00                                                 	
#define CALL_MODEM_PRESENTATION_RESTRICTED                	0x20                                              	
#define CALL_MODEM_PRESENTATION_UNAVAILABLE               	0x40                                              	
#define CALL_MODEM_USER_NOT_SCREENED                      	0x00                                                 	
#define CALL_MODEM_USER_SCREENED_PASSED                   	0x01                                                 	
#define CALL_MODEM_USER_SCREENED_FAILED                   	0x02                                                 	
#define CALL_MODEM_SCREEN_NETW_PROVIDED                   	0x03                                                 	

//ConstantTable for CALL_MODEM_MODE
#define CALL_MODEM_MODE_EMERGENCY                         	0x00                                                 	
#define CALL_MODEM_MODE_SPEECH                            	0x01                                                 	
#define CALL_MODEM_MODE_ALS_LINE_2                        	0xA2                                              	
#define CALL_MODEM_MODE_MULTIMEDIA                        	0xA3                                              	

//BitmaskTables for CALL_MODEM_MODE_INFO
#define CALL_MODEM_MODE_ORIGINATOR                        	0x01                                                 	

//ConstantTable for CALL_MODEM_UUS_TYPE
#define CALL_MODEM_UUS_TYPE_UUS1                          	0x01                                                 	

//ConstantTable for CALL_MODEM_UUS_ACTIVATION_TYPE
#define CALL_MODEM_UUS_ACTIVATION_IMPLICIT                	0x01                                                 	

//BitmaskTables for CALL_MODEM_ID
#define CALL_MODEM_ID_NONE                                	0x00                                                 	
#define CALL_MODEM_ID_1                                   	0x01                                                 	
#define CALL_MODEM_ID_2                                   	0x02                                                 	
#define CALL_MODEM_ID_3                                   	0x03                                                 	
#define CALL_MODEM_ID_4                                   	0x04                                                 	
#define CALL_MODEM_ID_5                                   	0x05                                                 	
#define CALL_MODEM_ID_6                                   	0x06                                                 	
#define CALL_MODEM_ID_7                                   	0x07                                                 	
#define CALL_MODEM_ID_CONFERENCE                          	0x10                                              	
#define CALL_MODEM_ID_WAITING                             	0x20                                              	
#define CALL_MODEM_ID_HOLD                                	0x40                                              	
#define CALL_MODEM_ID_ACTIVE                              	0x80                                              	
#define CALL_MODEM_ID_ALL                                 	0xF0                                              	

//ConstantTable for CALL_MODEM_CAUSE_TYPE_SENDER
#define CALL_MODEM_CAUSE_TYPE_CLIENT                      	0x01                                                 	
#define CALL_MODEM_CAUSE_TYPE_SERVER                      	0x02                                                 	
#define CALL_MODEM_CAUSE_TYPE_NETWORK                     	0x03                                                 	

//ConstantTable for CALL_MODEM_CAUSE
#define CALL_MODEM_CAUSE_NO_CALL                          	0x01                                                 	
#define CALL_MODEM_CAUSE_RELEASE_BY_USER                  	0x03                                                 	
#define CALL_MODEM_CAUSE_BUSY_USER_REQUEST                	0x04                                                 	
#define CALL_MODEM_CAUSE_ERROR_REQUEST                    	0x05                                                 	
#define CALL_MODEM_CAUSE_CALL_ACTIVE                      	0x07                                                 	
#define CALL_MODEM_CAUSE_NO_CALL_ACTIVE                   	0x08                                                 	
#define CALL_MODEM_CAUSE_INVALID_CALL_MODE                	0x09                                                 	
#define CALL_MODEM_CAUSE_TOO_LONG_ADDRESS                 	0x0B                                                 	
#define CALL_MODEM_CAUSE_INVALID_ADDRESS                  	0x0C                                                 	
#define CALL_MODEM_CAUSE_EMERGENCY                        	0x0D                                                 	
#define CALL_MODEM_CAUSE_NO_SERVICE                       	0x0E                                                 	
#define CALL_MODEM_CAUSE_CODE_REQUIRED                    	0x10                                              	
#define CALL_MODEM_CAUSE_NOT_ALLOWED                      	0x11                                              	
#define CALL_MODEM_CAUSE_DTMF_ERROR                       	0x12                                              	
#define CALL_MODEM_CAUSE_CHANNEL_LOSS                     	0x13                                              	
#define CALL_MODEM_CAUSE_FDN_NOT_OK                       	0x14                                              	
#define CALL_MODEM_CAUSE_BLACKLIST_BLOCKED                	0x16                                              	
#define CALL_MODEM_CAUSE_BLACKLIST_DELAYED                	0x17                                              	
#define CALL_MODEM_CAUSE_EMERGENCY_FAILURE                	0x1A                                              	
#define CALL_MODEM_CAUSE_NO_SIM                           	0x1F                                              	
#define CALL_MODEM_CAUSE_DTMF_SEND_ONGOING                	0x24                                              	
#define CALL_MODEM_CAUSE_CS_INACTIVE                      	0x25                                              	
#define CALL_MODEM_CAUSE_NOT_READY                        	0x26                                              	
#define CALL_MODEM_CAUSE_INCOMPATIBLE_DEST                	0x27                                              	

//ConstantTable for CALL_MODEM_NW_CAUSE
#define CALL_MODEM_NW_CAUSE_UNASSIGNED_NUMBER             	0x01                                                 	
#define CALL_MODEM_NW_CAUSE_NO_ROUTE                      	0x03                                                 	
#define CALL_MODEM_NW_CAUSE_CH_UNACCEPTABLE               	0x06                                                 	
#define CALL_MODEM_NW_CAUSE_OPER_BARRING                  	0x08                                                 	
#define CALL_MODEM_NW_CAUSE_NORMAL                        	0x10                                              	
#define CALL_MODEM_NW_CAUSE_USER_BUSY                     	0x11                                              	
#define CALL_MODEM_NW_CAUSE_NO_USER_RESPONSE              	0x12                                              	
#define CALL_MODEM_NW_CAUSE_ALERT_NO_ANSWER               	0x13                                              	
#define CALL_MODEM_NW_CAUSE_CALL_REJECTED                 	0x15                                              	
#define CALL_MODEM_NW_CAUSE_NUMBER_CHANGED                	0x16                                              	
#define CALL_MODEM_NW_CAUSE_NON_SELECT_CLEAR              	0x1A                                              	
#define CALL_MODEM_NW_CAUSE_DEST_OUT_OF_ORDER             	0x1B                                              	
#define CALL_MODEM_NW_CAUSE_INVALID_NUMBER                	0x1C                                              	
#define CALL_MODEM_NW_CAUSE_FACILITY_REJECTED             	0x1D                                              	
#define CALL_MODEM_NW_CAUSE_RESP_TO_STATUS                	0x1E                                              	
#define CALL_MODEM_NW_CAUSE_NORMAL_UNSPECIFIED            	0x1F                                              	
#define CALL_MODEM_NW_CAUSE_NO_CHANNEL                    	0x22                                              	
#define CALL_MODEM_NW_CAUSE_NETW_OUT_OF_ORDER             	0x26                                              	
#define CALL_MODEM_NW_CAUSE_TEMPORARY_FAILURE             	0x29                                              	
#define CALL_MODEM_NW_CAUSE_CONGESTION                    	0x2A                                              	
#define CALL_MODEM_NW_CAUSE_ACCESS_INFO_DISC              	0x2B                                              	
#define CALL_MODEM_NW_CAUSE_CHANNEL_NA                    	0x2C                                              	
#define CALL_MODEM_NW_CAUSE_RESOURCES_NA                  	0x2F                                              	
#define CALL_MODEM_NW_CAUSE_QOS_NA                        	0x31                                              	
#define CALL_MODEM_NW_CAUSE_FACILITY_UNSUBS               	0x32                                              	
#define CALL_MODEM_NW_CAUSE_COMING_BARRED_CUG             	0x37                                              	
#define CALL_MODEM_NW_CAUSE_BC_UNAUTHORIZED               	0x39                                              	
#define CALL_MODEM_NW_CAUSE_BC_NA                         	0x3A                                              	
#define CALL_MODEM_NW_CAUSE_SERVICE_NA                    	0x3F                                              	
#define CALL_MODEM_NW_CAUSE_BEARER_NOT_IMPL               	0x41                                              	
#define CALL_MODEM_NW_CAUSE_ACM_MAX                       	0x44                                              	
#define CALL_MODEM_NW_CAUSE_FACILITY_NOT_IMPL             	0x45                                              	
#define CALL_MODEM_NW_CAUSE_ONLY_RDI_BC                   	0x46                                              	
#define CALL_MODEM_NW_CAUSE_SERVICE_NOT_IMPL              	0x4F                                              	
#define CALL_MODEM_NW_CAUSE_INVALID_TI                    	0x51                                              	
#define CALL_MODEM_NW_CAUSE_NOT_IN_CUG                    	0x57                                              	
#define CALL_MODEM_NW_CAUSE_INCOMPATIBLE_DEST             	0x58                                              	
#define CALL_MODEM_NW_CAUSE_INV_TRANS_NET_SEL             	0x5B                                              	
#define CALL_MODEM_NW_CAUSE_SEMANTICAL_ERR                	0x5F                                              	
#define CALL_MODEM_NW_CAUSE_INVALID_MANDATORY             	0x60                                              	
#define CALL_MODEM_NW_CAUSE_MSG_TYPE_INEXIST              	0x61                                              	
#define CALL_MODEM_NW_CAUSE_MSG_TYPE_INCOMPAT             	0x62                                              	
#define CALL_MODEM_NW_CAUSE_IE_NON_EXISTENT               	0x63                                              	
#define CALL_MODEM_NW_CAUSE_COND_IE_ERROR                 	0x64                                              	
#define CALL_MODEM_NW_CAUSE_MSG_INCOMPATIBLE              	0x65                                              	
#define CALL_MODEM_NW_CAUSE_TIMER_EXPIRY                  	0x66                                              	
#define CALL_MODEM_NW_CAUSE_PROTOCOL_ERROR                	0x6F                                              	
#define CALL_MODEM_NW_CAUSE_INTERWORKING                  	0x7F                                              	

//ConstantTable for CALL_MODEM_STATUS
#define CALL_MODEM_STATUS_IDLE                            	0x00                                                 	
#define CALL_MODEM_STATUS_CREATE                          	0x01                                                 	
#define CALL_MODEM_STATUS_COMING                          	0x02                                                 	
#define CALL_MODEM_STATUS_PROCEEDING                      	0x03                                                 	
#define CALL_MODEM_STATUS_MO_ALERTING                     	0x04                                                 	
#define CALL_MODEM_STATUS_MT_ALERTING                     	0x05                                                 	
#define CALL_MODEM_STATUS_WAITING                         	0x06                                                 	
#define CALL_MODEM_STATUS_ANSWERED                        	0x07                                                 	
#define CALL_MODEM_STATUS_ACTIVE                          	0x08                                                 	
#define CALL_MODEM_STATUS_MO_RELEASE                      	0x09                                                 	
#define CALL_MODEM_STATUS_MT_RELEASE                      	0x0A                                                 	
#define CALL_MODEM_STATUS_HOLD_INITIATED                  	0x0B                                                 	
#define CALL_MODEM_STATUS_HOLD                            	0x0C                                                 	
#define CALL_MODEM_STATUS_RETRIEVE_INITIATED              	0x0D                                                 	
#define CALL_MODEM_STATUS_RECONNECT_PENDING               	0x0E                                                 	
#define CALL_MODEM_STATUS_TERMINATED                      	0x0F                                                 	
#define CALL_MODEM_STATUS_SWAP_INITIATED                  	0x10                                              	

//BitmaskTables for CALL_MODEM_PRESENT_INFO
#define CALL_MODEM_PRESENT                                	0x00                                                 	
#define CALL_MODEM_NOT_PRESENT                            	0x01                                                 	

//BitmaskTables for CALL_MODEM_ALERTING_INFO
#define CALL_MODEM_AI_GEN_ALERTING_TONE                   	0x01                                                 	

//BitmaskTables for CALL_MODEM_RELEASE_INFO
#define CALL_MODEM_RI_IN_BAND_INFO                        	0x01                                                 	

//ConstantTable for CALL_MODEM_OPERATION
#define CALL_MODEM_OP_HOLD                                	0x01                                                 	
#define CALL_MODEM_OP_RETRIEVE                            	0x02                                                 	
#define CALL_MODEM_OP_SWAP                                	0x03                                                 	
#define CALL_MODEM_OP_CONFERENCE_BUILD                    	0x04                                                 	
#define CALL_MODEM_OP_CONFERENCE_SPLIT                    	0x05                                                 	
#define CALL_MODEM_OP_CUG                                 	0xA0                                              	
#define CALL_MODEM_OP_TRANSFER                            	0xA1                                              	

//ConstantTable for CALL_MODEM_DTMF_STATUS
#define CALL_MODEM_DTMF_IDLE                              	0x00                                                 	
#define CALL_MODEM_DTMF_ACTIVE                            	0x01                                                 	

//ConstantTable for CALL_MODEM_RECONNECT_STATUS
#define CALL_MODEM_RECONNECT_PENDING                      	0x01                                                 	
#define CALL_MODEM_RECONNECTED                            	0x02                                                 	

//ConstantTable for CALL_MODEM_PROPERTY
#define CALL_MODEM_PROP_ALL                               	0x01                                                 	
#define CALL_MODEM_PROP_IMMEDIATE_CONNECT                 	0xA0                                              	
#define CALL_MODEM_PROP_CUG_INFO                          	0xA1                                              	
#define CALL_MODEM_PROP_CALL_MODE_DEFAULT                 	0xA2                                              	
#define CALL_MODEM_PROP_ALS                               	0xA6                                              	
#define CALL_MODEM_PROP_LINE_ID                           	0xA8                                              	
#define CALL_MODEM_PROP_EMERG                             	0xA9                                              	
#define CALL_MODEM_PROP_CTM                               	0xAC                                              	

//ConstantTable for CALL_MODEM_PROP_IMMEDIATE_CONNECT
#define CALL_MODEM_IMM_CONNECT_OFF                        	0x00                                                 	
#define CALL_MODEM_IMM_CONNECT_ON                         	0x01                                                 	

//ConstantTable for CALL_MODEM_PROP_ALS
#define CALL_MODEM_PROP_ALS_LINE_NORMAL                   	0x00                                                 	
#define CALL_MODEM_PROP_ALS_LINE_2                        	0x01                                                 	

//ConstantTable for CALL_MODEM_PROP_LINE_ID
#define CALL_MODEM_PROP_PRESENT_DEFAULT                   	0x00                                                 	
#define CALL_MODEM_PROP_PRESENT_ALLOWED                   	0x01                                                 	
#define CALL_MODEM_PROP_PRESENT_RESTRICTED                	0x02                                                 	

//ConstantTable for CALL_MODEM_PROP_EMERG
#define CALL_MODEM_PROP_EMERG_CHK_DEFAULT                 	0x00                                                 	
#define CALL_MODEM_PROP_EMERG_CHK_DISABLED                	0x01                                                 	

//ConstantTable for CALL_MODEM_PROP_CTM
#define CALL_MODEM_PROP_CTM_NOT_SUPPORTED                 	0x00                                                 	
#define CALL_MODEM_PROP_CTM_SUPPORTED                     	0x01                                                 	

//ConstantTable for CALL_MODEM_EMERG_NUMBER_OPERATION
#define CALL_MODEM_EMERG_NUMBER_REMOVE                    	0x00                                                 	
#define CALL_MODEM_EMERG_NUMBER_ADD                       	0x01                                                 	

//ConstantTable for CALL_MODEM_EMERG_NUMBER_CHECK_STATUS
#define CALL_MODEM_EMERG_NBR_CHECK_FAIL                   	0x00                                                 	
#define CALL_MODEM_EMERG_NBR_CHECK_PASS                   	0x01                                                 	

//ConstantTable for CALL_MODEM_PREFERENTIAL_CUG
#define CALL_MODEM_CUG_PREF                               	0x00                                                 	
#define CALL_MODEM_CUG_PREF_SUPPRESS                      	0x01                                                 	

//ConstantTable for CALL_MODEM_CUG_OUTGOING_ACCESS
#define CALL_MODEM_CUG_OA                                 	0x00                                                 	
#define CALL_MODEM_CUG_OA_SUPPRESS                        	0x01                                                 	

//ConstantTable for CALL_MODEM_ALERTING_PATTERN
#define CALL_MODEM_ALERTING_LEVEL_0                       	0x00                                                 	
#define CALL_MODEM_ALERTING_LEVEL_1                       	0x01                                                 	
#define CALL_MODEM_ALERTING_LEVEL_2                       	0x02                                                 	
#define CALL_MODEM_ALERTING_CATEGORY_1                    	0x04                                                 	
#define CALL_MODEM_ALERTING_CATEGORY_2                    	0x05                                                 	
#define CALL_MODEM_ALERTING_CATEGORY_3                    	0x06                                                 	
#define CALL_MODEM_ALERTING_CATEGORY_4                    	0x07                                                 	
#define CALL_MODEM_ALERTING_CATEGORY_5                    	0x08                                                 	

//ConstantTable for CALL_MODEM_CONDITION
#define CALL_MODEM_COND_UNKNOWN                           	0x00                                                 	
#define CALL_MODEM_COND_PERMANENT                         	0x01                                                 	
#define CALL_MODEM_COND_TRANSIENT                         	0x02                                                 	

//ConstantTable for CALL_MODEM_SS_DIAGNOSTICS
#define CALL_MODEM_SSD_OUTGOING_BARR_CUG                  	0x01                                                 	
#define CALL_MODEM_SSD_NO_CUG                             	0x02                                                 	
#define CALL_MODEM_SSD_UNKNOWN_CUG                        	0x03                                                 	
#define CALL_MODEM_SSD_INCOMPATIBLE_CUG                   	0x04                                                 	
#define CALL_MODEM_SSD_CUG_FAIL                           	0x05                                                 	
#define CALL_MODEM_SSD_CLIR_NOT_SUBSCRIBED                	0x06                                                 	

//ConstantTable for CALL_MODEM_NOTIFICATION_INDICATOR
#define CALL_MODEM_NOTIFY_USER_SUSPENDED                  	0x00                                                 	
#define CALL_MODEM_NOTIFY_USER_RESUMED                    	0x01                                                 	
#define CALL_MODEM_NOTIFY_BEARER_CHANGE                   	0x02                                                 	

//ConstantTable for CALL_MODEM_MMI_SS_CODES
#define CALL_MODEM_SSC_ALL_FWDS                           	0x02                                                 	
#define CALL_MODEM_SSC_ALL_COND_FWD                       	0x04                                                 	
#define CALL_MODEM_SSC_CFU                                	0x15                                              	
#define CALL_MODEM_SSC_CFB                                	0x43                                              	
#define CALL_MODEM_SSC_CFNRY                              	0x3D                                              	
#define CALL_MODEM_SSC_CFGNC                              	0x3E                                              	
#define CALL_MODEM_SSC_OUTGOING_BARR_SERV                 	0x14D                                             	
#define CALL_MODEM_SSC_INCOMING_BARR_SERV                 	0x161                                             	
#define CALL_MODEM_SSC_CALL_WAITING                       	0x2B                                              	
#define CALL_MODEM_SSC_CLIP                               	0x1E                                              	
#define CALL_MODEM_SSC_CLIR                               	0x1F                                              	
#define CALL_MODEM_SSC_COLP                               	0x4C                                              	
#define CALL_MODEM_SSC_COLR                               	0x4D                                              	
#define CALL_MODEM_SSC_CNAP                               	0x12C                                             	
#define CALL_MODEM_SSC_ECT                                	0x60                                              	
#define CALL_MODEM_SSC_MPTY                               	0xFFFE                                            	
#define CALL_MODEM_SSC_CALL_HOLD                          	0xFFFF                                            	

//BitmaskTables for CALL_MODEM_SS_STATUS
#define CALL_MODEM_SS_STATUS_ACTIVE                       	0x01                                                 	
#define CALL_MODEM_SS_STATUS_REGISTERED                   	0x02                                                 	
#define CALL_MODEM_SS_STATUS_PROVISIONED                  	0x04                                                 	
#define CALL_MODEM_SS_STATUS_QUIESCENT                    	0x08                                                 	

//BitmaskTables for CALL_MODEM_SS_NOTIFICATION
#define CALL_MODEM_SSN_INCOMING_IS_FWD                    	0x01                                                 	
#define CALL_MODEM_SSN_INCOMING_FWD                       	0x02                                                 	
#define CALL_MODEM_SSN_OUTGOING_FWD                       	0x04                                                 	

//BitmaskTables for CALL_MODEM_SS_INDICATOR
#define CALL_MODEM_SSI_CALL_IS_WAITING                    	0x01                                                 	
#define CALL_MODEM_SSI_MPTY                               	0x02                                                 	
#define CALL_MODEM_SSI_CLIR_SUPPR_REJ                     	0x04                                                 	

//ConstantTable for CALL_MODEM_SS_HOLD_INDICATOR
#define CALL_MODEM_HOLD_IND_RETRIEVED                     	0x00                                                 	
#define CALL_MODEM_HOLD_IND_ON_HOLD                       	0x01                                                 	

//ConstantTable for CALL_MODEM_SS_ECT_INDICATOR
#define CALL_MODEM_ECT_CALL_STATE_ALERT                   	0x00                                                 	
#define CALL_MODEM_ECT_CALL_STATE_ACTIVE                  	0x01                                                 	

//ConstantTable for CALL_MODEM_BLACKLIST_DURATION
#define CALL_MODEM_BLACKLIST_TIMEOUT                      	0x00                                                 	
#define CALL_MODEM_BLACKLIST_DURATION_5                   	0x05                                                 	
#define CALL_MODEM_BLACKLIST_DURATION_60                  	0x3C                                              	
#define CALL_MODEM_BLACKLIST_DURATION_180                 	0xB4                                              	

//ConstantTable for CALL_MODEM_BC_CHECK_RESULT
#define CALL_MODEM_BC_CHECK_RESULT_FAIL                   	0x00                                                 	
#define CALL_MODEM_BC_CHECK_RESULT_OK                     	0x01                                                 	

//ConstantTable for CALL_MODEM_DTMF_PAUSE_VALUES
#define CALL_MODEM_DTMF_PAUSE_1S                          	0x01                                                 	
#define CALL_MODEM_DTMF_PAUSE_2S                          	0x02                                                 	
#define CALL_MODEM_DTMF_PAUSE_3S                          	0x03                                                 	

//ConstantTable for CALL_MODEM_DTMF_INFO_VALUES
#define CALL_MODEM_DTMF_ENABLE_TONE_IND_SEND              	0x01                                                 	
#define CALL_MODEM_DTMF_DISABLE_TONE_IND_SEND             	0x02                                                 	

//ConstantTable for CALL_MODEM_CAUSE_OF_NO_CLI_VALUES
#define CALL_MODEM_CLI_UNAVAILABLE                        	0x01                                                 	
#define CALL_MODEM_CLI_REJECT_BY_USER                     	0x02                                                 	
#define CALL_MODEM_CLI_INTERACTION                        	0x03                                                 	
#define CALL_MODEM_CLI_COIN_LINE_PAYPHONE                 	0x04                                                 	

//ConstantTable for CALL_MODEM_MM_CAUSE_VALUES
#define CALL_MODEM_MM_CAUSE_HLR_IMSI_UNKNOWN              	0x02                                                 	
#define CALL_MODEM_MM_CAUSE_ILLEGAL_MS                    	0x03                                                 	
#define CALL_MODEM_MM_CAUSE_VLR_IMSI_UNKNOWN              	0x04                                                 	
#define CALL_MODEM_MM_CAUSE_IMEI_NOT_ACCEPTED             	0x05                                                 	
#define CALL_MODEM_MM_CAUSE_ILLEGAL_ME                    	0x06                                                 	
#define CALL_MODEM_MM_CAUSE_PLMN_NOT_ALLOWED              	0x0B                                                 	
#define CALL_MODEM_MM_CAUSE_LA_NOT_ALLOWED                	0x0C                                                 	
#define CALL_MODEM_MM_CAUSE_ROAM_NOT_ALLOWED_LA           	0x0D                                                 	
#define CALL_MODEM_MM_CAUSE_NO_SUITABLE_CELLS_LA          	0x0F                                                 	
#define CALL_MODEM_MM_CAUSE_NETWORK_FAILURE               	0x11                                              	
#define CALL_MODEM_MM_CAUSE_MAC_FAILURE                   	0x14                                              	
#define CALL_MODEM_MM_CAUSE_SYNC_FAILURE                  	0x15                                              	
#define CALL_MODEM_MM_CAUSE_CONGESTION                    	0x16                                              	
#define CALL_MODEM_MM_CAUSE_AUTH_UNACCEPTABLE             	0x17                                              	
#define CALL_MODEM_MM_CAUSE_NOT_SUPP_S_OPTION             	0x20                                              	
#define CALL_MODEM_MM_CAUSE_NOT_SUBS_S_OPTION             	0x21                                              	
#define CALL_MODEM_MM_CAUSE_OUT_OF_ORDER_S_OPTION         	0x22                                              	
#define CALL_MODEM_MM_CAUSE_CALL_NOT_IDENTIFIED           	0x26                                              	
#define CALL_MODEM_MM_CAUSE_SEMANT_INCORRECT              	0x5F                                              	
#define CALL_MODEM_MM_CAUSE_INV_MAND_INFORMATION          	0x60                                              	
#define CALL_MODEM_MM_CAUSE_NON_EXISTENT_MSG_TYPE         	0x61                                              	
#define CALL_MODEM_MM_CAUSE_NOT_COMP_MSG_TYPE             	0x62                                              	
#define CALL_MODEM_MM_CAUSE_NOT_EXISTENT_IE               	0x63                                              	
#define CALL_MODEM_MM_CAUSE_CONDITIONAL_IE_ERROR          	0x64                                              	
#define CALL_MODEM_MM_CAUSE_MSG_NOT_COMPATIBLE            	0x65                                              	
#define CALL_MODEM_MM_CAUSE_PROTOCOL_ERROR                	0x6F                                              	

//ConstantTable for CALL_MODEM_ALS_STATUS
#define CALL_MODEM_ALS_STATUS_NOT_READY                   	0x00                                                 	
#define CALL_MODEM_ALS_STATUS_DISABLED                    	0x01                                                 	
#define CALL_MODEM_ALS_STATUS_ENABLED                     	0x02                                                 	

//ConstantTable for CALL_MODEM_AUTO_ST_CHANGE
#define CALL_MODEM_AUTO_ST_CHANGE_DISABLE                 	0x00                                                 	
#define CALL_MODEM_AUTO_ST_CHANGE_DISABLE_WAITING         	0x01                                                 	

//BitmaskTables for CALL_MODEM_REQ_ADDITIONAL_INFO
#define CALL_MODEM_ADD_INFO_NONE                          	0x00                                                 	
#define CALL_MODEM_ADD_INFO_EMERG_CALL                    	0x01                                                 	

//BitmaskTables for CALL_MODEM_EMERG_SERVICE_CATEGORY
#define CALL_MODEM_SERVICE_CAT_NONE                       	0x00                                                 	
#define CALL_MODEM_SERVICE_CAT_POLICE                     	0x01                                                 	
#define CALL_MODEM_SERVICE_CAT_AMBULANCE                  	0x02                                                 	
#define CALL_MODEM_SERVICE_CAT_FIRE_BRIGADE               	0x04                                                 	
#define CALL_MODEM_SERVICE_CAT_MARINE_GUARD               	0x08                                                 	
#define CALL_MODEM_SERVICE_CAT_MOUNTAIN_RESCUE            	0x10                                              	

//ConstantTable for CALL_MODEM_STATUS_MOD
#define CALL_MODEM_STATUS_MODE_DEFAULT                    	0x00                                                 	
#define CALL_MODEM_STATUS_MODE_ADDR                       	0x01                                                 	
#define CALL_MODEM_STATUS_MODE_ADDR_AND_ORIGIN            	0x02                                                 	

//ConstantTable for CALL_MODEM_USER_CONN
#define CALL_MODEM_USER_CONN_CLOSED                       	0x00                                                 	
#define CALL_MODEM_USER_CONN_ATTACHED                     	0x01                                                 	

//BitmaskTables for CALL_MODEM_EMERG_NBR_ORIGIN
#define CALL_MODEM_EMERG_NBR_ORIG_HARD_CODED              	0x01                                                 	
#define CALL_MODEM_EMERG_NBR_ORIG_SIM                     	0x02                                                 	
#define CALL_MODEM_EMERG_NBR_ORIG_NETWORK                 	0x04                                                 	
#define CALL_MODEM_EMERG_NBR_ORIG_PERMANENT_MEM           	0x08                                                 	

//BitmaskTables for CALL_MODEM_RESOURCE_IDS
#define CALL_MODEM_RES_ID_MO_INIT                         	0x01                                                 	
#define CALL_MODEM_RES_ID_MT_INIT                         	0x02                                                 	
#define CALL_MODEM_RES_ID_MT_ANSWER                       	0x04                                                 	
#define CALL_MODEM_RES_ID_HOLD                            	0x08                                                 	
#define CALL_MODEM_RES_ID_RETRIEVE                        	0x10                                              	
#define CALL_MODEM_RES_ID_AUTO_HOLD                       	0x20                                              	
#define CALL_MODEM_RES_ID_AUTO_RETRIEVE                   	0x40                                              	
#define CALL_MODEM_RES_ID_SWAP                            	0x80                                              	
#define CALL_MODEM_RES_ID_CONF_BUILD                      	0x100                                             	
#define CALL_MODEM_RES_ID_CONF_SPLIT                      	0x200                                             	
#define CALL_MODEM_RES_ID_TRANSFER                        	0x400                                             	

//BitmaskTables for CALL_MODEM_RESOURCE_IDS_MASK
#define CALL_MODEM_RES_ID_MASK_MO_INIT                    	0x01                                                 	
#define CALL_MODEM_RES_ID_MASK_MT_INIT                    	0x02                                                 	
#define CALL_MODEM_RES_ID_MASK_MT_ANSWER                  	0x04                                                 	
#define CALL_MODEM_RES_ID_MASK_HOLD                       	0x08                                                 	
#define CALL_MODEM_RES_ID_MASK_RETRIEVE                   	0x10                                              	
#define CALL_MODEM_RES_ID_MASK_AUTO_HOLD                  	0x20                                              	
#define CALL_MODEM_RES_ID_MASK_AUTO_RETRIEVE              	0x40                                              	
#define CALL_MODEM_RES_ID_MASK_SWAP                       	0x80                                              	
#define CALL_MODEM_RES_ID_MASK_CONF_BUILD                 	0x100                                             	
#define CALL_MODEM_RES_ID_MASK_CONF_SPLIT                 	0x200                                             	
#define CALL_MODEM_RES_ID_MASK_TRANSFER                   	0x400                                             	

//BitmaskTables for CALL_MODEM_RESOURCE_IDS_CONF_REQ
#define CALL_MODEM_RES_ID_CONF_MO_INIT                    	0x01                                                 	
#define CALL_MODEM_RES_ID_CONF_MT_INIT                    	0x02                                                 	
#define CALL_MODEM_RES_ID_CONF_MT_ANSWER                  	0x04                                                 	
#define CALL_MODEM_RES_ID_CONF_HOLD                       	0x08                                                 	
#define CALL_MODEM_RES_ID_CONF_RETRIEVE                   	0x10                                              	
#define CALL_MODEM_RES_ID_CONF_AUTO_HOLD                  	0x20                                              	
#define CALL_MODEM_RES_ID_CONF_AUTO_RETRIEVE              	0x40                                              	
#define CALL_MODEM_RES_ID_CONF_SWAP                       	0x80                                              	
#define CALL_MODEM_RES_ID_CONF_CONF_BUILD                 	0x100                                             	
#define CALL_MODEM_RES_ID_CONF_CONF_SPLIT                 	0x200                                             	
#define CALL_MODEM_RES_ID_CONF_TRANSFER                   	0x400                                             	

//ConstantTable for CALL_MODEM_RESOURCE_STATUS
#define CALL_MODEM_RESOURCE_ALLOWED                       	0x01                                                 	
#define CALL_MODEM_RESOURCE_DENIED                        	0x02                                                 	

//BitmaskTables for CALL_MODEM_CHECK_INFO
#define CALL_MODEM_CHK_DISABLE_NONE                       	0x00                                                 	
#define CALL_MODEM_CHK_DISABLE_EMERG                      	0x01                                                 	
#define CALL_MODEM_CHK_DISABLE_FDN                        	0x02                                                 	
#define CALL_MODEM_CHK_DISABLE_BLACKLIST                  	0x04                                                 	
#define CALL_MODEM_CHK_DISABLE_ALS                        	0x08                                                 	

//ConstantTable for CALL_MODEM_RES_CONF_STATUS
#define CALL_MODEM_RES_CONF_STARTUP                       	0x01                                                 	
#define CALL_MODEM_RES_CONF_RECONFIGURED                  	0x02                                                 	

//ConstantTable for CALL_MODEM_RES_CONF_OPERATION
#define CALL_MODEM_RES_CONF_GET                           	0x01                                                 	
#define CALL_MODEM_RES_CONF_SET                           	0x02                                                 	

//ConstantTable for CALL_MODEM_LINE_ID
#define CALL_MODEM_PRESENT_DEFAULT                        	0x00                                                 	
#define CALL_MODEM_PRESENT_ALLOWED                        	0x01                                                 	
#define CALL_MODEM_PRESENT_RESTRICTED                     	0x02                                                 	

//ConstantTable for CALL_MODEM_MSG
#define CALL_MODEM_MSG_SETUP                              	0x01                                                 	
#define CALL_MODEM_MSG_CONNECT                            	0x02                                                 	
#define CALL_MODEM_MSG_RELEASE                            	0x03                                                 	

//ConstantTable for CALL_MODEM_MSG_DIRECTION
#define CALL_MODEM_MSG_DIRECT_SENT                        	0x01                                                 	
#define CALL_MODEM_MSG_DIRECT_RECEIVED                    	0x02                                                 	
#define CALL_MODEM_MSG_DIRECT_INTERNAL                    	0x03                                                 	

//BitmaskTables for CALL_MODEM_RESOURCE_CHECK_IDS
//#define CALL_MODEM_RES_ID_MO_INIT                       	0x00                   	 //Already defined as 0x01


//SUBBLOCKS


//Definition for CALL_MODEM_SB_ORIGIN_ADDRESS
#define CALL_MODEM_SB_ORIGIN_ADDRESS                      	0x01                                                 	
#define CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_SUBBLOCKID    	0 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_SUBBLOCKLEN   	1 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_ADDRTYPE      	2 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_PRESENTATION  	3 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_FILLERBYTE1   	4 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_ADDRLEN       	5 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_ADDRESS_OFFSET_ADDR          	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_ORIGIN_ADDRESS                 	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_ORIGIN_SUBADDRESS
#define CALL_MODEM_SB_ORIGIN_SUBADDRESS                   	0x02                                                 	
#define CALL_MODEM_SB_ORIGIN_SUBADDRESS_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_SUBADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_SUBADDRESS_OFFSET_ADDRLEN    	2 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_SUBADDRESS_OFFSET_ADDR       	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_ORIGIN_SUBADDRESS              	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_DESTINATION_ADDRESS
#define CALL_MODEM_SB_DESTINATION_ADDRESS                 	0x03                                                 	
#define CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDRTYPE 	2 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_FILLERBYTE2	4 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDRLEN  	5 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ADDRESS_OFFSET_ADDR     	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_DESTINATION_ADDRESS            	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_DESTINATION_SUBADDRESS
#define CALL_MODEM_SB_DESTINATION_SUBADDRESS              	0x04                                                 	
#define CALL_MODEM_SB_DESTINATION_SUBADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_SUBADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_SUBADDRESS_OFFSET_ADDRLEN	2 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_SUBADDRESS_OFFSET_ADDR  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DESTINATION_SUBADDRESS         	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_DESTINATION_PRE_ADDRESS
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS             	0x05                                                 	
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_ADDRLEN	3 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_ADDR 	4 //size 2 byte(s)
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_FILLERBYTE2	6 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_PRE_ADDRESS_OFFSET_FILLERBYTE3	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DESTINATION_PRE_ADDRESS        	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_DESTINATION_POST_ADDRESS
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS            	0x06                                                 	
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_ADDRLEN	3 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_ADDR	4 //size 2 byte(s)
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_FILLERBYTE2	6 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_POST_ADDRESS_OFFSET_FILLERBYTE3	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DESTINATION_POST_ADDRESS       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_CONNECTED_ADDRESS
#define CALL_MODEM_SB_CONNECTED_ADDRESS                   	0x24                                              	
#define CALL_MODEM_SB_CONNECTED_ADDRESS_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_ADDRESS_OFFSET_ADDRTYPE   	2 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_ADDRESS_OFFSET_PRESENTATION	3 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_ADDRESS_OFFSET_FILLERBYTE1	4 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_ADDRESS_OFFSET_ADDRLEN    	5 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_ADDRESS_OFFSET_ADDR       	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_CONNECTED_ADDRESS              	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_CONNECTED_SUBADDRESS
#define CALL_MODEM_SB_CONNECTED_SUBADDRESS                	0x25                                              	
#define CALL_MODEM_SB_CONNECTED_SUBADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_SUBADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_SUBADDRESS_OFFSET_ADDRLEN 	2 //size 1 byte(s)
#define CALL_MODEM_SB_CONNECTED_SUBADDRESS_OFFSET_ADDR    	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_CONNECTED_SUBADDRESS           	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_MODE
#define CALL_MODEM_SB_MODE                                	0x07                                                 	
#define CALL_MODEM_SB_MODE_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define CALL_MODEM_SB_MODE_OFFSET_SUBBLOCKLEN             	1 //size 1 byte(s)
#define CALL_MODEM_SB_MODE_OFFSET_MODE                    	2 //size 1 byte(s)
#define CALL_MODEM_SB_MODE_OFFSET_MODEINFO                	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_MODE                           	4


//Definition for CALL_MODEM_SB_CAUSE
#define CALL_MODEM_SB_CAUSE                               	0x08                                                 	
#define CALL_MODEM_SB_CAUSE_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define CALL_MODEM_SB_CAUSE_OFFSET_SUBBLOCKLEN            	1 //size 1 byte(s)
#define CALL_MODEM_SB_CAUSE_OFFSET_CAUSETYPE              	2 //size 1 byte(s)
#define CALL_MODEM_SB_CAUSE_OFFSET_CAUSE                  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_CAUSE                          	4


//Definition for CALL_MODEM_SB_OPERATION
#define CALL_MODEM_SB_OPERATION                           	0x09                                                 	
#define CALL_MODEM_SB_OPERATION_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define CALL_MODEM_SB_OPERATION_OFFSET_SUBBLOCKLEN        	1 //size 1 byte(s)
#define CALL_MODEM_SB_OPERATION_OFFSET_OPERATION          	2 //size 1 byte(s)
#define CALL_MODEM_SB_OPERATION_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_OPERATION                      	4


//Definition for CALL_MODEM_SB_STATUS
#define CALL_MODEM_SB_STATUS                              	0x0A                                                 	
#define CALL_MODEM_SB_STATUS_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_OFFSET_SUBBLOCKLEN           	1 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_OFFSET_STATUS                	2 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_STATUS                         	4


//Definition for CALL_MODEM_SB_STATUS_INFO
#define CALL_MODEM_SB_STATUS_INFO                         	0x0B                                                 	
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_CALLID           	2 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_MODE             	3 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_MODEINFO         	4 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_STATUS           	5 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_FILLERBYTE1      	6 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_INFO_OFFSET_FILLERBYTE2      	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_STATUS_INFO                    	8


//Definition for CALL_MODEM_SB_ALERTING_INFO
#define CALL_MODEM_SB_ALERTING_INFO                       	0x0C                                                 	
#define CALL_MODEM_SB_ALERTING_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define CALL_MODEM_SB_ALERTING_INFO_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define CALL_MODEM_SB_ALERTING_INFO_OFFSET_ALERTINGINFO   	2 //size 1 byte(s)
#define CALL_MODEM_SB_ALERTING_INFO_OFFSET_FILLERBYTE1    	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_ALERTING_INFO                  	4


//Definition for CALL_MODEM_SB_RELEASE_INFO
#define CALL_MODEM_SB_RELEASE_INFO                        	0x0D                                                 	
#define CALL_MODEM_SB_RELEASE_INFO_OFFSET_SUBBLOCKID      	0 //size 1 byte(s)
#define CALL_MODEM_SB_RELEASE_INFO_OFFSET_SUBBLOCKLEN     	1 //size 1 byte(s)
#define CALL_MODEM_SB_RELEASE_INFO_OFFSET_RELEASEINFO     	2 //size 1 byte(s)
#define CALL_MODEM_SB_RELEASE_INFO_OFFSET_FILLERBYTE1     	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_RELEASE_INFO                   	4


//Definition for CALL_MODEM_SB_ORIGIN_INFO
#define CALL_MODEM_SB_ORIGIN_INFO                         	0x0E                                                 	
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_PRESENTATION     	2 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_FILLERBYTE2      	4 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_FILLERBYTE3      	5 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_LINKID           	6 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_NAMELEN          	7 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_NAME             	8 //size 2 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_FILLERBYTE4      	10 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_INFO_OFFSET_FILLERBYTE5      	11 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_ORIGIN_INFO                    	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_DTMF_DIGIT
#define CALL_MODEM_SB_DTMF_DIGIT                          	0x0F                                                 	
#define CALL_MODEM_SB_DTMF_DIGIT_OFFSET_SUBBLOCKID        	0 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_DIGIT_OFFSET_SUBBLOCKLEN       	1 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_DIGIT_OFFSET_DATA              	2 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_DTMF_DIGIT                     	4


//Definition for CALL_MODEM_SB_DTMF_STRING
#define CALL_MODEM_SB_DTMF_STRING                         	0x10                                              	
#define CALL_MODEM_SB_DTMF_STRING_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STRING_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STRING_OFFSET_PAUSELENGTH      	2 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STRING_OFFSET_DATALEN          	3 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STRING_OFFSET_DATA             	4 //size 2 byte(s)
#define CALL_MODEM_SB_DTMF_STRING_OFFSET_FILLERBYTE1      	6 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STRING_OFFSET_FILLERBYTE2      	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DTMF_STRING                    	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_DTMF_BCD_STRING
#define CALL_MODEM_SB_DTMF_BCD_STRING                     	0x19                                              	
#define CALL_MODEM_SB_DTMF_BCD_STRING_OFFSET_SUBBLOCKID   	0 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_BCD_STRING_OFFSET_SUBBLOCKLEN  	1 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_BCD_STRING_OFFSET_PAUSELENGTH  	2 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_BCD_STRING_OFFSET_FILLERBYTE1  	3 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_BCD_STRING_OFFSET_DATALEN      	4 //size 2 byte(s)
#define CALL_MODEM_SB_DTMF_BCD_STRING_OFFSET_DATA         	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_DTMF_BCD_STRING                	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_DTMF_INFO
#define CALL_MODEM_SB_DTMF_INFO                           	0x1A                                              	
#define CALL_MODEM_SB_DTMF_INFO_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_INFO_OFFSET_SUBBLOCKLEN        	1 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_INFO_OFFSET_DTMFINFO           	2 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_INFO_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DTMF_INFO                      	4


//Definition for CALL_MODEM_SB_PROPERTY_INFO
#define CALL_MODEM_SB_PROPERTY_INFO                       	0x13                                              	
#define CALL_MODEM_SB_PROPERTY_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define CALL_MODEM_SB_PROPERTY_INFO_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define CALL_MODEM_SB_PROPERTY_INFO_OFFSET_PROPERTY       	2 //size 1 byte(s)
#define CALL_MODEM_SB_PROPERTY_INFO_OFFSET_PROPVALUE      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_PROPERTY_INFO                  	4


//Definition for CALL_MODEM_SB_EMERG_NUMBER
#define CALL_MODEM_SB_EMERG_NUMBER                        	0x14                                              	
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_SUBBLOCKID      	0 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_SUBBLOCKLEN     	1 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_ORIGIN          	2 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_FILLERBYTE1     	3 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_FILLERBYTE2     	4 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_LINKID          	5 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_FILLERBYTE3     	6 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_ADDRLEN         	7 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_ADDR            	8 //size 2 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_FILLERBYTE4     	10 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_OFFSET_FILLERBYTE5     	11 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_EMERG_NUMBER                   	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_DTMF_STATUS
#define CALL_MODEM_SB_DTMF_STATUS                         	0x11                                              	
#define CALL_MODEM_SB_DTMF_STATUS_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STATUS_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STATUS_OFFSET_STATUS           	2 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_STATUS_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DTMF_STATUS                    	4


//Definition for CALL_MODEM_SB_DTMF_TONE
#define CALL_MODEM_SB_DTMF_TONE                           	0x12                                              	
#define CALL_MODEM_SB_DTMF_TONE_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_TONE_OFFSET_SUBBLOCKLEN        	1 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_TONE_OFFSET_TONE               	2 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_DTMF_TONE                      	4


//Definition for CALL_MODEM_SB_CUG_INFO
#define CALL_MODEM_SB_CUG_INFO                            	0xA0                                              	
#define CALL_MODEM_SB_CUG_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define CALL_MODEM_SB_CUG_INFO_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define CALL_MODEM_SB_CUG_INFO_OFFSET_PREFCUG             	2 //size 1 byte(s)
#define CALL_MODEM_SB_CUG_INFO_OFFSET_CUGOA               	3 //size 1 byte(s)
#define CALL_MODEM_SB_CUG_INFO_OFFSET_CUGIND              	4 //size 2 byte(s)
#define CALL_MODEM_SB_CUG_INFO_OFFSET_FILLERBYTE1         	6 //size 1 byte(s)
#define CALL_MODEM_SB_CUG_INFO_OFFSET_FILLERBYTE2         	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_CUG_INFO                       	8


//Definition for CALL_MODEM_SB_ALERTING_PATTERN
#define CALL_MODEM_SB_ALERTING_PATTERN                    	0xA1                                              	
#define CALL_MODEM_SB_ALERTING_PATTERN_OFFSET_SUBBLOCKID  	0 //size 1 byte(s)
#define CALL_MODEM_SB_ALERTING_PATTERN_OFFSET_SUBBLOCKLEN 	1 //size 1 byte(s)
#define CALL_MODEM_SB_ALERTING_PATTERN_OFFSET_ALERTINGPATTERN	2 //size 1 byte(s)
#define CALL_MODEM_SB_ALERTING_PATTERN_OFFSET_FILLERBYTE1 	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_ALERTING_PATTERN               	4


//Definition for CALL_MODEM_SB_REDIRECTING_ADDRESS
#define CALL_MODEM_SB_REDIRECTING_ADDRESS                 	0xA4                                              	
#define CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_ADDRTYPE 	2 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_PRESENTATION	3 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_FILLERBYTE1	4 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_ADDRLEN  	5 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_ADDRESS_OFFSET_ADDR     	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_REDIRECTING_ADDRESS            	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_REDIRECTING_SUBADDRESS
#define CALL_MODEM_SB_REDIRECTING_SUBADDRESS              	0xA5                                              	
#define CALL_MODEM_SB_REDIRECTING_SUBADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_SUBADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_SUBADDRESS_OFFSET_ADDRLEN	2 //size 1 byte(s)
#define CALL_MODEM_SB_REDIRECTING_SUBADDRESS_OFFSET_ADDR  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_REDIRECTING_SUBADDRESS         	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_REMOTE_ADDRESS
#define CALL_MODEM_SB_REMOTE_ADDRESS                      	0xA6                                              	
#define CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_SUBBLOCKID    	0 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_SUBBLOCKLEN   	1 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_ADDRTYPE      	2 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_PRESENTATION  	3 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_FILLERBYTE1   	4 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_ADDRLEN       	5 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_ADDRESS_OFFSET_ADDR          	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_REMOTE_ADDRESS                 	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_REMOTE_SUBADDRESS
#define CALL_MODEM_SB_REMOTE_SUBADDRESS                   	0xA7                                              	
#define CALL_MODEM_SB_REMOTE_SUBADDRESS_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_SUBADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_SUBADDRESS_OFFSET_ADDRLEN    	2 //size 1 byte(s)
#define CALL_MODEM_SB_REMOTE_SUBADDRESS_OFFSET_ADDR       	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_REMOTE_SUBADDRESS              	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_DIAGNOSTICS
#define CALL_MODEM_SB_DIAGNOSTICS                         	0xA9                                              	
#define CALL_MODEM_SB_DIAGNOSTICS_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_MODEM_SB_DIAGNOSTICS_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_MODEM_SB_DIAGNOSTICS_OFFSET_CONDITION        	2 //size 1 byte(s)
#define CALL_MODEM_SB_DIAGNOSTICS_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DIAGNOSTICS                    	4


//Definition for CALL_MODEM_SB_SS_DIAGNOSTICS
#define CALL_MODEM_SB_SS_DIAGNOSTICS                      	0xAA                                              	
#define CALL_MODEM_SB_SS_DIAGNOSTICS_OFFSET_SUBBLOCKID    	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_DIAGNOSTICS_OFFSET_SUBBLOCKLEN   	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_DIAGNOSTICS_OFFSET_DIAGNOSTICS   	2 //size 1 byte(s)
#define CALL_MODEM_SB_SS_DIAGNOSTICS_OFFSET_FILLERBYTE1   	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_SS_DIAGNOSTICS                 	4


//Definition for CALL_MODEM_SB_NEW_DESTINATION
#define CALL_MODEM_SB_NEW_DESTINATION                     	0xAB                                              	
#define CALL_MODEM_SB_NEW_DESTINATION_OFFSET_SUBBLOCKID   	0 //size 1 byte(s)
#define CALL_MODEM_SB_NEW_DESTINATION_OFFSET_SUBBLOCKLEN  	1 //size 1 byte(s)
#define CALL_MODEM_SB_NEW_DESTINATION_OFFSET_ADDRTYPE     	2 //size 1 byte(s)
#define CALL_MODEM_SB_NEW_DESTINATION_OFFSET_FILLERBYTE1  	3 //size 1 byte(s)
#define CALL_MODEM_SB_NEW_DESTINATION_OFFSET_FILLERBYTE2  	4 //size 1 byte(s)
#define CALL_MODEM_SB_NEW_DESTINATION_OFFSET_ADDRLEN      	5 //size 1 byte(s)
#define CALL_MODEM_SB_NEW_DESTINATION_OFFSET_ADDR         	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_NEW_DESTINATION                	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_NOTIFY
#define CALL_MODEM_SB_NOTIFY                              	0xB1                                              	
#define CALL_MODEM_SB_NOTIFY_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define CALL_MODEM_SB_NOTIFY_OFFSET_SUBBLOCKLEN           	1 //size 1 byte(s)
#define CALL_MODEM_SB_NOTIFY_OFFSET_NOTIFICATION          	2 //size 1 byte(s)
#define CALL_MODEM_SB_NOTIFY_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_NOTIFY                         	4


//Definition for CALL_MODEM_SB_SS_NOTIFY
#define CALL_MODEM_SB_SS_NOTIFY                           	0xB2                                              	
#define CALL_MODEM_SB_SS_NOTIFY_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_NOTIFY_OFFSET_SUBBLOCKLEN        	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_NOTIFY_OFFSET_SSNOTIFICATION     	2 //size 1 byte(s)
#define CALL_MODEM_SB_SS_NOTIFY_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_SS_NOTIFY                      	4


//Definition for CALL_MODEM_SB_SS_CODE
#define CALL_MODEM_SB_SS_CODE                             	0xB3                                              	
#define CALL_MODEM_SB_SS_CODE_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_CODE_OFFSET_SUBBLOCKLEN          	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_CODE_OFFSET_MMISSCODE            	2 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_SS_CODE                        	4


//Definition for CALL_MODEM_SB_SS_STATUS
#define CALL_MODEM_SB_SS_STATUS                           	0xB4                                              	
#define CALL_MODEM_SB_SS_STATUS_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_STATUS_OFFSET_SUBBLOCKLEN        	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_STATUS_OFFSET_SSSTATUS           	2 //size 1 byte(s)
#define CALL_MODEM_SB_SS_STATUS_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_SS_STATUS                      	4


//Definition for CALL_MODEM_SB_SS_NOTIFY_INDICATOR
#define CALL_MODEM_SB_SS_NOTIFY_INDICATOR                 	0xB5                                              	
#define CALL_MODEM_SB_SS_NOTIFY_INDICATOR_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_NOTIFY_INDICATOR_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_NOTIFY_INDICATOR_OFFSET_SSINDICATOR	2 //size 1 byte(s)
#define CALL_MODEM_SB_SS_NOTIFY_INDICATOR_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_SS_NOTIFY_INDICATOR            	4


//Definition for CALL_MODEM_SB_SS_HOLD_INDICATOR
#define CALL_MODEM_SB_SS_HOLD_INDICATOR                   	0xB6                                              	
#define CALL_MODEM_SB_SS_HOLD_INDICATOR_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_HOLD_INDICATOR_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_HOLD_INDICATOR_OFFSET_SSHOLDINDICATOR	2 //size 1 byte(s)
#define CALL_MODEM_SB_SS_HOLD_INDICATOR_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_SS_HOLD_INDICATOR              	4


//Definition for CALL_MODEM_SB_SS_ECT_INDICATOR
#define CALL_MODEM_SB_SS_ECT_INDICATOR                    	0xB7                                              	
#define CALL_MODEM_SB_SS_ECT_INDICATOR_OFFSET_SUBBLOCKID  	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_ECT_INDICATOR_OFFSET_SUBBLOCKLEN 	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_ECT_INDICATOR_OFFSET_SSECTINDICATOR	2 //size 1 byte(s)
#define CALL_MODEM_SB_SS_ECT_INDICATOR_OFFSET_FILLERBYTE1 	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_SS_ECT_INDICATOR               	4


//Definition for CALL_MODEM_SB_DESTINATION_CS_ADDRESS
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS              	0x16                                              	
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_FILLERBYTE2	3 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_FILLERBYTE3	4 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_ADDRLEN	5 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_ADDR  	6 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_CS_ADDRESS_OFFSET_FILLERBYTE4	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DESTINATION_CS_ADDRESS         	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_BC
#define CALL_MODEM_SB_BC                                  	0xBA                                              	
#define CALL_MODEM_SB_BC_OFFSET_SUBBLOCKID                	0 //size 1 byte(s)
#define CALL_MODEM_SB_BC_OFFSET_SUBBLOCKLEN               	1 //size 1 byte(s)
#define CALL_MODEM_SB_BC_OFFSET_BCLENGTH                  	2 //size 1 byte(s)
#define CALL_MODEM_SB_BC_OFFSET_BCDATA                    	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_BC                             	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_CAUSE_OF_NO_CLI
#define CALL_MODEM_SB_CAUSE_OF_NO_CLI                     	0xBC                                              	
#define CALL_MODEM_SB_CAUSE_OF_NO_CLI_OFFSET_SUBBLOCKID   	0 //size 1 byte(s)
#define CALL_MODEM_SB_CAUSE_OF_NO_CLI_OFFSET_SUBBLOCKLEN  	1 //size 1 byte(s)
#define CALL_MODEM_SB_CAUSE_OF_NO_CLI_OFFSET_CAUSE        	2 //size 1 byte(s)
#define CALL_MODEM_SB_CAUSE_OF_NO_CLI_OFFSET_FILLERBYTE1  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_CAUSE_OF_NO_CLI                	4


//Definition for CALL_MODEM_SB_MM_CAUSE
#define CALL_MODEM_SB_MM_CAUSE                            	0xBD                                              	
#define CALL_MODEM_SB_MM_CAUSE_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define CALL_MODEM_SB_MM_CAUSE_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define CALL_MODEM_SB_MM_CAUSE_OFFSET_CAUSE               	2 //size 1 byte(s)
#define CALL_MODEM_SB_MM_CAUSE_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_MM_CAUSE                       	4


//Definition for CALL_MODEM_SB_MSG_INFO
#define CALL_MODEM_SB_MSG_INFO                            	0x49                                              	
#define CALL_MODEM_SB_MSG_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define CALL_MODEM_SB_MSG_INFO_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define CALL_MODEM_SB_MSG_INFO_OFFSET_MSG                 	2 //size 1 byte(s)
#define CALL_MODEM_SB_MSG_INFO_OFFSET_MSGDIR              	3 //size 1 byte(s)
#define CALL_MODEM_SB_MSG_INFO_OFFSET_TRANSACTIONID       	4 //size 1 byte(s)
#define CALL_MODEM_SB_MSG_INFO_OFFSET_FILLERBYTE1         	5 //size 1 byte(s)
#define CALL_MODEM_SB_MSG_INFO_OFFSET_FILLERBYTE2         	6 //size 1 byte(s)
#define CALL_MODEM_SB_MSG_INFO_OFFSET_FILLERBYTE3         	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_MSG_INFO                       	8


//Definition for CALL_MODEM_SB_DETAILED_CAUSE
#define CALL_MODEM_SB_DETAILED_CAUSE                      	0xBF                                              	
#define CALL_MODEM_SB_DETAILED_CAUSE_OFFSET_SUBBLOCKID    	0 //size 1 byte(s)
#define CALL_MODEM_SB_DETAILED_CAUSE_OFFSET_SUBBLOCKLEN   	1 //size 1 byte(s)
#define CALL_MODEM_SB_DETAILED_CAUSE_OFFSET_CAUSELEN      	2 //size 1 byte(s)
#define CALL_MODEM_SB_DETAILED_CAUSE_OFFSET_CAUSE         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DETAILED_CAUSE                 	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_SS_DATA
#define CALL_MODEM_SB_SS_DATA                             	0xC0                                              	
#define CALL_MODEM_SB_SS_DATA_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define CALL_MODEM_SB_SS_DATA_OFFSET_SUBBLOCKLEN          	1 //size 1 byte(s)
#define CALL_MODEM_SB_SS_DATA_OFFSET_SSDATALEN            	2 //size 1 byte(s)
#define CALL_MODEM_SB_SS_DATA_OFFSET_SSDATA               	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_SS_DATA                        	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_ALS_INFO
#define CALL_MODEM_SB_ALS_INFO                            	0xC1                                              	
#define CALL_MODEM_SB_ALS_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define CALL_MODEM_SB_ALS_INFO_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define CALL_MODEM_SB_ALS_INFO_OFFSET_ALSSIMSTATUS        	2 //size 1 byte(s)
#define CALL_MODEM_SB_ALS_INFO_OFFSET_ALSPPSTATUS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_ALS_INFO                       	4


//Definition for CALL_MODEM_SB_STATE_AUTO_CHANGE
#define CALL_MODEM_SB_STATE_AUTO_CHANGE                   	0x18                                              	
#define CALL_MODEM_SB_STATE_AUTO_CHANGE_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_MODEM_SB_STATE_AUTO_CHANGE_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_STATE_AUTO_CHANGE_OFFSET_STATEAUTOCHANGE	2 //size 1 byte(s)
#define CALL_MODEM_SB_STATE_AUTO_CHANGE_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_STATE_AUTO_CHANGE              	4


//Definition for CALL_MODEM_SB_EMERG_NUMBER_INFO
#define CALL_MODEM_SB_EMERG_NUMBER_INFO                   	0x1B                                              	
#define CALL_MODEM_SB_EMERG_NUMBER_INFO_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_INFO_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_INFO_OFFSET_LINKID     	2 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_INFO_OFFSET_SERVCATEGORY	3 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_INFO_OFFSET_FILLERBYTE1	4 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_INFO_OFFSET_ALPHAIDLEN 	5 //size 1 byte(s)
#define CALL_MODEM_SB_EMERG_NUMBER_INFO_OFFSET_ALPHAID    	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_EMERG_NUMBER_INFO              	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_STATUS_MODE
#define CALL_MODEM_SB_STATUS_MODE                         	0x1C                                              	
#define CALL_MODEM_SB_STATUS_MODE_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_MODE_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_MODE_OFFSET_MODE             	2 //size 1 byte(s)
#define CALL_MODEM_SB_STATUS_MODE_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_STATUS_MODE                    	4


//Definition for CALL_MODEM_SB_ADDR_AND_STATUS_INFO
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO                	0x1D                                              	
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_CALLID  	2 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_MODE    	3 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_MODEINFO	4 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_STATUS  	5 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_FILLERBYTE1	6 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_LINKID  	7 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_ADDRTYPE	8 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_PRESENTATION	9 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_FILLERBYTE2	10 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_ADDRLEN 	11 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_ADDR    	12 //size 2 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_FILLERBYTE3	14 //size 1 byte(s)
#define CALL_MODEM_SB_ADDR_AND_STATUS_INFO_OFFSET_FILLERBYTE4	15 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_ADDR_AND_STATUS_INFO           	16
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_DTMF_TIMERS
#define CALL_MODEM_SB_DTMF_TIMERS                         	0x1E                                              	
#define CALL_MODEM_SB_DTMF_TIMERS_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_TIMERS_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_TIMERS_OFFSET_DTMFDUR          	2 //size 2 byte(s)
#define CALL_MODEM_SB_DTMF_TIMERS_OFFSET_DTMFGAP          	4 //size 2 byte(s)
#define CALL_MODEM_SB_DTMF_TIMERS_OFFSET_FILLERBYTE1      	6 //size 1 byte(s)
#define CALL_MODEM_SB_DTMF_TIMERS_OFFSET_FILLERBYTE2      	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_DTMF_TIMERS                    	8


//Definition for CALL_MODEM_SB_NW_CAUSE
#define CALL_MODEM_SB_NW_CAUSE                            	0x20                                              	
#define CALL_MODEM_SB_NW_CAUSE_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define CALL_MODEM_SB_NW_CAUSE_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define CALL_MODEM_SB_NW_CAUSE_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define CALL_MODEM_SB_NW_CAUSE_OFFSET_CAUSE               	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_NW_CAUSE                       	4


//Definition for CALL_MODEM_SB_DESTINATION_ISI_ADDRESS
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS             	0x23                                              	
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS_OFFSET_ADDRTYPE	2 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS_OFFSET_FILLERBYTE2	4 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS_OFFSET_ADDRLEN	5 //size 1 byte(s)
#define CALL_MODEM_SB_DESTINATION_ISI_ADDRESS_OFFSET_ADDR 	6 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_DESTINATION_ISI_ADDRESS        	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_MODEM_SB_RESOURCE
#define CALL_MODEM_SB_RESOURCE                            	0x41                                              	
#define CALL_MODEM_SB_RESOURCE_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_OFFSET_RES                 	2 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_RESOURCE                       	4


//Definition for CALL_MODEM_SB_RESOURCE_CONF
#define CALL_MODEM_SB_RESOURCE_CONF                       	0x42                                              	
#define CALL_MODEM_SB_RESOURCE_CONF_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_CONF_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_CONF_OFFSET_RES            	2 //size 2 byte(s)
#define CALL_MODEM_SB_RESOURCE_CONF_OFFSET_RESMASK        	4 //size 2 byte(s)
#define CALL_MODEM_SB_RESOURCE_CONF_OFFSET_FILLERBYTE1    	6 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_CONF_OFFSET_FILLERBYTE2    	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_RESOURCE_CONF                  	8


//Definition for CALL_MODEM_SB_RESOURCE_CONF_REQUIRED
#define CALL_MODEM_SB_RESOURCE_CONF_REQUIRED              	0x43                                              	
#define CALL_MODEM_SB_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_CONF_REQUIRED_OFFSET_RESPP 	2 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_RESOURCE_CONF_REQUIRED         	4


//Definition for CALL_MODEM_SB_RESOURCE_SEQ_ID
#define CALL_MODEM_SB_RESOURCE_SEQ_ID                     	0x44                                              	
#define CALL_MODEM_SB_RESOURCE_SEQ_ID_OFFSET_SUBBLOCKID   	0 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_SEQ_ID_OFFSET_SUBBLOCKLEN  	1 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_SEQ_ID_OFFSET_SEQUENCEID   	2 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_SEQ_ID_OFFSET_FILLERBYTE1  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_RESOURCE_SEQ_ID                	4


//Definition for CALL_MODEM_SB_RESOURCE_STATUS
#define CALL_MODEM_SB_RESOURCE_STATUS                     	0x45                                              	
#define CALL_MODEM_SB_RESOURCE_STATUS_OFFSET_SUBBLOCKID   	0 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_STATUS_OFFSET_SUBBLOCKLEN  	1 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_STATUS_OFFSET_RESSTATUS    	2 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_STATUS_OFFSET_FILLERBYTE1  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_RESOURCE_STATUS                	4


//Definition for CALL_MODEM_SB_RESOURCE_CHECK_INFO
#define CALL_MODEM_SB_RESOURCE_CHECK_INFO                 	0x46                                              	
#define CALL_MODEM_SB_RESOURCE_CHECK_INFO_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_CHECK_INFO_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_RESOURCE_CHECK_INFO_OFFSET_RES      	2 //size 2 byte(s)
#define SIZE_CALL_MODEM_SB_RESOURCE_CHECK_INFO            	4


//Definition for CALL_MODEM_SB_CHECK_INFO
#define CALL_MODEM_SB_CHECK_INFO                          	0x40                                              	
#define CALL_MODEM_SB_CHECK_INFO_OFFSET_SUBBLOCKID        	0 //size 1 byte(s)
#define CALL_MODEM_SB_CHECK_INFO_OFFSET_SUBBLOCKLEN       	1 //size 1 byte(s)
#define CALL_MODEM_SB_CHECK_INFO_OFFSET_CHECKINFO         	2 //size 1 byte(s)
#define CALL_MODEM_SB_CHECK_INFO_OFFSET_FILLERBYTE1       	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_CHECK_INFO                     	4


//Definition for CALL_MODEM_SB_LINE_ID
#define CALL_MODEM_SB_LINE_ID                             	0x47                                              	
#define CALL_MODEM_SB_LINE_ID_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define CALL_MODEM_SB_LINE_ID_OFFSET_SUBBLOCKLEN          	1 //size 1 byte(s)
#define CALL_MODEM_SB_LINE_ID_OFFSET_LINEID               	2 //size 1 byte(s)
#define CALL_MODEM_SB_LINE_ID_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_LINE_ID                        	4


//Definition for CALL_MODEM_SB_USER_TO_USER
#define CALL_MODEM_SB_USER_TO_USER                        	0x48                                              	
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_SUBBLOCKID      	0 //size 1 byte(s)
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_SUBBLOCKLEN     	1 //size 1 byte(s)
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_UUSTYPE         	2 //size 1 byte(s)
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_UUSACTTYPE      	3 //size 1 byte(s)
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_UTOULEN         	4 //size 1 byte(s)
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_UTOU            	5 //size 1 byte(s)
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_FILLERBYTE1     	6 //size 1 byte(s)
#define CALL_MODEM_SB_USER_TO_USER_OFFSET_FILLERBYTE2     	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_USER_TO_USER                   	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_ORIGIN_CS_ADDRESS
#define CALL_MODEM_SB_ORIGIN_CS_ADDRESS                   	0x4A                                              	
#define CALL_MODEM_SB_ORIGIN_CS_ADDRESS_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_CS_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_CS_ADDRESS_OFFSET_ADDRLEN    	2 //size 1 byte(s)
#define CALL_MODEM_SB_ORIGIN_CS_ADDRESS_OFFSET_ADDR       	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_ORIGIN_CS_ADDRESS              	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_SB_REQ_INFO
#define CALL_MODEM_SB_REQ_INFO                            	0x4B                                              	
#define CALL_MODEM_SB_REQ_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define CALL_MODEM_SB_REQ_INFO_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define CALL_MODEM_SB_REQ_INFO_OFFSET_TRID                	2 //size 1 byte(s)
#define CALL_MODEM_SB_REQ_INFO_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_SB_REQ_INFO                       	4


//MESSAGES


//Definition for CALL_MODEM_CREATE_REQ
#define CALL_MODEM_CREATE_REQ                             	0x01                                                 	
#define CALL_MODEM_CREATE_REQ_OFFSET_TRID                 	0 //size 1 byte(s)
#define CALL_MODEM_CREATE_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define CALL_MODEM_CREATE_REQ_OFFSET_CALLID               	2 //size 1 byte(s)
#define CALL_MODEM_CREATE_REQ_OFFSET_SUBBLOCKS            	3 //size 1 byte(s)
#define CALL_MODEM_CREATE_REQ_OFFSET_MODE                 	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_CREATE_REQ                        	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_CREATE_RESP
#define CALL_MODEM_CREATE_RESP                            	0x02                                                 	
#define CALL_MODEM_CREATE_RESP_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_CREATE_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_CREATE_RESP_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_CREATE_RESP_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_CREATE_RESP                       	4


//Definition for CALL_MODEM_COMING_IND
#define CALL_MODEM_COMING_IND                             	0x03                                                 	
#define CALL_MODEM_COMING_IND_OFFSET_TRID                 	0 //size 1 byte(s)
#define CALL_MODEM_COMING_IND_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define CALL_MODEM_COMING_IND_OFFSET_CALLID               	2 //size 1 byte(s)
#define CALL_MODEM_COMING_IND_OFFSET_SUBBLOCKS            	3 //size 1 byte(s)
#define CALL_MODEM_COMING_IND_OFFSET_MODE                 	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_COMING_IND                        	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_MO_ALERT_IND
#define CALL_MODEM_MO_ALERT_IND                           	0x04                                                 	
#define CALL_MODEM_MO_ALERT_IND_OFFSET_TRID               	0 //size 1 byte(s)
#define CALL_MODEM_MO_ALERT_IND_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define CALL_MODEM_MO_ALERT_IND_OFFSET_CALLID             	2 //size 1 byte(s)
#define CALL_MODEM_MO_ALERT_IND_OFFSET_SUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_MO_ALERT_IND                      	4


//Definition for CALL_MODEM_MT_ALERT_IND
#define CALL_MODEM_MT_ALERT_IND                           	0x05                                                 	
#define CALL_MODEM_MT_ALERT_IND_OFFSET_TRID               	0 //size 1 byte(s)
#define CALL_MODEM_MT_ALERT_IND_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define CALL_MODEM_MT_ALERT_IND_OFFSET_CALLID             	2 //size 1 byte(s)
#define CALL_MODEM_MT_ALERT_IND_OFFSET_SUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_MT_ALERT_IND                      	4


//Definition for CALL_MODEM_WAITING_IND
#define CALL_MODEM_WAITING_IND                            	0x06                                                 	
#define CALL_MODEM_WAITING_IND_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_WAITING_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_WAITING_IND_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_WAITING_IND_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_WAITING_IND                       	4


//Definition for CALL_MODEM_ANSWER_REQ
#define CALL_MODEM_ANSWER_REQ                             	0x07                                                 	
#define CALL_MODEM_ANSWER_REQ_OFFSET_TRID                 	0 //size 1 byte(s)
#define CALL_MODEM_ANSWER_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define CALL_MODEM_ANSWER_REQ_OFFSET_CALLID               	2 //size 1 byte(s)
#define CALL_MODEM_ANSWER_REQ_OFFSET_SUBBLOCKS            	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_ANSWER_REQ                        	4


//Definition for CALL_MODEM_ANSWER_RESP
#define CALL_MODEM_ANSWER_RESP                            	0x08                                                 	
#define CALL_MODEM_ANSWER_RESP_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_ANSWER_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_ANSWER_RESP_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_ANSWER_RESP_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_ANSWER_RESP                       	4


//Definition for CALL_MODEM_RELEASE_REQ
#define CALL_MODEM_RELEASE_REQ                            	0x09                                                 	
#define CALL_MODEM_RELEASE_REQ_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_RELEASE_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_RELEASE_REQ_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_RELEASE_REQ_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define CALL_MODEM_RELEASE_REQ_OFFSET_CAUSE               	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_RELEASE_REQ                       	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_RELEASE_RESP
#define CALL_MODEM_RELEASE_RESP                           	0x0A                                                 	
#define CALL_MODEM_RELEASE_RESP_OFFSET_TRID               	0 //size 1 byte(s)
#define CALL_MODEM_RELEASE_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define CALL_MODEM_RELEASE_RESP_OFFSET_CALLID             	2 //size 1 byte(s)
#define CALL_MODEM_RELEASE_RESP_OFFSET_SUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RELEASE_RESP                      	4


//Definition for CALL_MODEM_RELEASE_IND
#define CALL_MODEM_RELEASE_IND                            	0x0B                                                 	
#define CALL_MODEM_RELEASE_IND_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_RELEASE_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_RELEASE_IND_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_RELEASE_IND_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define CALL_MODEM_RELEASE_IND_OFFSET_CAUSE               	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_RELEASE_IND                       	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_TERMINATED_IND
#define CALL_MODEM_TERMINATED_IND                         	0x0C                                                 	
#define CALL_MODEM_TERMINATED_IND_OFFSET_TRID             	0 //size 1 byte(s)
#define CALL_MODEM_TERMINATED_IND_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define CALL_MODEM_TERMINATED_IND_OFFSET_CALLID           	2 //size 1 byte(s)
#define CALL_MODEM_TERMINATED_IND_OFFSET_SUBBLOCKS        	3 //size 1 byte(s)
#define CALL_MODEM_TERMINATED_IND_OFFSET_CAUSE            	4 //size 4 byte(s)
#define CALL_MODEM_TERMINATED_IND_OFFSET_MODE             	8 //size 4 byte(s)
#define SIZE_CALL_MODEM_TERMINATED_IND                    	12
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_STATUS_REQ
#define CALL_MODEM_STATUS_REQ                             	0x0D                                                 	
#define CALL_MODEM_STATUS_REQ_OFFSET_TRID                 	0 //size 1 byte(s)
#define CALL_MODEM_STATUS_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define CALL_MODEM_STATUS_REQ_OFFSET_CALLID               	2 //size 1 byte(s)
#define CALL_MODEM_STATUS_REQ_OFFSET_SUBBLOCKS            	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_STATUS_REQ                        	4


//Definition for CALL_MODEM_STATUS_RESP
#define CALL_MODEM_STATUS_RESP                            	0x0E                                                 	
#define CALL_MODEM_STATUS_RESP_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_STATUS_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_STATUS_RESP_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define CALL_MODEM_STATUS_RESP_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_STATUS_RESP                       	4


//Definition for CALL_MODEM_STATUS_IND
#define CALL_MODEM_STATUS_IND                             	0x0F                                                 	
#define CALL_MODEM_STATUS_IND_OFFSET_TRID                 	0 //size 1 byte(s)
#define CALL_MODEM_STATUS_IND_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define CALL_MODEM_STATUS_IND_OFFSET_CALLID               	2 //size 1 byte(s)
#define CALL_MODEM_STATUS_IND_OFFSET_SUBBLOCKS            	3 //size 1 byte(s)
#define CALL_MODEM_STATUS_IND_OFFSET_STATUS               	4 //size 4 byte(s)
#define CALL_MODEM_STATUS_IND_OFFSET_MODE                 	8 //size 4 byte(s)
#define SIZE_CALL_MODEM_STATUS_IND                        	12
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_PRESENT_IND
#define CALL_MODEM_PRESENT_IND                            	0x10                                              	
#define CALL_MODEM_PRESENT_IND_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_PRESENT_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_PRESENT_IND_OFFSET_CALLPRESENT         	2 //size 1 byte(s)
#define CALL_MODEM_PRESENT_IND_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_PRESENT_IND                       	4


//Definition for CALL_MODEM_CONTROL_REQ
#define CALL_MODEM_CONTROL_REQ                            	0x11                                              	
#define CALL_MODEM_CONTROL_REQ_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_CONTROL_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_CONTROL_REQ_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_CONTROL_REQ_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define CALL_MODEM_CONTROL_REQ_OFFSET_OPERATION           	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_CONTROL_REQ                       	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_CONTROL_RESP
#define CALL_MODEM_CONTROL_RESP                           	0x12                                              	
#define CALL_MODEM_CONTROL_RESP_OFFSET_TRID               	0 //size 1 byte(s)
#define CALL_MODEM_CONTROL_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define CALL_MODEM_CONTROL_RESP_OFFSET_CALLID             	2 //size 1 byte(s)
#define CALL_MODEM_CONTROL_RESP_OFFSET_SUBBLOCKS          	3 //size 1 byte(s)
#define CALL_MODEM_CONTROL_RESP_OFFSET_OPERATION          	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_CONTROL_RESP                      	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_CONTROL_IND
#define CALL_MODEM_CONTROL_IND                            	0x13                                              	
#define CALL_MODEM_CONTROL_IND_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_CONTROL_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_CONTROL_IND_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_CONTROL_IND_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define CALL_MODEM_CONTROL_IND_OFFSET_OPERATION           	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_CONTROL_IND                       	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_DTMF_SEND_REQ
#define CALL_MODEM_DTMF_SEND_REQ                          	0x17                                              	
#define CALL_MODEM_DTMF_SEND_REQ_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_DTMF_SEND_REQ_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_DTMF_SEND_REQ_OFFSET_CALLID            	2 //size 1 byte(s)
#define CALL_MODEM_DTMF_SEND_REQ_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_DTMF_SEND_REQ                     	4


//Definition for CALL_MODEM_DTMF_SEND_RESP
#define CALL_MODEM_DTMF_SEND_RESP                         	0x18                                              	
#define CALL_MODEM_DTMF_SEND_RESP_OFFSET_TRID             	0 //size 1 byte(s)
#define CALL_MODEM_DTMF_SEND_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define CALL_MODEM_DTMF_SEND_RESP_OFFSET_CALLID           	2 //size 1 byte(s)
#define CALL_MODEM_DTMF_SEND_RESP_OFFSET_SUBBLOCKS        	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_DTMF_SEND_RESP                    	4


//Definition for CALL_MODEM_DTMF_STOP_REQ
#define CALL_MODEM_DTMF_STOP_REQ                          	0x19                                              	
#define CALL_MODEM_DTMF_STOP_REQ_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_DTMF_STOP_REQ_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_DTMF_STOP_REQ_OFFSET_CALLID            	2 //size 1 byte(s)
#define CALL_MODEM_DTMF_STOP_REQ_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_DTMF_STOP_REQ                     	4


//Definition for CALL_MODEM_DTMF_STOP_RESP
#define CALL_MODEM_DTMF_STOP_RESP                         	0x1A                                              	
#define CALL_MODEM_DTMF_STOP_RESP_OFFSET_TRID             	0 //size 1 byte(s)
#define CALL_MODEM_DTMF_STOP_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define CALL_MODEM_DTMF_STOP_RESP_OFFSET_CALLID           	2 //size 1 byte(s)
#define CALL_MODEM_DTMF_STOP_RESP_OFFSET_SUBBLOCKS        	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_DTMF_STOP_RESP                    	4


//Definition for CALL_MODEM_DTMF_STATUS_IND
#define CALL_MODEM_DTMF_STATUS_IND                        	0x1B                                              	
#define CALL_MODEM_DTMF_STATUS_IND_OFFSET_TRID            	0 //size 1 byte(s)
#define CALL_MODEM_DTMF_STATUS_IND_OFFSET_MESSAGEID       	1 //size 1 byte(s)
#define CALL_MODEM_DTMF_STATUS_IND_OFFSET_CALLID          	2 //size 1 byte(s)
#define CALL_MODEM_DTMF_STATUS_IND_OFFSET_SUBBLOCKS       	3 //size 1 byte(s)
#define CALL_MODEM_DTMF_STATUS_IND_OFFSET_STATUS          	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_DTMF_STATUS_IND                   	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_DTMF_TONE_IND
#define CALL_MODEM_DTMF_TONE_IND                          	0x1C                                              	
#define CALL_MODEM_DTMF_TONE_IND_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_DTMF_TONE_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_DTMF_TONE_IND_OFFSET_CALLID            	2 //size 1 byte(s)
#define CALL_MODEM_DTMF_TONE_IND_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define CALL_MODEM_DTMF_TONE_IND_OFFSET_TONE              	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_DTMF_TONE_IND                     	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_RECONNECT_IND
#define CALL_MODEM_RECONNECT_IND                          	0x1E                                              	
#define CALL_MODEM_RECONNECT_IND_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_RECONNECT_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_RECONNECT_IND_OFFSET_RECONNECTSTATUS   	2 //size 1 byte(s)
#define CALL_MODEM_RECONNECT_IND_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RECONNECT_IND                     	4


//Definition for CALL_MODEM_PROPERTY_GET_REQ
#define CALL_MODEM_PROPERTY_GET_REQ                       	0x1F                                              	
#define CALL_MODEM_PROPERTY_GET_REQ_OFFSET_TRID           	0 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_GET_REQ_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_GET_REQ_OFFSET_PROPERTY       	2 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_GET_REQ_OFFSET_SUBBLOCKS      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_PROPERTY_GET_REQ                  	4


//Definition for CALL_MODEM_PROPERTY_GET_RESP
#define CALL_MODEM_PROPERTY_GET_RESP                      	0x20                                              	
#define CALL_MODEM_PROPERTY_GET_RESP_OFFSET_TRID          	0 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_GET_RESP_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_GET_RESP_OFFSET_PROPERTY      	2 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_GET_RESP_OFFSET_SUBBLOCKS     	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_PROPERTY_GET_RESP                 	4


//Definition for CALL_MODEM_PROPERTY_SET_REQ
#define CALL_MODEM_PROPERTY_SET_REQ                       	0x21                                              	
#define CALL_MODEM_PROPERTY_SET_REQ_OFFSET_TRID           	0 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_REQ_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_REQ_OFFSET_FILLERBYTE1    	2 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_REQ_OFFSET_SUBBLOCKS      	3 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_REQ_OFFSET_PROPERTYINFO   	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_PROPERTY_SET_REQ                  	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_PROPERTY_SET_RESP
#define CALL_MODEM_PROPERTY_SET_RESP                      	0x22                                              	
#define CALL_MODEM_PROPERTY_SET_RESP_OFFSET_TRID          	0 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_RESP_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_RESP_OFFSET_FILLERBYTE1   	2 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_RESP_OFFSET_SUBBLOCKS     	3 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_RESP_OFFSET_PROPERTYINFO  	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_PROPERTY_SET_RESP                 	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_PROPERTY_SET_IND
#define CALL_MODEM_PROPERTY_SET_IND                       	0x23                                              	
#define CALL_MODEM_PROPERTY_SET_IND_OFFSET_TRID           	0 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_IND_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_IND_OFFSET_PROPERTY       	2 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_IND_OFFSET_SUBBLOCKS      	3 //size 1 byte(s)
#define CALL_MODEM_PROPERTY_SET_IND_OFFSET_PROPERTYINFO   	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_PROPERTY_SET_IND                  	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_EMERG_NBR_CHECK_REQ
#define CALL_MODEM_EMERG_NBR_CHECK_REQ                    	0x28                                              	
#define CALL_MODEM_EMERG_NBR_CHECK_REQ_OFFSET_TRID        	0 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_CHECK_REQ_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_CHECK_REQ_OFFSET_FILLERBYTE1 	2 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_CHECK_REQ_OFFSET_SUBBLOCKS   	3 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_CHECK_REQ_OFFSET_NUMBER      	4 //size 8 byte(s)
#define SIZE_CALL_MODEM_EMERG_NBR_CHECK_REQ               	12
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_EMERG_NBR_CHECK_RESP
#define CALL_MODEM_EMERG_NBR_CHECK_RESP                   	0x29                                              	
#define CALL_MODEM_EMERG_NBR_CHECK_RESP_OFFSET_TRID       	0 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_CHECK_RESP_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_CHECK_RESP_OFFSET_STATUS     	2 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_CHECK_RESP_OFFSET_SUBBLOCKS  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_EMERG_NBR_CHECK_RESP              	4


//Definition for CALL_MODEM_EMERG_NBR_GET_REQ
#define CALL_MODEM_EMERG_NBR_GET_REQ                      	0x26                                              	
#define CALL_MODEM_EMERG_NBR_GET_REQ_OFFSET_TRID          	0 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_GET_REQ_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_GET_REQ_OFFSET_FILLERBYTE1   	2 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_GET_REQ_OFFSET_SUBBLOCKS     	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_EMERG_NBR_GET_REQ                 	4


//Definition for CALL_MODEM_EMERG_NBR_GET_RESP
#define CALL_MODEM_EMERG_NBR_GET_RESP                     	0x27                                              	
#define CALL_MODEM_EMERG_NBR_GET_RESP_OFFSET_TRID         	0 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_GET_RESP_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_GET_RESP_OFFSET_FILLERBYTE1  	2 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_GET_RESP_OFFSET_SUBBLOCKS    	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_EMERG_NBR_GET_RESP                	4


//Definition for CALL_MODEM_EMERG_NBR_MODIFY_REQ
#define CALL_MODEM_EMERG_NBR_MODIFY_REQ                   	0x24                                              	
#define CALL_MODEM_EMERG_NBR_MODIFY_REQ_OFFSET_TRID       	0 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_MODIFY_REQ_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_MODIFY_REQ_OFFSET_OPERATION  	2 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_MODIFY_REQ_OFFSET_SUBBLOCKS  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_EMERG_NBR_MODIFY_REQ              	4


//Definition for CALL_MODEM_EMERG_NBR_MODIFY_RESP
#define CALL_MODEM_EMERG_NBR_MODIFY_RESP                  	0x25                                              	
#define CALL_MODEM_EMERG_NBR_MODIFY_RESP_OFFSET_TRID      	0 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_MODIFY_RESP_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_MODIFY_RESP_OFFSET_OPERATION 	2 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_MODIFY_RESP_OFFSET_SUBBLOCKS 	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_EMERG_NBR_MODIFY_RESP             	4


//Definition for CALL_MODEM_NOTIFICATION_IND
#define CALL_MODEM_NOTIFICATION_IND                       	0xA0                                              	
#define CALL_MODEM_NOTIFICATION_IND_OFFSET_TRID           	0 //size 1 byte(s)
#define CALL_MODEM_NOTIFICATION_IND_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define CALL_MODEM_NOTIFICATION_IND_OFFSET_CALLID         	2 //size 1 byte(s)
#define CALL_MODEM_NOTIFICATION_IND_OFFSET_SUBBLOCKS      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_NOTIFICATION_IND                  	4


//Definition for CALL_MODEM_BLACKLIST_CLEAR_REQ
#define CALL_MODEM_BLACKLIST_CLEAR_REQ                    	0xA4                                              	
#define CALL_MODEM_BLACKLIST_CLEAR_REQ_OFFSET_TRID        	0 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_CLEAR_REQ_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_CLEAR_REQ_OFFSET_FILLERBYTE1 	2 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_CLEAR_REQ_OFFSET_SUBBLOCKS   	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_BLACKLIST_CLEAR_REQ               	4


//Definition for CALL_MODEM_BLACKLIST_CLEAR_RESP
#define CALL_MODEM_BLACKLIST_CLEAR_RESP                   	0xA5                                              	
#define CALL_MODEM_BLACKLIST_CLEAR_RESP_OFFSET_TRID       	0 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_CLEAR_RESP_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_CLEAR_RESP_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_CLEAR_RESP_OFFSET_SUBBLOCKS  	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_BLACKLIST_CLEAR_RESP              	4


//Definition for CALL_MODEM_BLACKLIST_TIMER_IND
#define CALL_MODEM_BLACKLIST_TIMER_IND                    	0xA6                                              	
#define CALL_MODEM_BLACKLIST_TIMER_IND_OFFSET_TRID        	0 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_TIMER_IND_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_TIMER_IND_OFFSET_DURATION    	2 //size 1 byte(s)
#define CALL_MODEM_BLACKLIST_TIMER_IND_OFFSET_SUBBLOCKS   	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_BLACKLIST_TIMER_IND               	4


//Definition for CALL_MODEM_BC_GET_REQ
#define CALL_MODEM_BC_GET_REQ                             	0xAA                                              	
#define CALL_MODEM_BC_GET_REQ_OFFSET_TRID                 	0 //size 1 byte(s)
#define CALL_MODEM_BC_GET_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define CALL_MODEM_BC_GET_REQ_OFFSET_FILLERBYTE1          	2 //size 1 byte(s)
#define CALL_MODEM_BC_GET_REQ_OFFSET_SUBBLOCKS            	3 //size 1 byte(s)
#define CALL_MODEM_BC_GET_REQ_OFFSET_MODE                 	4 //size 4 byte(s)
#define SIZE_CALL_MODEM_BC_GET_REQ                        	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_MODEM_BC_GET_RESP
#define CALL_MODEM_BC_GET_RESP                            	0xAB                                              	
#define CALL_MODEM_BC_GET_RESP_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_BC_GET_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_BC_GET_RESP_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define CALL_MODEM_BC_GET_RESP_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_BC_GET_RESP                       	4


//Definition for CALL_MODEM_BC_CHECK_REQ
#define CALL_MODEM_BC_CHECK_REQ                           	0xAC                                              	
#define CALL_MODEM_BC_CHECK_REQ_OFFSET_TRID               	0 //size 1 byte(s)
#define CALL_MODEM_BC_CHECK_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define CALL_MODEM_BC_CHECK_REQ_OFFSET_FILLERBYTE1        	2 //size 1 byte(s)
#define CALL_MODEM_BC_CHECK_REQ_OFFSET_SUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_BC_CHECK_REQ                      	4


//Definition for CALL_MODEM_BC_CHECK_RESP
#define CALL_MODEM_BC_CHECK_RESP                          	0xAD                                              	
#define CALL_MODEM_BC_CHECK_RESP_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_BC_CHECK_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_BC_CHECK_RESP_OFFSET_RESULT            	2 //size 1 byte(s)
#define CALL_MODEM_BC_CHECK_RESP_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_BC_CHECK_RESP                     	4


//Definition for CALL_MODEM_SS_DATA_IND
#define CALL_MODEM_SS_DATA_IND                            	0xAF                                              	
#define CALL_MODEM_SS_DATA_IND_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_SS_DATA_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_SS_DATA_IND_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_SS_DATA_IND_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define CALL_MODEM_SS_DATA_IND_OFFSET_FILLERBYTE2         	4 //size 1 byte(s)
#define CALL_MODEM_SS_DATA_IND_OFFSET_FILLERBYTE3         	5 //size 1 byte(s)
#define CALL_MODEM_SS_DATA_IND_OFFSET_FILLERBYTE4         	6 //size 1 byte(s)
#define CALL_MODEM_SS_DATA_IND_OFFSET_SUBBLOCKS           	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SS_DATA_IND                       	8


//Definition for CALL_MODEM_EMERG_NBR_IND
#define CALL_MODEM_EMERG_NBR_IND                          	0x31                                              	
#define CALL_MODEM_EMERG_NBR_IND_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_IND_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define CALL_MODEM_EMERG_NBR_IND_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_EMERG_NBR_IND                     	4


//Definition for CALL_MODEM_SERVICE_DENIED_IND
#define CALL_MODEM_SERVICE_DENIED_IND                     	0x32                                              	
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_TRID         	0 //size 1 byte(s)
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_REQMESSAGEID 	2 //size 1 byte(s)
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_ADDITIONALINFO	3 //size 1 byte(s)
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_CALLID       	4 //size 1 byte(s)
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_FILLERBYTE1  	5 //size 1 byte(s)
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_FILLERBYTE2  	6 //size 1 byte(s)
#define CALL_MODEM_SERVICE_DENIED_IND_OFFSET_SUBBLOCKS    	7 //size 1 byte(s)
#define SIZE_CALL_MODEM_SERVICE_DENIED_IND                	8


//Definition for CALL_MODEM_RELEASE_END_REQ
#define CALL_MODEM_RELEASE_END_REQ                        	0x34                                              	
#define CALL_MODEM_RELEASE_END_REQ_OFFSET_TRID            	0 //size 1 byte(s)
#define CALL_MODEM_RELEASE_END_REQ_OFFSET_MESSAGEID       	1 //size 1 byte(s)
#define CALL_MODEM_RELEASE_END_REQ_OFFSET_CALLID          	2 //size 1 byte(s)
#define CALL_MODEM_RELEASE_END_REQ_OFFSET_SUBBLOCKS       	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RELEASE_END_REQ                   	4


//Definition for CALL_MODEM_RELEASE_END_RESP
#define CALL_MODEM_RELEASE_END_RESP                       	0x35                                              	
#define CALL_MODEM_RELEASE_END_RESP_OFFSET_TRID           	0 //size 1 byte(s)
#define CALL_MODEM_RELEASE_END_RESP_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define CALL_MODEM_RELEASE_END_RESP_OFFSET_CALLID         	2 //size 1 byte(s)
#define CALL_MODEM_RELEASE_END_RESP_OFFSET_SUBBLOCKS      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RELEASE_END_RESP                  	4


//Definition for CALL_MODEM_USER_CONNECT_IND
#define CALL_MODEM_USER_CONNECT_IND                       	0x33                                              	
#define CALL_MODEM_USER_CONNECT_IND_OFFSET_TRID           	0 //size 1 byte(s)
#define CALL_MODEM_USER_CONNECT_IND_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define CALL_MODEM_USER_CONNECT_IND_OFFSET_USERCONN       	2 //size 1 byte(s)
#define CALL_MODEM_USER_CONNECT_IND_OFFSET_SUBBLOCKS      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_USER_CONNECT_IND                  	4


//Definition for CALL_MODEM_RESOURCE_IND
#define CALL_MODEM_RESOURCE_IND                           	0x40                                              	
#define CALL_MODEM_RESOURCE_IND_OFFSET_TRID               	0 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_IND_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_IND_OFFSET_CALLID             	2 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_IND_OFFSET_SUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RESOURCE_IND                      	4


//Definition for CALL_MODEM_RESOURCE_REQ
#define CALL_MODEM_RESOURCE_REQ                           	0x41                                              	
#define CALL_MODEM_RESOURCE_REQ_OFFSET_TRID               	0 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_REQ_OFFSET_CALLID             	2 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_REQ_OFFSET_SUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RESOURCE_REQ                      	4


//Definition for CALL_MODEM_RESOURCE_RESP
#define CALL_MODEM_RESOURCE_RESP                          	0x42                                              	
#define CALL_MODEM_RESOURCE_RESP_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_RESP_OFFSET_CALLID            	2 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_RESP_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RESOURCE_RESP                     	4


//Definition for CALL_MODEM_RESOURCE_CLEAR_IND
#define CALL_MODEM_RESOURCE_CLEAR_IND                     	0x43                                              	
#define CALL_MODEM_RESOURCE_CLEAR_IND_OFFSET_TRID         	0 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CLEAR_IND_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CLEAR_IND_OFFSET_CALLID       	2 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CLEAR_IND_OFFSET_SUBBLOCKS    	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RESOURCE_CLEAR_IND                	4


//Definition for CALL_MODEM_RESOURCE_CONF_IND
#define CALL_MODEM_RESOURCE_CONF_IND                      	0x46                                              	
#define CALL_MODEM_RESOURCE_CONF_IND_OFFSET_TRID          	0 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_IND_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_IND_OFFSET_CONFSTATUS    	2 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_IND_OFFSET_SUBBLOCKS     	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RESOURCE_CONF_IND                 	4


//Definition for CALL_MODEM_RESOURCE_CONF_REQ
#define CALL_MODEM_RESOURCE_CONF_REQ                      	0x44                                              	
#define CALL_MODEM_RESOURCE_CONF_REQ_OFFSET_TRID          	0 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_REQ_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_REQ_OFFSET_CONFOPERATION 	2 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_REQ_OFFSET_SUBBLOCKS     	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RESOURCE_CONF_REQ                 	4


//Definition for CALL_MODEM_RESOURCE_CONF_RESP
#define CALL_MODEM_RESOURCE_CONF_RESP                     	0x45                                              	
#define CALL_MODEM_RESOURCE_CONF_RESP_OFFSET_TRID         	0 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_RESP_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_RESP_OFFSET_CONFOPERATION	2 //size 1 byte(s)
#define CALL_MODEM_RESOURCE_CONF_RESP_OFFSET_SUBBLOCKS    	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_RESOURCE_CONF_RESP                	4


//Definition for CALL_MODEM_TERMINATE_REQ
#define CALL_MODEM_TERMINATE_REQ                          	0x47                                              	
#define CALL_MODEM_TERMINATE_REQ_OFFSET_TRID              	0 //size 1 byte(s)
#define CALL_MODEM_TERMINATE_REQ_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define CALL_MODEM_TERMINATE_REQ_OFFSET_CALLID            	2 //size 1 byte(s)
#define CALL_MODEM_TERMINATE_REQ_OFFSET_SUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_TERMINATE_REQ                     	4


//Definition for CALL_MODEM_TERMINATE_RESP
#define CALL_MODEM_TERMINATE_RESP                         	0x48                                              	
#define CALL_MODEM_TERMINATE_RESP_OFFSET_TRID             	0 //size 1 byte(s)
#define CALL_MODEM_TERMINATE_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define CALL_MODEM_TERMINATE_RESP_OFFSET_CALLID           	2 //size 1 byte(s)
#define CALL_MODEM_TERMINATE_RESP_OFFSET_SUBBLOCKS        	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_TERMINATE_RESP                    	4


//Definition for CALL_MODEM_NETWORK_CONN_IND
#define CALL_MODEM_NETWORK_CONN_IND                       	0x49                                              	
#define CALL_MODEM_NETWORK_CONN_IND_OFFSET_TRID           	0 //size 1 byte(s)
#define CALL_MODEM_NETWORK_CONN_IND_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define CALL_MODEM_NETWORK_CONN_IND_OFFSET_CALLID         	2 //size 1 byte(s)
#define CALL_MODEM_NETWORK_CONN_IND_OFFSET_SUBBLOCKS      	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_NETWORK_CONN_IND                  	4


//Definition for CALL_MODEM_MESSAGE_IND
#define CALL_MODEM_MESSAGE_IND                            	0x50                                              	
#define CALL_MODEM_MESSAGE_IND_OFFSET_TRID                	0 //size 1 byte(s)
#define CALL_MODEM_MESSAGE_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define CALL_MODEM_MESSAGE_IND_OFFSET_CALLID              	2 //size 1 byte(s)
#define CALL_MODEM_MESSAGE_IND_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_CALL_MODEM_MESSAGE_IND                       	4

#endif