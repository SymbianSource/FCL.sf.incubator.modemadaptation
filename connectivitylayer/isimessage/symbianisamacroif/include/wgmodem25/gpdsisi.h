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
* Name      : PN_GPDS [ 0x31 ] Resource General Packet Data Server
* Version   : 019.001
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_GPDSISI_H_
#define _SYMBIAN_ISI_GPDSISI_H_

#define GENERAL_PACKET_DATA_SERVER_ISI_VERSION_Z	 19
#define GENERAL_PACKET_DATA_SERVER_ISI_VERSION_Y	 3


//CONSTANTS
#define GPDS_IP_DATAGRAM_MAX_LENGTH                       	0x5DC                                             	
#define GPDS_PPP_INFO_MAX_LENGTH                          	0x5DC                                             	
#define GPDS_QOS99_SUBSCRIBED                             	0x00                                                 	
#define GPDS_QOS99_DEFAULT                                	0xFFFF                                            	
#define GPDS_MAX_USERNAME_SIZE                            	0x35                                              	
#define GPDS_MAX_PASSWORD_SIZE                            	0x35                                              	
#define GPDS_MAX_CHALLENGE_SIZE                           	0x35                                              	
#define GPDS_MAX_RESPONSE_SIZE                            	0x34                                              	
#define GPDS_MAX_CID_LIST_SIZE                            	0x0E                                                 	
#define GPDS_ANY_SIZE                                     	0x01                                                 	
#define GPDS_CID_VOID                                     	0xFF                                              	
#define GPDS_CID_ALL                                      	0xFE                                              	
#define GPDS_DISABLE                                      	0x00                                                 	


//TABLES

//ConstantTable for GPDS_PPP_MODE
#define GPDS_LL_FRAMED_PPP                                	0x00                                                 	
#define GPDS_LL_NONFRAMED_PPP                             	0x01                                                 	
#define GPDS_LL_PLAIN                                     	0x02                                                 	

//ConstantTable for GPDS_STATUS
#define GPDS_ERROR                                        	0x00                                                 	
#define GPDS_OK                                           	0x01                                                 	
#define GPDS_FAIL                                         	0x02                                                 	

//ConstantTable for GPDS_ATTACH_MODE
#define GPDS_ATTACH_MODE_MANUAL                           	0x00                                                 	
#define GPDS_ATTACH_MODE_AUTOMATIC                        	0x01                                                 	
#define GPDS_ATTACH_MODE_DEFAULT                          	0xFF                                              	

//ConstantTable for GPDS_ATTACH_STATUS
#define GPDS_DETACHED                                     	0x00                                                 	
#define GPDS_ATTACHED                                     	0x01                                                 	

//ConstantTable for GPDS_MT_ACT_MODE
#define GPDS_MT_ACT_MODE_REJECT                           	0x00                                                 	
#define GPDS_MT_ACT_MODE_ACCEPT                           	0x01                                                 	
#define GPDS_MT_ACT_MODE_DEFAULT                          	0xFF                                              	

//ConstantTable for GPDS_CLASSC_MODE
#define GPDS_CLASSC_MODE_GPRS                             	0x00                                                 	
#define GPDS_CLASSC_MODE_GSM                              	0x01                                                 	
#define GPDS_CLASSC_MODE_DEFAULT                          	0xFF                                              	

//ConstantTable for GPDS_PRECEDENCE
#define GPDS_QOS_PREC_0                                   	0x00                                                 	
#define GPDS_QOS_PREC_1                                   	0x01                                                 	
#define GPDS_QOS_PREC_2                                   	0x02                                                 	
#define GPDS_QOS_PREC_3                                   	0x03                                                 	
#define GPDS_QOS_PREC_DEFAULT                             	0xFF                                              	

//ConstantTable for GPDS_DELAY
#define GPDS_QOS_DELC_0                                   	0x00                                                 	
#define GPDS_QOS_DELC_1                                   	0x01                                                 	
#define GPDS_QOS_DELC_2                                   	0x02                                                 	
#define GPDS_QOS_DELC_3                                   	0x03                                                 	
#define GPDS_QOS_DELC_4                                   	0x04                                                 	
#define GPDS_QOS_DELC_DEFAULT                             	0xFF                                              	

//ConstantTable for GPDS_RELIABILITY
#define GPDS_QOS_RELC_0                                   	0x00                                                 	
#define GPDS_QOS_RELC_1                                   	0x01                                                 	
#define GPDS_QOS_RELC_2                                   	0x02                                                 	
#define GPDS_QOS_RELC_3                                   	0x03                                                 	
#define GPDS_QOS_RELC_4                                   	0x04                                                 	
#define GPDS_QOS_RELC_5                                   	0x05                                                 	
#define GPDS_QOS_RELC_DEFAULT                             	0xFF                                              	

//ConstantTable for GPDS_PEAK_THROUGHPUT
#define GPDS_QOS_PETC_0                                   	0x00                                                 	
#define GPDS_QOS_PETC_1                                   	0x01                                                 	
#define GPDS_QOS_PETC_2                                   	0x02                                                 	
#define GPDS_QOS_PETC_3                                   	0x03                                                 	
#define GPDS_QOS_PETC_4                                   	0x04                                                 	
#define GPDS_QOS_PETC_5                                   	0x05                                                 	
#define GPDS_QOS_PETC_6                                   	0x06                                                 	
#define GPDS_QOS_PETC_7                                   	0x07                                                 	
#define GPDS_QOS_PETC_8                                   	0x08                                                 	
#define GPDS_QOS_PETC_9                                   	0x09                                                 	
#define GPDS_QOS_PETC_DEFAULT                             	0xFF                                              	

//ConstantTable for GPDS_MEAN_THROUGHPUT
#define GPDS_QOS_METC_0                                   	0x00                                                 	
#define GPDS_QOS_METC_1                                   	0x01                                                 	
#define GPDS_QOS_METC_2                                   	0x02                                                 	
#define GPDS_QOS_METC_3                                   	0x03                                                 	
#define GPDS_QOS_METC_4                                   	0x04                                                 	
#define GPDS_QOS_METC_5                                   	0x05                                                 	
#define GPDS_QOS_METC_6                                   	0x06                                                 	
#define GPDS_QOS_METC_7                                   	0x07                                                 	
#define GPDS_QOS_METC_8                                   	0x08                                                 	
#define GPDS_QOS_METC_9                                   	0x09                                                 	
#define GPDS_QOS_METC_10                                  	0x0A                                                 	
#define GPDS_QOS_METC_11                                  	0x0B                                                 	
#define GPDS_QOS_METC_12                                  	0x0C                                                 	
#define GPDS_QOS_METC_13                                  	0x0D                                                 	
#define GPDS_QOS_METC_14                                  	0x0E                                                 	
#define GPDS_QOS_METC_15                                  	0x0F                                                 	
#define GPDS_QOS_METC_16                                  	0x10                                              	
#define GPDS_QOS_METC_17                                  	0x11                                              	
#define GPDS_QOS_METC_18                                  	0x12                                              	
#define GPDS_QOS_METC_31                                  	0x1F                                              	
#define GPDS_QOS_METC_DEFAULT                             	0xFF                                              	

//ConstantTable for GPDS_CAUSE
#define GPDS_CAUSE_UNKNOWN                                	0x00                                                 	
#define GPDS_CAUSE_IMSI                                   	0x02                                                 	
#define GPDS_CAUSE_MS_ILLEGAL                             	0x03                                                 	
#define GPDS_CAUSE_ME_ILLEGAL                             	0x06                                                 	
#define GPDS_CAUSE_GPRS_NOT_ALLOWED                       	0x07                                                 	
#define GPDS_NOT_ALLOWED                                  	0x08                                                 	
#define GPDS_CAUSE_MS_IDENTITY                            	0x09                                                 	
#define GPDS_CAUSE_DETACH                                 	0x0A                                                 	
#define GPDS_PLMN_NOT_ALLOWED                             	0x0B                                                 	
#define GPDS_LA_NOT_ALLOWED                               	0x0C                                                 	
#define GPDS_ROAMING_NOT_ALLOWED                          	0x0D                                                 	
#define GPDS_CAUSE_GPRS_NOT_ALLOWED_IN_PLMN               	0x0E                                                 	
#define GPDS_CAUSE_NO_SUITABLE_CELLS_IN_LA                	0x0F                                                 	
#define GPDS_CAUSE_MSC_NOT_REACH                          	0x10                                              	
#define GPDS_CAUSE_PLMN_FAIL                              	0x11                                              	
#define GPDS_CAUSE_NETWORK_CONGESTION                     	0x16                                              	
#define GPDS_CAUSE_LLC_SNDCP_FAILURE                      	0x19                                              	
#define GPDS_CAUSE_RESOURCE_INSUFF                        	0x1A                                              	
#define GPDS_CAUSE_APN                                    	0x1B                                              	
#define GPDS_CAUSE_PDP_UNKNOWN                            	0x1C                                              	
#define GPDS_CAUSE_AUTHENTICATION                         	0x1D                                              	
#define GPDS_CAUSE_ACT_REJECT_GGSN                        	0x1E                                              	
#define GPDS_CAUSE_ACT_REJECT                             	0x1F                                              	
#define GPDS_CAUSE_SERV_OPT_NOT_SUPPORTED                 	0x20                                              	
#define GPDS_CAUSE_SERV_OPT_NOT_SUBSCRIBED                	0x21                                              	
#define GPDS_CAUSE_SERV_OPT_OUT_OF_ORDER                  	0x22                                              	
#define GPDS_CAUSE_NSAPI_ALREADY_USED                     	0x23                                              	
#define GPDS_CAUSE_DEACT_REGULAR                          	0x24                                              	
#define GPDS_CAUSE_QOS                                    	0x25                                              	
#define GPDS_CAUSE_NETWORK_FAIL                           	0x26                                              	
#define GPDS_CAUSE_REACTIVATION_REQ                       	0x27                                              	
#define GPDS_CAUSE_FEAT_NOT_SUPPORTED                     	0x28                                              	
#define GPDS_CAUSE_TFT_SEMANTIC_ERROR                     	0x29                                              	
#define GPDS_CAUSE_TFT_SYNTAX_ERROR                       	0x2A                                              	
#define GPDS_CAUSE_CONTEXT_UNKNOWN                        	0x2B                                              	
#define GPDS_CAUSE_FILTER_SEMANTIC_ERROR                  	0x2C                                              	
#define GPDS_CAUSE_FILTER_SYNTAX_ERROR                    	0x2D                                              	
#define GPDS_CAUSE_CONT_WITHOUT_TFT                       	0x2E                                              	
#define GPDS_CAUSE_MULTICAST_MEMBERSHIP_TIMEOUT           	0x2F                                              	
#define GPDS_CAUSE_INVALID_MANDATORY_INFO                 	0x60                                              	
#define GPDS_CAUSE_MSG_TYPE_NON_EXISTENTOR_NOT_IMPLTD     	0x61                                              	
#define GPDS_CAUSE_MSG_TYPE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE	0x62                                              	
#define GPDS_CAUSE_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED     	0x63                                              	
#define GPDS_CAUSE_CONDITIONAL_IE_ERROR                   	0x64                                              	
#define GPDS_CAUSE_MSG_NOT_COMPATIBLE_WITH_PROTOCOL_STATE 	0x65                                              	
#define GPDS_CAUSE_UNSPECIFIED                            	0x6F                                              	
#define GPDS_CAUSE_APN_INCOMPATIBLE_WITH_CURR_CTXT        	0x70                                              	
#define GPDS_CAUSE_FDN                                    	0xA0                                              	
#define GPDS_CAUSE_USER_ABORT                             	0xA1                                              	
#define GPDS_CAUSE_CS_INACTIVE                            	0xA2                                              	
#define GPDS_CAUSE_CSD_OVERRIDE                           	0xA3                                              	
#define GPDS_CAUSE_APN_CONTROL                            	0xA4                                              	
#define GPDS_CAUSE_CALL_CONTROL                           	0xA5                                              	
#define GPDS_CAUSE_TEMPERATURE_LIMIT                      	0xA6                                              	
#define GPDS_CAUSE_RETRY_COUNTER_EXPIRED                  	0xC8                                              	
#define GPDS_CAUSE_NO_CONNECTION                          	0xC9                                              	
#define GPDS_CAUSE_DETACHED                               	0xF5                                              	
#define GPDS_CAUSE_NO_SERVICE_POWER_SAVE                  	0xF7                                              	
#define GPDS_CAUSE_SIM_REMOVED                            	0xF9                                              	
#define GPDS_CAUSE_POWER_OFF                              	0xFA                                              	
#define GPDS_CAUSE_LAI_FORBIDDEN_NATIONAL_ROAM_LIST       	0xFB                                              	
#define GPDS_CAUSE_LAI_FORBIDDEN_REG_PROVISION_LIST       	0xFC                                              	
#define GPDS_CAUSE_ACCESS_BARRED                          	0xFD                                              	
#define GPDS_CAUSE_FATAL_FAILURE                          	0xFE                                              	
#define GPDS_CAUSE_AUT_FAILURE                            	0xFF                                              	

//ConstantTable for GPDS_CLIENT_TYPE
#define GPDS_CONTEXT_CLIENT_SOCKET                        	0x01                                                 	
#define GPDS_CONTEXT_CLIENT_DIAL_UP                       	0x02                                                 	
#define GPDS_CONTEXT_CLIENT_UNKNOWN                       	0x00                                                 	

//ConstantTable for GPDS_CONTEXT_TYPE
#define GPDS_CONT_TYPE_NORMAL                             	0x00                                                 	
#define GPDS_CONT_TYPE_NWI                                	0x01                                                 	
#define GPDS_CONT_TYPE_SEC                                	0x02                                                 	

//ConstantTable for GPDS_ATTACH_TYPE
#define GPDS_ATTACH_TYPE_GPRS                             	0x01                                                 	
#define GPDS_ATTACH_TYPE_COMBINED                         	0x03                                                 	

//ConstantTable for GPDS_DETACH_TYPE
#define GPDS_DETACH_TYPE_GPRS_MO_POFF                     	0x00                                                 	
#define GPDS_DETACH_TYPE_GPRS_MO                          	0x01                                                 	
#define GPDS_DETACH_TYPE_MO_COMBINED_POFF                 	0x02                                                 	
#define GPDS_DETACH_TYPE_MO_COMBINED                      	0x03                                                 	
#define GPDS_DETACH_TYPE_MT_REATTACH_REQ                  	0x04                                                 	
#define GPDS_DETACH_TYPE_MT                               	0x05                                                 	
#define GPDS_DETACH_TYPE_MO_REATTACH                      	0x06                                                 	
#define GPDS_DETACH_TYPE_MO_COMB_REATTACH                 	0x07                                                 	

//ConstantTable for GPDS_TRANSFER_STATUS
#define GPDS_TRANSFER_NOT_AVAIL                           	0x00                                                 	
#define GPDS_TRANSFER_AVAIL                               	0x01                                                 	

//ConstantTable for GPDS_TRANSFER_CAUSE
#define GPDS_TRANSFER_CAUSE_ATTACHED                      	0x02                                                 	
#define GPDS_TRANSFER_CAUSE_DETACHED                      	0x03                                                 	
#define GPDS_TRANSFER_CAUSE_RESUMED                       	0x04                                                 	
#define GPDS_TRANSFER_CAUSE_SUSPENDED_NO_COVERAGE         	0x05                                                 	
#define GPDS_TRANSFER_CAUSE_SUSPENDED_CALL_SMS            	0x07                                                 	
#define GPDS_TRANSFER_CAUSE_SUSPENDED_CALL                	0x08                                                 	
#define GPDS_TRANSFER_CAUSE_SUSPENDED_RAU                 	0x09                                                 	
#define GPDS_TRANSFER_CAUSE_SUSPENDED_LU                  	0x0A                                                 	
#define GPDS_TRANSFER_CAUSE_DSAC_RESTRICTION              	0x0B                                                 	

//ConstantTable for GPDS_DIRECTION
#define GPDS_RADIO_UPLINK                                 	0x00                                                 	
#define GPDS_RADIO_DOWNLINK                               	0x01                                                 	
#define GPDS_RADIO_BIDIRECTION                            	0x02                                                 	

//ConstantTable for GPDS_RADIO_STATUS
#define GPDS_RADIO_ON                                     	0x01                                                 	
#define GPDS_RADIO_OFF                                    	0x00                                                 	

//ConstantTable for GPDS_FORCED_READY_STATE
#define GPDS_FORCED_READY_STATE_OFF                       	0x00                                                 	
#define GPDS_FORCED_READY_STATE_ON                        	0x01                                                 	

//ConstantTable for GPDS_FOLLOW_ON_REQUEST_MODE
#define GPDS_FOLLOW_OFF                                   	0x00                                                 	
#define GPDS_FOLLOW_ON                                    	0x01                                                 	

//ConstantTable for GPDS_PDP_TYPE
#define GPDS_PDP_TYPE_PPP                                 	0x01                                                 	
#define GPDS_PDP_TYPE_IPV4                                	0x21                                              	
#define GPDS_PDP_TYPE_IPV6                                	0x57                                              	
#define GPDS_PDP_TYPE_DEFAULT                             	0xFF                                              	

//ConstantTable for GPDS_AOL_CONTEXT
#define GPDS_AOL_CTX_NOT_ACTIVE                           	0x00                                                 	
#define GPDS_AOL_CTX_HPLMN_ACTIVE                         	0x01                                                 	
#define GPDS_AOL_CTX_VPLMN_ACTIVE                         	0x02                                                 	
#define GPDS_AOL_CTX_ACTIVE                               	0x03                                                 	
#define GPDS_AOL_CTX_DEFAULT                              	0xFF                                              	

//ConstantTable for GPDS_RESOURCE_CONTROL_RESULT
#define GPDS_ALLOWED                                      	0x01                                                 	
#define GPDS_MODIFIED                                     	0x02                                                 	
#define GPDS_REJECTED                                     	0x03                                                 	

//ConstantTable for GPDS_RESOURCE_CONF_OPERATION
#define GPDS_RESOURCE_CONF_GET                            	0x01                                                 	
#define GPDS_RESOURCE_CONF_SET                            	0x02                                                 	

//ConstantTable for GPDS_RESOURCE_CONF_STATUS
#define GPDS_RESOURCE_CONF_READY                          	0x01                                                 	
#define GPDS_RESOURCE_CONF_RECONFIGURED                   	0x02                                                 	

//BitmaskTables for GPDS_RESOURCE_IDS
#define GPDS_RES_ID_CC_FOR_GPRS                           	0x01                                                 	

//BitmaskTables for GPDS_RESOURCE_IDS_MASK
#define GPDS_RES_ID_MASK_CC_FOR_GPRS                      	0x01                                                 	

//BitmaskTables for GPDS_RESOURCE_IDS_CONF_REQ
#define GPDS_RES_ID_CONF_CC_FOR_GPRS                      	0x01                                                 	

//ConstantTable for GPDS_DATA_COUNTER_SETTINGS
#define GPDS_DATA_COUNTER_SETTING_DEFAULT                 	0x00                                                 	
#define GPDS_DATA_COUNTER_SETTING_TIME                    	0x01                                                 	
#define GPDS_DATA_COUNTER_SETTING_TRANSFER                	0x02                                                 	
#define GPDS_DATA_COUNTER_SETTING_BOTH                    	0x03                                                 	


//ConstantTable for FtdTable
#define FTD_SB_GPDS_ATT_AND_PDP                           	0x10                                              	
#define FTD_SB_GPDS_NSAPI_1                               	0x11                                              	
#define FTD_SB_GPDS_SAPI_1                                	0x12                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_1_1                       	0x13                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_1_2                       	0x14                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_1_3                       	0x15                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_1_4                       	0x16                                              	
#define FTD_SB_GPDS_NSAPI_2                               	0x17                                              	
#define FTD_SB_GPDS_SAPI_2                                	0x18                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_2_1                       	0x19                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_2_2                       	0x1A                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_2_3                       	0x1B                                              	
#define FTD_SB_GPDS_PDP_ADDRESS_2_4                       	0x1C                                              	
#define FTD_SB_PDI_FCS_FAIL                               	0x1D                                              	
#define FTD_SB_PDI_MRU                                    	0x1E                                              	
#define FTD_SB_PDI_MTU                                    	0x1F                                              	
#define FTD_SB_SN_RELC_1                                  	0x20                                              	
#define FTD_SB_SN_PREC_1                                  	0x21                                              	
#define FTD_SB_SN_DELC_1                                  	0x22                                              	
#define FTD_SB_SN_PTPC_1                                  	0x23                                              	
#define FTD_SB_SN_MTPC_1                                  	0x24                                              	
#define FTD_SB_SN_RPRI_1                                  	0x25                                              	
#define FTD_SB_SN_DCOMP_1                                 	0x26                                              	
#define FTD_SB_SN_HCOMP_1                                 	0x27                                              	
#define FTD_SB_SN_NPDU_ABORT                              	0x28                                              	
#define FTD_SB_SN_NPDU_RECV                               	0x29                                              	
#define FTD_SB_SN_NPDU_SENT                               	0x2A                                              	
#define FTD_SB_SN_NPDU_RESENT                             	0x2B                                              	
#define FTD_SB_SN_RELC_2                                  	0x2C                                              	
#define FTD_SB_SN_PREC_2                                  	0x2D                                              	
#define FTD_SB_SN_DELC_2                                  	0x2E                                              	
#define FTD_SB_SN_PTPC_2                                  	0x2F                                              	
#define FTD_SB_SN_MTPC_2                                  	0x30                                              	
#define FTD_SB_SN_RPRI_2                                  	0x31                                              	
#define FTD_SB_SN_DCOMP_2                                 	0x32                                              	
#define FTD_SB_SN_HCOMP_2                                 	0x33                                              	
#define FTD_SB_LLC_FCS_FAIL                               	0x34                                              	
#define FTD_SB_LLC_KC1                                    	0x35                                              	
#define FTD_SB_LLC_KC2                                    	0x36                                              	
#define FTD_SB_LLC_OC_I_UL                                	0x37                                              	
#define FTD_SB_LLC_OC_I_DL                                	0x38                                              	
#define FTD_SB_LLC_OC_UI_UL                               	0x39                                              	
#define FTD_SB_LLC_OC_UI_DL                               	0x3A                                              	
#define FTD_SB_LLC_PDU_MISS                               	0x3B                                              	
#define FTD_SB_LLC_PDU_RECV                               	0x3C                                              	
#define FTD_SB_LLC_PDU_RESENT                             	0x3D                                              	
#define FTD_SB_LLC_PDU_SENT                               	0x3E                                              	
#define FTD_SB_LLC_N200_1                                 	0x3F                                              	
#define FTD_SB_LLC_T200_1                                 	0x40                                              	
#define FTD_SB_LLC_N201_U_1                               	0x41                                              	
#define FTD_SB_LLC_N201_I_1                               	0x42                                              	
#define FTD_SB_LLC_KD_1                                   	0x43                                              	
#define FTD_SB_LLC_KU_1                                   	0x44                                              	
#define FTD_SB_LLC_VS_1                                   	0x45                                              	
#define FTD_SB_LLC_VR_1                                   	0x46                                              	
#define FTD_SB_LLC_VA_1                                   	0x47                                              	
#define FTD_SB_LLC_N200_2                                 	0x48                                              	
#define FTD_SB_LLC_T200_2                                 	0x49                                              	
#define FTD_SB_LLC_N201_U_2                               	0x4A                                              	
#define FTD_SB_LLC_N201_I_2                               	0x4B                                              	
#define FTD_SB_LLC_KD_2                                   	0x4C                                              	
#define FTD_SB_LLC_KU_2                                   	0x4D                                              	
#define FTD_SB_LLC_VS_2                                   	0x4E                                              	
#define FTD_SB_LLC_VR_2                                   	0x4F                                              	
#define FTD_SB_LLC_VA_2                                   	0x50                                              	
#define FTD_SB_RLC_PAN_MAX                                	0x51                                              	
#define FTD_SB_RLC_PAN_DEC                                	0x52                                              	
#define FTD_SB_RLC_PAN_INC                                	0x53                                              	
#define FTD_SB_RLC_T3192                                  	0x54                                              	
#define FTD_SB_RLC_BS_CV_MAX                              	0x55                                              	
#define FTD_SB_RLC_ACK_SENT                               	0x56                                              	
#define FTD_SB_RLC_DL_MODE                                	0x57                                              	
#define FTD_SB_RLC_DL_CH_CODE                             	0x58                                              	
#define FTD_SB_RLC_DL_SLOTS                               	0x59                                              	
#define FTD_SB_RLC_BLOCKS_RECV                            	0x5A                                              	
#define FTD_SB_RLC_BLOCKS_MISS                            	0x5B                                              	
#define FTD_SB_RLC_TBF_REL_T3190                          	0x5C                                              	
#define FTD_SB_RLC_PDU_ABORT                              	0x5D                                              	
#define FTD_SB_RLC_UL_MODE                                	0x5E                                              	
#define FTD_SB_RLC_UL_CH_CODE                             	0x5F                                              	
#define FTD_SB_RLC_UL_SLOTS                               	0x60                                              	
#define FTD_SB_RLC_ENDING_MODE                            	0x61                                              	
#define FTD_SB_RLC_TBF_REL_NO                             	0x62                                              	
#define FTD_SB_RLC_ACK_RECV                               	0x63                                              	
#define FTD_SB_RLC_BLOCKS_SENT                            	0x64                                              	
#define FTD_SB_RLC_BLOCKS_RESENT                          	0x65                                              	
#define FTD_SB_RLC_N3102                                  	0x66                                              	
#define FTD_SB_RLC_WINDOW_STALL                           	0x67                                              	
#define FTD_SB_RLC_CONT_RES_FAIL                          	0x68                                              	
#define FTD_SB_RLC_TBF_REL_N3102                          	0x69                                              	
#define FTD_SB_RLC_TBF_REL_T3164                          	0x6A                                              	
#define FTD_SB_RLC_TBF_REL_T3180                          	0x6B                                              	
#define FTD_SB_RLC_TBF_REL_T3182                          	0x6C                                              	
#define FTD_SB_RLC_TBF_REL_T3184                          	0x6D                                              	
#define FTD_SB_GMM_ATTACH_STATUS                          	0x6E                                              	
#define FTD_SB_GMM_R_AND_S                                	0x6F                                              	
#define FTD_SB_GMM_NW_MODE                                	0x70                                              	
#define FTD_SB_GMM_ATTACH_TYPE                            	0x71                                              	
#define FTD_SB_GMM_READY_TMR_STATUS                       	0x72                                              	
#define FTD_SB_GMM_READY_TMR_TIMEOUT                      	0x73                                              	
#define FTD_SB_GMM_RAU_TMR_CURRENT                        	0x74                                              	
#define FTD_SB_GMM_RAU_TMR_TIMEOUT                        	0x75                                              	
#define FTD_SB_GMM_PTMSI_VALUE                            	0x76                                              	
#define FTD_SB_GMM_RAI_VALUE                              	0x77                                              	
#define FTD_SB_GMM_CIPHERING_VALUE                        	0x78                                              	
#define FTD_SB_GMM_SPLIT_PG_CYCLE_VALUE                   	0x79                                              	
#define FTD_SB_GMM_NON_DRX_TMR                            	0x7A                                              	
#define FTD_SB_GMM_SMS_PRIORITY                           	0x7B                                              	
#define FTD_SB_GMM_ATTACH_FAIL_CAUSE                      	0x7C                                              	
#define FTD_SB_GMM_ATTACH_ATTEMPT                         	0x7D                                              	
#define FTD_SB_GMM_ATTACH_OK                              	0x7E                                              	
#define FTD_SB_GMM_NTW_INIT_DET_CAUSE                     	0x7F                                              	
#define FTD_SB_GMM_NTW_INIT_DETACH                        	0x80                                              	
#define FTD_SB_MAC_DL_TFI                                 	0x81                                              	
#define FTD_SB_MAC_ALLOC_MODE                             	0x82                                              	
#define FTD_SB_MAC_UL_TBF_EST_CAUSE                       	0x83                                              	
#define FTD_SB_MAC_CH_REQ_TYPE                            	0x84                                              	
#define FTD_SB_MAC_RADIO_PRIORITY                         	0x85                                              	
#define FTD_SB_MAC_TBF_EST_RES                            	0x86                                              	
#define FTD_SB_MAC_TBF_EST_REALIZED                       	0x87                                              	
#define FTD_SB_MAC_CTRL_ACK_TYPE                          	0x88                                              	
#define FTD_SB_MAC_ACC_BURST_TYPE                         	0x89                                              	
#define FTD_SB_MAC_DRX_TIMER_MAX                          	0x8A                                              	
#define FTD_SB_MAC_SPLIT_PG_CYCLE_ON_CCCH                 	0x8B                                              	
#define FTD_SB_MAC_PAGE_MODE                              	0x8C                                              	
#define FTD_SB_MAC_NC_MODE_BROADCAST                      	0x8D                                              	
#define FTD_SB_MAC_T3168_VALUE                            	0x8E                                              	
#define FTD_SB_MAC_PCCCH_SUPPORTED                        	0x8F                                              	
#define FTD_SB_MAC_BS_PBCCH_BLKS                          	0x90                                              	
#define FTD_SB_MAC_BS_PCC_CHANS                           	0x91                                              	
#define FTD_SB_MAC_BS_PAG_BLKS_RES                        	0x92                                              	
#define FTD_SB_MAC_BS_PCC_REL                             	0x93                                              	
#define FTD_SB_MAC_BS_PRACH_BLKS                          	0x94                                              	
#define FTD_SB_MAC_MAX_PRA_RETRANS                        	0x95                                              	
#define FTD_SB_MAC_PSI5_BROADCAST                         	0x96                                              	
#define FTD_SB_MAC_PSI1_REPEAT_PERIOD                     	0x97                                              	
#define FTD_SB_MAC_PSI_COUNT_LR                           	0x98                                              	
#define FTD_SB_MAC_PSI_COUNT_HR                           	0x99                                              	
#define FTD_SB_MAC_PSI_STATUS_IND                         	0x9A                                              	
#define FTD_SB_MAC_QUAL_REP_SENT_IN                       	0x9B                                              	
#define FTD_SB_MAC_QUAL_REP_C                             	0x9C                                              	
#define FTD_SB_MAC_QUAL_REP_RXQUAL                        	0x9D                                              	
#define FTD_SB_MAC_QUAL_REP_SIGN_VAR                      	0x9E                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN0                   	0x9F                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN1                   	0xA0                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN2                   	0xA1                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN3                   	0xA2                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN4                   	0xA3                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN5                   	0xA4                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN6                   	0xA5                                              	
#define FTD_SB_MAC_QUAL_REP_I_LEVEL_TN7                   	0xA6                                              	
#define FTD_SB_MAC_NC_MODE                                	0xA7                                              	
#define FTD_SB_MAC_NC_NON_DRX_PERIOD                      	0xA8                                              	
#define FTD_SB_MAC_NC_REPORTING_PERIOD                    	0xA9                                              	
#define FTD_SB_MAC_NC_PARAMS_SOURCE                       	0xAA                                              	
#define FTD_SB_MAC_EXT_MEAS_ORDER                         	0xAB                                              	
#define FTD_SB_MAC_EXT_MEAS_REP_TYPE                      	0xAC                                              	
#define FTD_SB_MAC_EXT_REPORTING_PERIOD                   	0xAD                                              	
#define FTD_SB_MAC_EXT_PARAMS_SOURCE                      	0xAE                                              	
#define FTD_SB_MAC_EXT_NCC_PERMITTED                      	0xAF                                              	
#define FTD_SB_MAC_NC_REP_RXLEV_SERVCELL                  	0xB0                                              	
#define FTD_SB_MAC_NC_REP_INT_SERVCELL                    	0xB1                                              	
#define FTD_SB_MAC_NC_REP_1_CARRIER                       	0xB2                                              	
#define FTD_SB_MAC_NC_REP_1_BSIC                          	0xB3                                              	
#define FTD_SB_MAC_NC_REP_1_RXLEV                         	0xB4                                              	
#define FTD_SB_MAC_NC_REP_2_CARRIER                       	0xB5                                              	
#define FTD_SB_MAC_NC_REP_2_BSIC                          	0xB6                                              	
#define FTD_SB_MAC_NC_REP_2_RXLEV                         	0xB7                                              	
#define FTD_SB_MAC_NC_REP_3_CARRIER                       	0xB8                                              	
#define FTD_SB_MAC_NC_REP_3_BSIC                          	0xB9                                              	
#define FTD_SB_MAC_NC_REP_3_RXLEV                         	0xBA                                              	
#define FTD_SB_MAC_NC_REP_4_CARRIER                       	0xBB                                              	
#define FTD_SB_MAC_NC_REP_4_BSIC                          	0xBC                                              	
#define FTD_SB_MAC_NC_REP_4_RXLEV                         	0xBD                                              	
#define FTD_SB_MAC_NC_REP_5_CARRIER                       	0xBE                                              	
#define FTD_SB_MAC_NC_REP_5_BSIC                          	0xBF                                              	
#define FTD_SB_MAC_NC_REP_5_RXLEV                         	0xC0                                              	
#define FTD_SB_MAC_NC_REP_6_CARRIER                       	0xC1                                              	
#define FTD_SB_MAC_NC_REP_6_BSIC                          	0xC2                                              	
#define FTD_SB_MAC_NC_REP_6_RXLEV                         	0xC3                                              	
#define FTD_SB_MAC_EXT_REP_1_CARRIER                      	0xC4                                              	
#define FTD_SB_MAC_EXT_REP_1_BSIC                         	0xC5                                              	
#define FTD_SB_MAC_EXT_REP_1_RXLEV                        	0xC6                                              	
#define FTD_SB_MAC_EXT_REP_2_CARRIER                      	0xC7                                              	
#define FTD_SB_MAC_EXT_REP_2_BSIC                         	0xC8                                              	
#define FTD_SB_MAC_EXT_REP_2_RXLEV                        	0xC9                                              	
#define FTD_SB_MAC_EXT_REP_3_CARRIER                      	0xCA                                              	
#define FTD_SB_MAC_EXT_REP_3_BSIC                         	0xCB                                              	
#define FTD_SB_MAC_EXT_REP_3_RXLEV                        	0xCC                                              	
#define FTD_SB_MAC_EXT_REP_4_CARRIER                      	0xCD                                              	
#define FTD_SB_MAC_EXT_REP_4_BSIC                         	0xCE                                              	
#define FTD_SB_MAC_EXT_REP_4_RXLEV                        	0xCF                                              	
#define FTD_SB_MAC_EXT_REP_5_CARRIER                      	0xD0                                              	
#define FTD_SB_MAC_EXT_REP_5_BSIC                         	0xD1                                              	
#define FTD_SB_MAC_EXT_REP_5_RXLEV                        	0xD2                                              	
#define FTD_SB_MAC_EXT_REP_6_CARRIER                      	0xD3                                              	
#define FTD_SB_MAC_EXT_REP_6_BSIC                         	0xD4                                              	
#define FTD_SB_MAC_EXT_REP_6_RXLEV                        	0xD5                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN0                    	0xD6                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN1                    	0xD7                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN2                    	0xD8                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN3                    	0xD9                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN4                    	0xDA                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN5                    	0xDB                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN6                    	0xDC                                              	
#define FTD_SB_MAC_EXT_REP_I_LEVEL_TN7                    	0xDD                                              	
#define FTD_SB_MAC_DL_PWR_CTRL_BTS_MODE                   	0xDE                                              	
#define FTD_SB_MAC_DL_PWR_CTRL_P0                         	0xDF                                              	
#define FTD_SB_MAC_DL_PWR_CTRL_PR_MODE                    	0xE0                                              	
#define FTD_SB_MAC_CTR_UL_IDLE_CCCH_ATT                   	0xE1                                              	
#define FTD_SB_MAC_CTR_UL_IDLE_CCCH                       	0xE2                                              	
#define FTD_SB_MAC_CTR_UL_IDLE_PCCCH_ATT                  	0xE3                                              	
#define FTD_SB_MAC_CTR_UL_IDLE_PCCCH                      	0xE4                                              	
#define FTD_SB_MAC_CTR_UL_DURING_DL_ATT                   	0xE5                                              	
#define FTD_SB_MAC_CTR_UL_DURING_DL                       	0xE6                                              	
#define FTD_SB_MAC_CTR_DL_EST_CCCH                        	0xE7                                              	
#define FTD_SB_MAC_CTR_DL_EST_PCCCH                       	0xE8                                              	
#define FTD_SB_MAC_CTR_DL_EST_T3192_RUNNING               	0xE9                                              	
#define FTD_SB_MAC_CTR_DL_EST_TRANSFER                    	0xEA                                              	
#define FTD_SB_MAC_CTR_UL_REALLOC_ATT                     	0xEB                                              	
#define FTD_SB_MAC_CTR_UL_REALLOC                         	0xEC                                              	
#define FTD_SB_MAC_CTR_UL_REALLOC_NET                     	0xED                                              	
#define FTD_SB_MAC_CTR_DL_REALLOC                         	0xEE                                              	
#define FTD_SB_MAC_CTR_UL_FAIL_CCCH_OTHER                 	0xEF                                              	
#define FTD_SB_MAC_CTR_UL_FAIL_PCCCH_OTHER                	0xF0                                              	
#define FTD_SB_MAC_CTR_UL_FAIL_REJECT_CCCH                	0xF1                                              	
#define FTD_SB_MAC_CTR_UL_FAIL_REJECT_PCCCH               	0xF2                                              	
#define FTD_SB_MAC_CTR_UL_FAIL_TIMEOUT_CCCH               	0xF3                                              	
#define FTD_SB_MAC_CTR_UL_FAIL_TIMEOUT_PCCCH              	0xF4                                              	
#define FTD_SB_MAC_CTR_UL_WAIT_ON_SDCCH                   	0xF5                                              	
#define FTD_SB_MAC_CTR_UL_QUEUE_ON_PCCCH                  	0xF6                                              	
#define FTD_SB_MAC_CTR_ABN_UL_REL_TBF_RELEASE             	0xF7                                              	
#define FTD_SB_MAC_CTR_ABN_UL_REL_T3188_EXPIRY            	0xF8                                              	
#define FTD_SB_MAC_CTR_DL_ACKS_SENT                       	0xF9                                              	
#define FTD_SB_MAC_CTR_UL_ACKS_RECEIVED                   	0xFA                                              	
#define FTD_SB_MAC_CTR_OTHER_CTRL_BLOCKS                  	0xFB                                              	
#define FTD_SB_MAC_CTR_OTHER_CTRL_BLOCKS_SENT             	0xFC                                              	
#define FTD_SB_MAC_CTR_4ACC_BURSTS_PCAS_SENT              	0xFD                                              	
#define FTD_SB_MAC_CTR_SUSPEND_IDLE                       	0xFE                                              	
#define FTD_SB_MAC_CTR_SUSPEND_TRANSFER                   	0xFF                                              	
#define FTD_SB_MAC_CTR_RESUME                             	0x100                                             	
#define FTD_SB_MAC_CTR_PACKET_PAGING_PTMSI_PCCCH          	0x101                                             	
#define FTD_SB_MAC_CTR_PACKET_PAGING_IMSI_PCCCH           	0x102                                             	
#define FTD_SB_MAC_CTR_PACKET_PAGING_PTMSI_CCCH           	0x103                                             	
#define FTD_SB_MAC_CTR_PACKET_PAGING_IMSI_CCCH            	0x104                                             	
#define FTD_SB_MAC_CTR_PSI13_ON_PACCH                     	0x105                                             	
#define FTD_SB_MAC_CTR_PSI1_ON_PACCH                      	0x106                                             	
#define FTD_SB_MAC_CTR_MEAS_REP_SEND_CCCH_ATT             	0x107                                             	
#define FTD_SB_MAC_CTR_MEAS_REP_SEND_CCCH                 	0x108                                             	
#define FTD_SB_MAC_CTR_MEAS_REP_SEND_PCCCH_ATT            	0x109                                             	
#define FTD_SB_MAC_CTR_MEAS_REP_SEND_PCCCH                	0x10A                                             	
#define FTD_SB_MAC_CTR_MEAS_REP_SEND_TBF_ATT              	0x10B                                             	
#define FTD_SB_MAC_CTR_MEAS_REP_SEND_TBF                  	0x10C                                             	
#define FTD_SB_GPDS_CONTEXT_COUNT                         	0x10D                                             	
#define FTD_SB_GPDS_ACTIVE_CIDS                           	0x10E                                             	
#define FTD_SB_GPDS_RELATEDPRIM_CID                       	0x10F                                             	
#define FTD_SB_GMM_PREVENT_READY_EXPIRY                   	0x110                                             	
#define FTD_SB_GMM_PRAU_FAIL_CAUSE                        	0x111                                             	
#define FTD_SB_GMM_PRAU_ATTEMPT                           	0x112                                             	
#define FTD_SB_GMM_PRAU_OK                                	0x113                                             	
#define FTD_SB_GMM_RAU_FAIL_CAUSE                         	0x114                                             	
#define FTD_SB_RAU_ATTEMPT                                	0x115                                             	
#define FTD_SB_RAU_OK                                     	0x116                                             	
#define FTD_SB_SM_MO_PDP_FAIL_CAUSE                       	0x117                                             	
#define FTD_SB_SM_MO_PDP_ATTEMPT                          	0x118                                             	
#define FTD_SB_SM_MO_PDP_OK                               	0x119                                             	
#define FTD_SB_SM_MO_PDP_DEAC_CAUSE                       	0x11A                                             	
#define FTD_SB_SM_MT_PDP_DEAC_CAUSE                       	0x11B                                             	
#define FTD_SB_MAC_UL_TBF_IDLE_ATTEMPT                    	0x11C                                             	
#define FTD_SB_RLC_UL_TBF_IDLE_SUCC                       	0x11D                                             	
#define FTD_SB_MAC_UL_TBF_TRANS_ATTEMPT                   	0x11E                                             	
#define FTD_SB_RLC_UL_TBF_TRANS_SUCC                      	0x11F                                             	
#define FTD_SB_MAC_DL_TBF_COUNTER                         	0x120                                             	
#define FTD_SB_RLC_UL_TBF_ABNORM_REL                      	0x121                                             	
#define FTD_SB_MAC_RESEL_C_C                              	0x122                                             	
#define FTD_SB_MAC_RESEL_C_P                              	0x123                                             	
#define FTD_SB_MAC_RESEL_P_C                              	0x124                                             	
#define FTD_SB_MAC_RESEL_P_P                              	0x125                                             	
#define FTD_SB_MAC_PCCO_C_C_ATT                           	0x126                                             	
#define FTD_SB_MAC_PCCO_C_C_OK                            	0x127                                             	
#define FTD_SB_MAC_PCCO_C_P_ATT                           	0x128                                             	
#define FTD_SB_MAC_PCCO_C_P_OK                            	0x129                                             	
#define FTD_SB_MAC_PCCO_P_C_ATT                           	0x12A                                             	
#define FTD_SB_MAC_PCCO_P_C_OK                            	0x12B                                             	
#define FTD_SB_MAC_PCCO_P_P_ATT                           	0x12C                                             	
#define FTD_SB_MAC_PCCO_P_P_OK                            	0x12D                                             	
#define FTD_SB_GMM_FOLLOW_ON_REQUEST                      	0x130                                             	
#define FTD_SB_ERLC_MCS_DOWNLINK                          	0x155                                             	
#define FTD_SB_ERLC_MS_OUT_OF_MEM                         	0x156                                             	
#define FTD_SB_ERLC_WINDOW_DOWNLINK                       	0x157                                             	
#define FTD_SB_ERLC_MCS_UPLINK                            	0x158                                             	
#define FTD_SB_ERLC_RESEGMENT                             	0x159                                             	
#define FTD_SB_ERLC_LINK_ADAPTATIONS                      	0x15A                                             	
#define FTD_SB_ERLC_WINDOW_UPLINK                         	0x15B                                             	
#define FTD_SB_ERLC_PRE_EMPTIVE                           	0x15C                                             	
#define FTD_SB_EGPRS_LQ_MEAS_MODE                         	0x15D                                             	
#define FTD_SB_EGPRS_BEP_MEAN_GPSK                        	0x15E                                             	
#define FTD_SB_EGPRS_BEP_CV_GPSK                          	0x15F                                             	
#define FTD_SB_EGPRS_BEP_MEAN_8PSK                        	0x160                                             	
#define FTD_SB_EGPRS_BEP_CV_8PSK                          	0x161                                             	
#define FTD_SB_DTM_GTTP_UL_CTR                            	0x170                                             	
#define FTD_SB_DTM_GTTP_DL_CTR                            	0x171                                             	
#define FTD_SB_GPDS_MSL_SMS_SERV_REQ_COUNTER              	0x1A0                                             	
#define FTD_SB_GPDS_MSL_DATA_SERV_REQ_COUNTER             	0x1A1                                             	
#define FTD_SB_GPDS_MSL_GSM_SERV_REQ_COUNTER              	0x1A2                                             	
#define FTD_SB_GPDS_MSL_INTSYS_HO_COUNTER                 	0x1A3                                             	
#define FTD_SB_GPDS_CHANGE_CONTEXT                        	0x200                                             	
#define FTD_SB_GPDS_CID                                   	0x201                                             	
#define FTD_SB_GPDS_NSAPI                                 	0x202                                             	
#define FTD_SB_GPDS_SAPI                                  	0x203                                             	
#define FTD_SB_GPDS_PDP_ADDRESS_V4_1                      	0x204                                             	
#define FTD_SB_GPDS_PDP_ADDRESS_V4_2                      	0x205                                             	
#define FTD_SB_GPDS_PDP_ADDRESS_V4_3                      	0x206                                             	
#define FTD_SB_GPDS_PDP_ADDRESS_V4_4                      	0x207                                             	
#define FTD_SB_GPDS_SM_RELC                               	0x208                                             	
#define FTD_SB_GPDS_SM_DELC                               	0x209                                             	
#define FTD_SB_GPDS_SM_PREC                               	0x20A                                             	
#define FTD_SB_GPDS_SM_PTPC                               	0x20B                                             	
#define FTD_SB_GPDS_SM_MTPC                               	0x20C                                             	
#define FTD_SB_GPDS_SM_TRAC                               	0x250                                             	
#define FTD_SB_GPDS_SM_DOES                               	0x251                                             	
#define FTD_SB_GPDS_SM_DELO                               	0x252                                             	
#define FTD_SB_GPDS_SM_MSDU                               	0x253                                             	
#define FTD_SB_GPDS_SM_MBRU                               	0x254                                             	
#define FTD_SB_GPDS_SM_MBRD                               	0x255                                             	
#define FTD_SB_GPDS_SM_RBER                               	0x256                                             	
#define FTD_SB_GPDS_SM_SDER                               	0x257                                             	
#define FTD_SB_GPDS_SM_TRHP                               	0x258                                             	
#define FTD_SB_GPDS_SM_TRDL                               	0x259                                             	
#define FTD_SB_GPDS_SM_GBRU                               	0x25A                                             	
#define FTD_SB_GPDS_SM_GBRD                               	0x25B                                             	
#define FTD_SB_GPDS_MSL_RBID                              	0x2A0                                             	
#define FTD_SB_GPDS_MSL_RPRI                              	0x2A1                                             	
#define FTD_SB_GPDS_MSL_PFID                              	0x2A2                                             	
#define FTD_SB_GPDS_MSL_HCOMP                             	0x2A3                                             	
#define FTD_SB_GPDS_MSL_DCOMP                             	0x2A4                                             	
#define FTD_SB_GPDS_PDCP_RBID                             	0x2A5                                             	
#define FTD_SB_GPDS_PDCP_HEADER                           	0x2A6                                             	
#define FTD_SB_GPDS_PDCP_RELOC                            	0x2A7                                             	
#define FTD_SB_GPDS_PDCP_MAX_WS                           	0x2A8                                             	
#define FTD_SB_GPDS_PDCP_RECV_DISC                        	0x2A9                                             	
#define FTD_SB_GPDS_PDCP_SENT_DISC                        	0x2AA                                             	
#define FTD_SB_GPDS_PDCP_SEQ_RECV                         	0x2AB                                             	
#define FTD_SB_GPDS_PDCP_SEQ_SENT                         	0x2AC                                             	
#define FTD_SB_GPDS_PDCP_PDU_RECV                         	0x2AD                                             	
#define FTD_SB_GPDS_PDCP_PDU_SENT                         	0x2AE                                             	
#define FTD_SB_GPDS_RFC2507_MAX_PERIOD                    	0x2AF                                             	
#define FTD_SB_GPDS_RFC2507_MAX_TIME                      	0x2B0                                             	
#define FTD_SB_GPDS_RFC2507_MAX_HEADER                    	0x2B1                                             	
#define FTD_SB_GPDS_RFC2507_EXP_REORD                     	0x2B2                                             	
#define FTD_SB_GPDS_RFC2507_TCP_SPACE                     	0x2B3                                             	
#define FTD_SB_GPDS_RFC2507_NON_TCP_SPACE                 	0x2B4                                             	
#define FTD_SB_GPDS_RFC2507_CTX_STATE_RECV                	0x2B5                                             	
#define FTD_SB_GPDS_RFC2507_CTX_STATE_SENT                	0x2B6                                             	
#define FTD_SB_URR_STATE                                  	0x300                                             	
#define FTD_SB_URR_SCAN_FREQUENCY                         	0x301                                             	
#define FTD_SB_URR_CAUSE                                  	0x302                                             	
#define FTD_SB_URR_REGISTER_REATT                         	0x303                                             	
#define FTD_SB_URR_ROVE_IN_CTR                            	0x304                                             	
#define FTD_SB_URR_ROVE_OUT_CTR                           	0x305                                             	
#define FTD_SB_URR_HO_TO_UMAN_CTR                         	0x306                                             	
#define FTD_SB_URR_HO_TO_GERAN_CTR                        	0x307                                             	
#define FTD_SB_URR_ARFCN                                  	0x308                                             	
#define FTD_SB_URR_BSIC                                   	0x309                                             	
#define FTD_SB_URR_LAC_BLACKLIST                          	0x30A                                             	
#define FTD_SB_URR_ADDR_TYPE                              	0x30B                                             	
#define FTD_SB_URR_UNC_ADDRESS_1                          	0x30C                                             	
#define FTD_SB_URR_UNC_ADDRESS_2                          	0x30D                                             	
#define FTD_SB_URR_UNC_ADDRESS_3                          	0x30E                                             	
#define FTD_SB_URR_UNC_ADDRESS_4                          	0x30F                                             	
#define FTD_SB_URR_SGW_ADDRESS_1                          	0x310                                             	
#define FTD_SB_URR_SGW_ADDRESS_2                          	0x311                                             	
#define FTD_SB_URR_SGW_ADDRESS_3                          	0x312                                             	
#define FTD_SB_URR_SGW_ADDRESS_4                          	0x313                                             	
#define FTD_SB_URLC_CAUSE                                 	0x330                                             	


//SUBBLOCKS


//Definition for GPDS_COMP_INFO
#define GPDS_COMP_INFO                                    	0x00                                                 	
#define GPDS_COMP_INFO_OFFSET_ID                          	0 //size 1 byte(s)
#define GPDS_COMP_INFO_OFFSET_LENGTH                      	1 //size 1 byte(s)
#define GPDS_COMP_INFO_OFFSET_DATACOMPRESSION             	2 //size 1 byte(s)
#define GPDS_COMP_INFO_OFFSET_HEADERCOMPRESSION           	3 //size 1 byte(s)
#define SIZE_GPDS_COMP_INFO                               	4


//Definition for GPDS_QOS_REQ_INFO
#define GPDS_QOS_REQ_INFO                                 	0x01                                                 	
#define GPDS_QOS_REQ_INFO_OFFSET_ID                       	0 //size 1 byte(s)
#define GPDS_QOS_REQ_INFO_OFFSET_LENGTH                   	1 //size 1 byte(s)
#define GPDS_QOS_REQ_INFO_OFFSET_PRECEDENCE               	2 //size 1 byte(s)
#define GPDS_QOS_REQ_INFO_OFFSET_DELAY                    	3 //size 1 byte(s)
#define GPDS_QOS_REQ_INFO_OFFSET_RELIABILITY              	4 //size 1 byte(s)
#define GPDS_QOS_REQ_INFO_OFFSET_PEAKTHROUGHPUT           	5 //size 1 byte(s)
#define GPDS_QOS_REQ_INFO_OFFSET_MEANTHROUGHPUT           	6 //size 1 byte(s)
#define GPDS_QOS_REQ_INFO_OFFSET_FILLERBYTE1              	7 //size 1 byte(s)
#define SIZE_GPDS_QOS_REQ_INFO                            	8


//Definition for GPDS_QOS_MIN_INFO
#define GPDS_QOS_MIN_INFO                                 	0x02                                                 	
#define GPDS_QOS_MIN_INFO_OFFSET_ID                       	0 //size 1 byte(s)
#define GPDS_QOS_MIN_INFO_OFFSET_LENGTH                   	1 //size 1 byte(s)
#define GPDS_QOS_MIN_INFO_OFFSET_PRECEDENCE               	2 //size 1 byte(s)
#define GPDS_QOS_MIN_INFO_OFFSET_DELAY                    	3 //size 1 byte(s)
#define GPDS_QOS_MIN_INFO_OFFSET_RELIABILITY              	4 //size 1 byte(s)
#define GPDS_QOS_MIN_INFO_OFFSET_PEAKTHROUGHPUT           	5 //size 1 byte(s)
#define GPDS_QOS_MIN_INFO_OFFSET_MEANTHROUGHPUT           	6 //size 1 byte(s)
#define GPDS_QOS_MIN_INFO_OFFSET_FILLERBYTE1              	7 //size 1 byte(s)
#define SIZE_GPDS_QOS_MIN_INFO                            	8


//Definition for GPDS_QOS_NEG_INFO
#define GPDS_QOS_NEG_INFO                                 	0x03                                                 	
#define GPDS_QOS_NEG_INFO_OFFSET_ID                       	0 //size 1 byte(s)
#define GPDS_QOS_NEG_INFO_OFFSET_LENGTH                   	1 //size 1 byte(s)
#define GPDS_QOS_NEG_INFO_OFFSET_PRECEDENCE               	2 //size 1 byte(s)
#define GPDS_QOS_NEG_INFO_OFFSET_DELAY                    	3 //size 1 byte(s)
#define GPDS_QOS_NEG_INFO_OFFSET_RELIABILITY              	4 //size 1 byte(s)
#define GPDS_QOS_NEG_INFO_OFFSET_PEAKTHROUGHPUT           	5 //size 1 byte(s)
#define GPDS_QOS_NEG_INFO_OFFSET_MEANTHROUGHPUT           	6 //size 1 byte(s)
#define GPDS_QOS_NEG_INFO_OFFSET_FILLERBYTE1              	7 //size 1 byte(s)
#define SIZE_GPDS_QOS_NEG_INFO                            	8


//Definition for GPDS_PDP_ADDRESS_INFO
#define GPDS_PDP_ADDRESS_INFO                             	0x04                                                 	
#define GPDS_PDP_ADDRESS_INFO_OFFSET_ID                   	0 //size 1 byte(s)
#define GPDS_PDP_ADDRESS_INFO_OFFSET_LENGTH               	1 //size 1 byte(s)
#define GPDS_PDP_ADDRESS_INFO_OFFSET_FILLERBYTE1          	2 //size 1 byte(s)
#define GPDS_PDP_ADDRESS_INFO_OFFSET_ADDRLENGTH           	3 //size 1 byte(s)
#define GPDS_PDP_ADDRESS_INFO_OFFSET_ADDRESS              	4 //size 1 byte(s)
#define GPDS_PDP_ADDRESS_INFO_OFFSET_FILLERBYTE2          	5 //size 1 byte(s)
#define GPDS_PDP_ADDRESS_INFO_OFFSET_FILLERBYTE3          	6 //size 1 byte(s)
#define GPDS_PDP_ADDRESS_INFO_OFFSET_FILLERBYTE4          	7 //size 1 byte(s)
#define SIZE_GPDS_PDP_ADDRESS_INFO                        	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_APN_INFO
#define GPDS_APN_INFO                                     	0x05                                                 	
#define GPDS_APN_INFO_OFFSET_ID                           	0 //size 1 byte(s)
#define GPDS_APN_INFO_OFFSET_LENGTH                       	1 //size 1 byte(s)
#define GPDS_APN_INFO_OFFSET_ADDRLENGTH                   	2 //size 1 byte(s)
#define GPDS_APN_INFO_OFFSET_APNNAME                      	3 //size 1 byte(s)
#define SIZE_GPDS_APN_INFO                                	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GPDS_FILT_SRC_IPV4_ADDR_INFO
#define GPDS_FILT_SRC_IPV4_ADDR_INFO                      	0x10                                              	
#define GPDS_FILT_SRC_IPV4_ADDR_INFO_OFFSET_ID            	0 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV4_ADDR_INFO_OFFSET_LENGTH        	1 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV4_ADDR_INFO_OFFSET_IPV4ADDRMASK  	2 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV4_ADDR_INFO_OFFSET_FILLERBYTE1   	3 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV4_ADDR_INFO_OFFSET_IPV4ADDRESS   	4 //size 4 byte(s)
#define SIZE_GPDS_FILT_SRC_IPV4_ADDR_INFO                 	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_FILT_SRC_IPV6_ADDR_INFO
#define GPDS_FILT_SRC_IPV6_ADDR_INFO                      	0x20                                              	
#define GPDS_FILT_SRC_IPV6_ADDR_INFO_OFFSET_ID            	0 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV6_ADDR_INFO_OFFSET_LENGTH        	1 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV6_ADDR_INFO_OFFSET_IPV6ADDRMASK  	2 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV6_ADDR_INFO_OFFSET_FILLERBYTE1   	3 //size 1 byte(s)
#define GPDS_FILT_SRC_IPV6_ADDR_INFO_OFFSET_IPV6ADDRESS   	4 //size 16 byte(s)
#define SIZE_GPDS_FILT_SRC_IPV6_ADDR_INFO                 	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_FILT_PROTOCOL_INFO
#define GPDS_FILT_PROTOCOL_INFO                           	0x30                                              	
#define GPDS_FILT_PROTOCOL_INFO_OFFSET_ID                 	0 //size 1 byte(s)
#define GPDS_FILT_PROTOCOL_INFO_OFFSET_LENGTH             	1 //size 1 byte(s)
#define GPDS_FILT_PROTOCOL_INFO_OFFSET_PROTOCOLID         	2 //size 1 byte(s)
#define GPDS_FILT_PROTOCOL_INFO_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_GPDS_FILT_PROTOCOL_INFO                      	4


//Definition for GPDS_FILT_DST_PORT_INFO
#define GPDS_FILT_DST_PORT_INFO                           	0x40                                              	
#define GPDS_FILT_DST_PORT_INFO_OFFSET_ID                 	0 //size 1 byte(s)
#define GPDS_FILT_DST_PORT_INFO_OFFSET_LENGTH             	1 //size 1 byte(s)
#define GPDS_FILT_DST_PORT_INFO_OFFSET_DESTINATIONPORT    	2 //size 2 byte(s)
#define SIZE_GPDS_FILT_DST_PORT_INFO                      	4


//Definition for GPDS_FILT_DST_PORT_RANGE_INFO
#define GPDS_FILT_DST_PORT_RANGE_INFO                     	0x41                                              	
#define GPDS_FILT_DST_PORT_RANGE_INFO_OFFSET_ID           	0 //size 1 byte(s)
#define GPDS_FILT_DST_PORT_RANGE_INFO_OFFSET_LENGTH       	1 //size 1 byte(s)
#define GPDS_FILT_DST_PORT_RANGE_INFO_OFFSET_LOWLIMITDSTPORTRANGE	2 //size 2 byte(s)
#define GPDS_FILT_DST_PORT_RANGE_INFO_OFFSET_HIGHLIMITDSTPORTRANGE	4 //size 2 byte(s)
#define GPDS_FILT_DST_PORT_RANGE_INFO_OFFSET_FILLERBYTE1  	6 //size 1 byte(s)
#define GPDS_FILT_DST_PORT_RANGE_INFO_OFFSET_FILLERBYTE2  	7 //size 1 byte(s)
#define SIZE_GPDS_FILT_DST_PORT_RANGE_INFO                	8


//Definition for GPDS_FILT_SRC_PORT_INFO
#define GPDS_FILT_SRC_PORT_INFO                           	0x50                                              	
#define GPDS_FILT_SRC_PORT_INFO_OFFSET_ID                 	0 //size 1 byte(s)
#define GPDS_FILT_SRC_PORT_INFO_OFFSET_LENGTH             	1 //size 1 byte(s)
#define GPDS_FILT_SRC_PORT_INFO_OFFSET_SOURCEPORT         	2 //size 2 byte(s)
#define SIZE_GPDS_FILT_SRC_PORT_INFO                      	4


//Definition for GPDS_FILT_SRC_PORT_RANGE_INFO
#define GPDS_FILT_SRC_PORT_RANGE_INFO                     	0x51                                              	
#define GPDS_FILT_SRC_PORT_RANGE_INFO_OFFSET_ID           	0 //size 1 byte(s)
#define GPDS_FILT_SRC_PORT_RANGE_INFO_OFFSET_LENGTH       	1 //size 1 byte(s)
#define GPDS_FILT_SRC_PORT_RANGE_INFO_OFFSET_LOWLIMITSRCPORTRANGE	2 //size 2 byte(s)
#define GPDS_FILT_SRC_PORT_RANGE_INFO_OFFSET_HIGHLIMITSRCPORTRANGE	4 //size 2 byte(s)
#define GPDS_FILT_SRC_PORT_RANGE_INFO_OFFSET_FILLERBYTE1  	6 //size 1 byte(s)
#define GPDS_FILT_SRC_PORT_RANGE_INFO_OFFSET_FILLERBYTE2  	7 //size 1 byte(s)
#define SIZE_GPDS_FILT_SRC_PORT_RANGE_INFO                	8


//Definition for GPDS_FILT_SPI_INFO
#define GPDS_FILT_SPI_INFO                                	0x60                                              	
#define GPDS_FILT_SPI_INFO_OFFSET_ID                      	0 //size 1 byte(s)
#define GPDS_FILT_SPI_INFO_OFFSET_LENGTH                  	1 //size 1 byte(s)
#define GPDS_FILT_SPI_INFO_OFFSET_FILLERBYTE1             	2 //size 1 byte(s)
#define GPDS_FILT_SPI_INFO_OFFSET_FILLERBYTE2             	3 //size 1 byte(s)
#define GPDS_FILT_SPI_INFO_OFFSET_SECURITYPARAMIND        	4 //size 4 byte(s)
#define SIZE_GPDS_FILT_SPI_INFO                           	8


//Definition for GPDS_FILT_TOS_INFO
#define GPDS_FILT_TOS_INFO                                	0x70                                              	
#define GPDS_FILT_TOS_INFO_OFFSET_ID                      	0 //size 1 byte(s)
#define GPDS_FILT_TOS_INFO_OFFSET_LENGTH                  	1 //size 1 byte(s)
#define GPDS_FILT_TOS_INFO_OFFSET_TOS                     	2 //size 1 byte(s)
#define GPDS_FILT_TOS_INFO_OFFSET_TOSMASK                 	3 //size 1 byte(s)
#define SIZE_GPDS_FILT_TOS_INFO                           	4


//Definition for GPDS_FILT_FLOW_LABEL_INFO
#define GPDS_FILT_FLOW_LABEL_INFO                         	0x80                                              	
#define GPDS_FILT_FLOW_LABEL_INFO_OFFSET_ID               	0 //size 1 byte(s)
#define GPDS_FILT_FLOW_LABEL_INFO_OFFSET_LENGTH           	1 //size 1 byte(s)
#define GPDS_FILT_FLOW_LABEL_INFO_OFFSET_FILLERBYTE1      	2 //size 1 byte(s)
#define GPDS_FILT_FLOW_LABEL_INFO_OFFSET_FILLERBYTE2      	3 //size 1 byte(s)
#define GPDS_FILT_FLOW_LABEL_INFO_OFFSET_FLOWLABEL        	4 //size 4 byte(s)
#define SIZE_GPDS_FILT_FLOW_LABEL_INFO                    	8


//Definition for GPDS_TFT_FILTER_INFO
#define GPDS_TFT_FILTER_INFO                              	0x0A                                                 	
#define GPDS_TFT_FILTER_INFO_OFFSET_ID                    	0 //size 1 byte(s)
#define GPDS_TFT_FILTER_INFO_OFFSET_LENGTH                	1 //size 1 byte(s)
#define GPDS_TFT_FILTER_INFO_OFFSET_PACKETFILTERID        	2 //size 1 byte(s)
#define GPDS_TFT_FILTER_INFO_OFFSET_FILTEREVALPREC        	3 //size 1 byte(s)
#define GPDS_TFT_FILTER_INFO_OFFSET_FILLERBYTE1           	4 //size 1 byte(s)
#define GPDS_TFT_FILTER_INFO_OFFSET_FILLERBYTE2           	5 //size 1 byte(s)
#define GPDS_TFT_FILTER_INFO_OFFSET_FILLERBYTE3           	6 //size 1 byte(s)
#define GPDS_TFT_FILTER_INFO_OFFSET_NUMBEROFSUBBLOCKS     	7 //size 1 byte(s)
#define SIZE_GPDS_TFT_FILTER_INFO                         	8


//Definition for GPDS_TFT_INFO
#define GPDS_TFT_INFO                                     	0x09                                                 	
#define GPDS_TFT_INFO_OFFSET_ID                           	0 //size 1 byte(s)
#define GPDS_TFT_INFO_OFFSET_LENGTH                       	1 //size 1 byte(s)
#define GPDS_TFT_INFO_OFFSET_TFTOPERATION                 	2 //size 1 byte(s)
#define GPDS_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS            	3 //size 1 byte(s)
#define SIZE_GPDS_TFT_INFO                                	4


//Definition for GPDS_QOS99_REQ_INFO
#define GPDS_QOS99_REQ_INFO                               	0x06                                                 	
#define GPDS_QOS99_REQ_INFO_OFFSET_ID                     	0 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_LENGTH                 	1 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_TRAFFICCLASS           	2 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_DELIVERYORDER          	3 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_DELIVERYOFERRONEUSSDU  	4 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_RESIDUALBER            	5 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_ERRORRATIO             	6 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_PRIORITY               	7 //size 1 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_TRANSFERDELAY          	8 //size 2 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_MAXSDUSIZE             	10 //size 2 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_MAXBITRATEUPLINK       	12 //size 2 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_MAXBITRATEDOWNLINK     	14 //size 2 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_GUABITRATEUPLINK       	16 //size 2 byte(s)
#define GPDS_QOS99_REQ_INFO_OFFSET_GUABITRATEDOWNLINK     	18 //size 2 byte(s)
#define SIZE_GPDS_QOS99_REQ_INFO                          	20


//Definition for GPDS_QOS99_MIN_INFO
#define GPDS_QOS99_MIN_INFO                               	0x07                                                 	
#define GPDS_QOS99_MIN_INFO_OFFSET_ID                     	0 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_LENGTH                 	1 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_TRAFFICCLASS           	2 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_DELIVERYORDER          	3 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_DELIVERYOFERRONEUSSDU  	4 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_RESIDUALBER            	5 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_ERRORRATIO             	6 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_PRIORITY               	7 //size 1 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_TRANSFERDELAY          	8 //size 2 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_MAXSDUSIZE             	10 //size 2 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_MAXBITRATEUPLINK       	12 //size 2 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_MAXBITRATEDOWNLINK     	14 //size 2 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_GUABITRATEUPLINK       	16 //size 2 byte(s)
#define GPDS_QOS99_MIN_INFO_OFFSET_GUABITRATEDOWNLINK     	18 //size 2 byte(s)
#define SIZE_GPDS_QOS99_MIN_INFO                          	20


//Definition for GPDS_QOS99_NEG_INFO
#define GPDS_QOS99_NEG_INFO                               	0x08                                                 	
#define GPDS_QOS99_NEG_INFO_OFFSET_ID                     	0 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_LENGTH                 	1 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_TRAFFICCLASS           	2 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_DELIVERYORDER          	3 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_DELIVERYOFERRONEUSSDU  	4 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_RESIDUALBER            	5 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_ERRORRATIO             	6 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_PRIORITY               	7 //size 1 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_TRANSFERDELAY          	8 //size 2 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_MAXSDUSIZE             	10 //size 2 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_MAXBITRATEUPLINK       	12 //size 2 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_MAXBITRATEDOWNLINK     	14 //size 2 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_GUABITRATEUPLINK       	16 //size 2 byte(s)
#define GPDS_QOS99_NEG_INFO_OFFSET_GUABITRATEDOWNLINK     	18 //size 2 byte(s)
#define SIZE_GPDS_QOS99_NEG_INFO                          	20


//Definition for GPDS_PDNS_ADDRESS_INFO
#define GPDS_PDNS_ADDRESS_INFO                            	0x0D                                                 	
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_ID                  	0 //size 1 byte(s)
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_LENGTH              	1 //size 1 byte(s)
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_ADDRLENGTH          	3 //size 1 byte(s)
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_ADDRESS             	4 //size 1 byte(s)
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_FILLERBYTE2         	5 //size 1 byte(s)
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_FILLERBYTE3         	6 //size 1 byte(s)
#define GPDS_PDNS_ADDRESS_INFO_OFFSET_FILLERBYTE4         	7 //size 1 byte(s)
#define SIZE_GPDS_PDNS_ADDRESS_INFO                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_SDNS_ADDRESS_INFO
#define GPDS_SDNS_ADDRESS_INFO                            	0x0E                                                 	
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_ID                  	0 //size 1 byte(s)
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_LENGTH              	1 //size 1 byte(s)
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_ADDRLENGTH          	3 //size 1 byte(s)
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_ADDRESS             	4 //size 1 byte(s)
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_FILLERBYTE2         	5 //size 1 byte(s)
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_FILLERBYTE3         	6 //size 1 byte(s)
#define GPDS_SDNS_ADDRESS_INFO_OFFSET_FILLERBYTE4         	7 //size 1 byte(s)
#define SIZE_GPDS_SDNS_ADDRESS_INFO                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_USERNAME_INFO
#define GPDS_USERNAME_INFO                                	0x0B                                                 	
#define GPDS_USERNAME_INFO_OFFSET_ID                      	0 //size 1 byte(s)
#define GPDS_USERNAME_INFO_OFFSET_LENGTH                  	1 //size 1 byte(s)
#define GPDS_USERNAME_INFO_OFFSET_USERNAMELENGTH          	2 //size 1 byte(s)
#define GPDS_USERNAME_INFO_OFFSET_USERNAME                	3 //size 1 byte(s)
#define SIZE_GPDS_USERNAME_INFO                           	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GPDS_PASSWORD_INFO
#define GPDS_PASSWORD_INFO                                	0x0C                                                 	
#define GPDS_PASSWORD_INFO_OFFSET_ID                      	0 //size 1 byte(s)
#define GPDS_PASSWORD_INFO_OFFSET_LENGTH                  	1 //size 1 byte(s)
#define GPDS_PASSWORD_INFO_OFFSET_PASSWORDLENGTH          	2 //size 1 byte(s)
#define GPDS_PASSWORD_INFO_OFFSET_PASSWORD                	3 //size 1 byte(s)
#define SIZE_GPDS_PASSWORD_INFO                           	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GPDS_DNS_ADDRESS_REQ_INFO
#define GPDS_DNS_ADDRESS_REQ_INFO                         	0x90                                              	
#define GPDS_DNS_ADDRESS_REQ_INFO_OFFSET_ID               	0 //size 1 byte(s)
#define GPDS_DNS_ADDRESS_REQ_INFO_OFFSET_LENGTH           	1 //size 1 byte(s)
#define GPDS_DNS_ADDRESS_REQ_INFO_OFFSET_FILLERBYTE1      	2 //size 1 byte(s)
#define GPDS_DNS_ADDRESS_REQ_INFO_OFFSET_FILLERBYTE2      	3 //size 1 byte(s)
#define SIZE_GPDS_DNS_ADDRESS_REQ_INFO                    	4


//Definition for GPDS_CHALLENGE_INFO
#define GPDS_CHALLENGE_INFO                               	0x0F                                                 	
#define GPDS_CHALLENGE_INFO_OFFSET_ID                     	0 //size 1 byte(s)
#define GPDS_CHALLENGE_INFO_OFFSET_LENGTH                 	1 //size 1 byte(s)
#define GPDS_CHALLENGE_INFO_OFFSET_CHALLENGELENGTH        	2 //size 1 byte(s)
#define GPDS_CHALLENGE_INFO_OFFSET_CHALLENGE              	3 //size 1 byte(s)
#define SIZE_GPDS_CHALLENGE_INFO                          	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GPDS_CONDITIONAL_DETACH_INFO
#define GPDS_CONDITIONAL_DETACH_INFO                      	0xA0                                              	
#define GPDS_CONDITIONAL_DETACH_INFO_OFFSET_ID            	0 //size 1 byte(s)
#define GPDS_CONDITIONAL_DETACH_INFO_OFFSET_LENGTH        	1 //size 1 byte(s)
#define GPDS_CONDITIONAL_DETACH_INFO_OFFSET_FILLERBYTE1   	2 //size 1 byte(s)
#define GPDS_CONDITIONAL_DETACH_INFO_OFFSET_FILLERBYTE2   	3 //size 1 byte(s)
#define SIZE_GPDS_CONDITIONAL_DETACH_INFO                 	4


//Definition for GPDS_RESPONSE_INFO
#define GPDS_RESPONSE_INFO                                	0x11                                              	
#define GPDS_RESPONSE_INFO_OFFSET_ID                      	0 //size 1 byte(s)
#define GPDS_RESPONSE_INFO_OFFSET_LENGTH                  	1 //size 1 byte(s)
#define GPDS_RESPONSE_INFO_OFFSET_IDENTIFIER              	2 //size 1 byte(s)
#define GPDS_RESPONSE_INFO_OFFSET_RESPONSELENGTH          	3 //size 1 byte(s)
#define GPDS_RESPONSE_INFO_OFFSET_RESPONSE                	4 //size 1 byte(s)
#define GPDS_RESPONSE_INFO_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define GPDS_RESPONSE_INFO_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define GPDS_RESPONSE_INFO_OFFSET_FILLERBYTE3             	7 //size 1 byte(s)
#define SIZE_GPDS_RESPONSE_INFO                           	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GPDS_ACTIVATE_PDP_CONTEXT_REQUEST
#define GPDS_ACTIVATE_PDP_CONTEXT_REQUEST                 	0xA4                                              	
#define GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_SBID     	0 //size 1 byte(s)
#define GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_FILLERBYTE1	1 //size 1 byte(s)
#define GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_SBLEN    	2 //size 2 byte(s)
#define GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_DATALENGTH	4 //size 2 byte(s)
#define GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_DATA     	6 //size 1 byte(s)
#define GPDS_ACTIVATE_PDP_CONTEXT_REQUEST_OFFSET_FILLERBYTE2	7 //size 1 byte(s)
#define SIZE_GPDS_ACTIVATE_PDP_CONTEXT_REQUEST            	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_RESOURCE_CONF
#define GPDS_RESOURCE_CONF                                	0xA5                                              	
#define GPDS_RESOURCE_CONF_OFFSET_ID                      	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_OFFSET_LENGTH                  	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_OFFSET_GPDSRESOURCEID          	2 //size 2 byte(s)
#define GPDS_RESOURCE_CONF_OFFSET_RESCONFIGURATION        	4 //size 2 byte(s)
#define GPDS_RESOURCE_CONF_OFFSET_FILLERBYTE1             	6 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_OFFSET_FILLERBYTE2             	7 //size 1 byte(s)
#define SIZE_GPDS_RESOURCE_CONF                           	8


//Definition for GPDS_RESOURCE
#define GPDS_RESOURCE                                     	0xA3                                              	
#define GPDS_RESOURCE_OFFSET_ID                           	0 //size 1 byte(s)
#define GPDS_RESOURCE_OFFSET_LENGTH                       	1 //size 1 byte(s)
#define GPDS_RESOURCE_OFFSET_RES                          	2 //size 2 byte(s)
#define SIZE_GPDS_RESOURCE                                	4


//Definition for GPDS_RESOURCE_CONF_REQUIRED
#define GPDS_RESOURCE_CONF_REQUIRED                       	0xA6                                              	
#define GPDS_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_REQUIRED_OFFSET_RESPP          	2 //size 2 byte(s)
#define SIZE_GPDS_RESOURCE_CONF_REQUIRED                  	4


//Definition for GPDS_DATA_COUNTER_INFO
#define GPDS_DATA_COUNTER_INFO                            	0xA8                                              	
#define GPDS_DATA_COUNTER_INFO_OFFSET_ID                  	0 //size 1 byte(s)
#define GPDS_DATA_COUNTER_INFO_OFFSET_LENGTH              	1 //size 1 byte(s)
#define GPDS_DATA_COUNTER_INFO_OFFSET_CID                 	2 //size 1 byte(s)
#define GPDS_DATA_COUNTER_INFO_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define GPDS_DATA_COUNTER_INFO_OFFSET_RXCOUNT             	4 //size 8 byte(s)
#define GPDS_DATA_COUNTER_INFO_OFFSET_TXCOUNT             	12 //size 8 byte(s)
#define SIZE_GPDS_DATA_COUNTER_INFO                       	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//MESSAGES


//Definition for GPDS_LL_CONFIGURE_REQ
#define GPDS_LL_CONFIGURE_REQ                             	0x00                                                 	
#define GPDS_LL_CONFIGURE_REQ_OFFSET_UTID                 	0 //size 1 byte(s)
#define GPDS_LL_CONFIGURE_REQ_OFFSET_MSGID                	1 //size 1 byte(s)
#define GPDS_LL_CONFIGURE_REQ_OFFSET_CID                  	2 //size 1 byte(s)
#define GPDS_LL_CONFIGURE_REQ_OFFSET_PIPEHANDLE           	3 //size 1 byte(s)
#define GPDS_LL_CONFIGURE_REQ_OFFSET_PPPMODE              	4 //size 1 byte(s)
#define SIZE_GPDS_LL_CONFIGURE_REQ                        	5


//Definition for GPDS_LL_CONFIGURE_RESP
#define GPDS_LL_CONFIGURE_RESP                            	0x01                                                 	
#define GPDS_LL_CONFIGURE_RESP_OFFSET_UTID                	0 //size 1 byte(s)
#define GPDS_LL_CONFIGURE_RESP_OFFSET_MSGID               	1 //size 1 byte(s)
#define GPDS_LL_CONFIGURE_RESP_OFFSET_CID                 	2 //size 1 byte(s)
#define GPDS_LL_CONFIGURE_RESP_OFFSET_STATUS              	3 //size 1 byte(s)
#define SIZE_GPDS_LL_CONFIGURE_RESP                       	4


//Definition for GPDS_CONTEXT_ID_CREATE_REQ
#define GPDS_CONTEXT_ID_CREATE_REQ                        	0x02                                                 	
#define GPDS_CONTEXT_ID_CREATE_REQ_OFFSET_UTID            	0 //size 1 byte(s)
#define GPDS_CONTEXT_ID_CREATE_REQ_OFFSET_MSGID           	1 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ID_CREATE_REQ                   	2


//Definition for GPDS_CONTEXT_ID_CREATE_RESP
#define GPDS_CONTEXT_ID_CREATE_RESP                       	0x03                                                 	
#define GPDS_CONTEXT_ID_CREATE_RESP_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_CONTEXT_ID_CREATE_RESP_OFFSET_MSGID          	1 //size 1 byte(s)
#define GPDS_CONTEXT_ID_CREATE_RESP_OFFSET_CID            	2 //size 1 byte(s)
#define GPDS_CONTEXT_ID_CREATE_RESP_OFFSET_STATUS         	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ID_CREATE_RESP                  	4


//Definition for GPDS_CONTEXT_ID_CREATE_IND
#define GPDS_CONTEXT_ID_CREATE_IND                        	0x04                                                 	
#define GPDS_CONTEXT_ID_CREATE_IND_OFFSET_UTID            	0 //size 1 byte(s)
#define GPDS_CONTEXT_ID_CREATE_IND_OFFSET_MSGID           	1 //size 1 byte(s)
#define GPDS_CONTEXT_ID_CREATE_IND_OFFSET_CID             	2 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ID_CREATE_IND                   	3


//Definition for GPDS_CONTEXT_ID_DELETE_IND
#define GPDS_CONTEXT_ID_DELETE_IND                        	0x05                                                 	
#define GPDS_CONTEXT_ID_DELETE_IND_OFFSET_UTID            	0 //size 1 byte(s)
#define GPDS_CONTEXT_ID_DELETE_IND_OFFSET_MSGID           	1 //size 1 byte(s)
#define GPDS_CONTEXT_ID_DELETE_IND_OFFSET_CID             	2 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ID_DELETE_IND                   	3


//Definition for GPDS_LL_BIND_REQ
#define GPDS_LL_BIND_REQ                                  	0x20                                              	
#define GPDS_LL_BIND_REQ_OFFSET_UTID                      	0 //size 1 byte(s)
#define GPDS_LL_BIND_REQ_OFFSET_MSGID                     	1 //size 1 byte(s)
#define GPDS_LL_BIND_REQ_OFFSET_CID                       	2 //size 1 byte(s)
#define GPDS_LL_BIND_REQ_OFFSET_PIPEHANDLE                	3 //size 1 byte(s)
#define SIZE_GPDS_LL_BIND_REQ                             	4


//Definition for GPDS_LL_BIND_RESP
#define GPDS_LL_BIND_RESP                                 	0x21                                              	
#define GPDS_LL_BIND_RESP_OFFSET_UTID                     	0 //size 1 byte(s)
#define GPDS_LL_BIND_RESP_OFFSET_MSGID                    	1 //size 1 byte(s)
#define GPDS_LL_BIND_RESP_OFFSET_CID                      	2 //size 1 byte(s)
#define GPDS_LL_BIND_RESP_OFFSET_STATUS                   	3 //size 1 byte(s)
#define SIZE_GPDS_LL_BIND_RESP                            	4


//Definition for GPDS_CONTEXT_CONFIGURE_REQ
#define GPDS_CONTEXT_CONFIGURE_REQ                        	0x06                                                 	
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_UTID            	0 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_MSGID           	1 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_CID             	2 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_PDPTYPE         	3 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_CONTEXTTYPE     	4 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_PRIMARYCID      	5 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_FILLERBYTE1     	6 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_REQ_OFFSET_NUMBEROFSUBBLOCKS	7 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_CONFIGURE_REQ                   	8


//Definition for GPDS_CONTEXT_CONFIGURE_RESP
#define GPDS_CONTEXT_CONFIGURE_RESP                       	0x07                                                 	
#define GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_MSGID          	1 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_CID            	2 //size 1 byte(s)
#define GPDS_CONTEXT_CONFIGURE_RESP_OFFSET_STATUS         	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_CONFIGURE_RESP                  	4


//Definition for GPDS_CONTEXT_MODIFY_REQ
#define GPDS_CONTEXT_MODIFY_REQ                           	0x2A                                              	
#define GPDS_CONTEXT_MODIFY_REQ_OFFSET_UTID               	0 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_REQ_OFFSET_MSGID              	1 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_REQ_OFFSET_CID                	2 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_REQ_OFFSET_NUMBEROFSUBBLOCKS  	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_MODIFY_REQ                      	4


//Definition for GPDS_CONTEXT_MODIFY_RESP
#define GPDS_CONTEXT_MODIFY_RESP                          	0x2B                                              	
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_UTID              	0 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_MSGID             	1 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_CID               	2 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_STATUS            	3 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_CAUSE             	4 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_FILLERBYTE1       	5 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_FILLERBYTE2       	6 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_RESP_OFFSET_NUMBEROFSUBBLOCKS 	7 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_MODIFY_RESP                     	8


//Definition for GPDS_CONTEXT_MODIFY_IND
#define GPDS_CONTEXT_MODIFY_IND                           	0x2C                                              	
#define GPDS_CONTEXT_MODIFY_IND_OFFSET_UTID               	0 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_IND_OFFSET_MSGID              	1 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_IND_OFFSET_CID                	2 //size 1 byte(s)
#define GPDS_CONTEXT_MODIFY_IND_OFFSET_NUMBEROFSUBBLOCKS  	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_MODIFY_IND                      	4


//Definition for GPDS_CONTEXT_ACTIVATE_REQ
#define GPDS_CONTEXT_ACTIVATE_REQ                         	0x08                                                 	
#define GPDS_CONTEXT_ACTIVATE_REQ_OFFSET_UTID             	0 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_REQ_OFFSET_MSGID            	1 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_REQ_OFFSET_CID              	2 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_REQ_OFFSET_NUMBEROFSUBBLOCKS	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ACTIVATE_REQ                    	4


//Definition for GPDS_CONTEXT_ACTIVATE_RESP
#define GPDS_CONTEXT_ACTIVATE_RESP                        	0x09                                                 	
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_UTID            	0 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_MSGID           	1 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_CID             	2 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_STATUS          	3 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_CAUSE           	4 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_FILLERBYTE1     	5 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_FILLERBYTE2     	6 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_RESP_OFFSET_NUMBEROFSUBBLOCKS	7 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ACTIVATE_RESP                   	8


//Definition for GPDS_CONTEXT_ACTIVATE_IND
#define GPDS_CONTEXT_ACTIVATE_IND                         	0x0A                                                 	
#define GPDS_CONTEXT_ACTIVATE_IND_OFFSET_UTID             	0 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_IND_OFFSET_MSGID            	1 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_IND_OFFSET_CID              	2 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_IND_OFFSET_NUMBEROFSUBBLOCKS	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ACTIVATE_IND                    	4


//Definition for GPDS_CONTEXT_ACTIVATING_IND
#define GPDS_CONTEXT_ACTIVATING_IND                       	0x25                                              	
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_MSGID          	1 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_CID            	2 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_CLIENTTYPE     	3 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_PDPTYPE        	4 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_CONTEXTTYPE    	5 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_FILLERBYTE1    	6 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATING_IND_OFFSET_NUMBEROFSUBBLOCKS	7 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ACTIVATING_IND                  	8


//Definition for GPDS_CONTEXT_ACTIVATE_FAIL_IND
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND                    	0x1F                                              	
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_UTID        	0 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_MSGID       	1 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_CID         	2 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_CAUSE       	3 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_FILLERBYTE1 	4 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_FILLERBYTE2 	5 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_FILLERBYTE3 	6 //size 1 byte(s)
#define GPDS_CONTEXT_ACTIVATE_FAIL_IND_OFFSET_NUMBEROFSUBBLOCKS	7 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_ACTIVATE_FAIL_IND               	8


//Definition for GPDS_CONTEXT_DEACTIVATE_REQ
#define GPDS_CONTEXT_DEACTIVATE_REQ                       	0x0B                                                 	
#define GPDS_CONTEXT_DEACTIVATE_REQ_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_REQ_OFFSET_MSGID          	1 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_REQ_OFFSET_CID            	2 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_DEACTIVATE_REQ                  	3


//Definition for GPDS_CONTEXT_DEACTIVATE_RESP
#define GPDS_CONTEXT_DEACTIVATE_RESP                      	0x0C                                                 	
#define GPDS_CONTEXT_DEACTIVATE_RESP_OFFSET_UTID          	0 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_RESP_OFFSET_MSGID         	1 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_RESP_OFFSET_CID           	2 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_RESP_OFFSET_STATUS        	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_DEACTIVATE_RESP                 	4


//Definition for GPDS_CONTEXT_DEACTIVATE_IND
#define GPDS_CONTEXT_DEACTIVATE_IND                       	0x0D                                                 	
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_MSGID          	1 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_CID            	2 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_CAUSE          	3 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_FILLERBYTE1    	4 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_FILLERBYTE2    	5 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_FILLERBYTE3    	6 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATE_IND_OFFSET_NUMBEROFSUBBLOCKS	7 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_DEACTIVATE_IND                  	8


//Definition for GPDS_CONTEXT_DEACTIVATING_IND
#define GPDS_CONTEXT_DEACTIVATING_IND                     	0x2F                                              	
#define GPDS_CONTEXT_DEACTIVATING_IND_OFFSET_UTID         	0 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATING_IND_OFFSET_MSGID        	1 //size 1 byte(s)
#define GPDS_CONTEXT_DEACTIVATING_IND_OFFSET_CID          	2 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_DEACTIVATING_IND                	3


//Definition for GPDS_CONTEXT_NWI_ACT_REQUEST_IND
#define GPDS_CONTEXT_NWI_ACT_REQUEST_IND                  	0x0E                                                 	
#define GPDS_CONTEXT_NWI_ACT_REQUEST_IND_OFFSET_UTID      	0 //size 1 byte(s)
#define GPDS_CONTEXT_NWI_ACT_REQUEST_IND_OFFSET_MSGID     	1 //size 1 byte(s)
#define GPDS_CONTEXT_NWI_ACT_REQUEST_IND_OFFSET_PDPTYPE   	2 //size 1 byte(s)
#define GPDS_CONTEXT_NWI_ACT_REQUEST_IND_OFFSET_NUMBEROFSUBBLOCKS	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_NWI_ACT_REQUEST_IND             	4


//Definition for GPDS_CONTEXT_NWI_ACT_REJECT_REQ
#define GPDS_CONTEXT_NWI_ACT_REJECT_REQ                   	0x0F                                                 	
#define GPDS_CONTEXT_NWI_ACT_REJECT_REQ_OFFSET_UTID       	0 //size 1 byte(s)
#define GPDS_CONTEXT_NWI_ACT_REJECT_REQ_OFFSET_MSGID      	1 //size 1 byte(s)
#define GPDS_CONTEXT_NWI_ACT_REJECT_REQ_OFFSET_CAUSE      	2 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_NWI_ACT_REJECT_REQ              	3


//Definition for GPDS_CONTEXT_NWI_ACT_REJECT_RESP
#define GPDS_CONTEXT_NWI_ACT_REJECT_RESP                  	0x10                                              	
#define GPDS_CONTEXT_NWI_ACT_REJECT_RESP_OFFSET_UTID      	0 //size 1 byte(s)
#define GPDS_CONTEXT_NWI_ACT_REJECT_RESP_OFFSET_MSGID     	1 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_NWI_ACT_REJECT_RESP             	2


//Definition for GPDS_CONFIGURE_REQ
#define GPDS_CONFIGURE_REQ                                	0x11                                              	
#define GPDS_CONFIGURE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define GPDS_CONFIGURE_REQ_OFFSET_MSGID                   	1 //size 1 byte(s)
#define GPDS_CONFIGURE_REQ_OFFSET_ATTACHMODE              	2 //size 1 byte(s)
#define GPDS_CONFIGURE_REQ_OFFSET_MTCONTEXTACTMODE        	3 //size 1 byte(s)
#define GPDS_CONFIGURE_REQ_OFFSET_CLASSCMODE              	4 //size 1 byte(s)
#define GPDS_CONFIGURE_REQ_OFFSET_AOLCONTEXT              	5 //size 1 byte(s)
#define GPDS_CONFIGURE_REQ_OFFSET_FILLERBYTE1             	6 //size 1 byte(s)
#define GPDS_CONFIGURE_REQ_OFFSET_FILLERBYTE2             	7 //size 1 byte(s)
#define SIZE_GPDS_CONFIGURE_REQ                           	8


//Definition for GPDS_CONFIGURE_RESP
#define GPDS_CONFIGURE_RESP                               	0x12                                              	
#define GPDS_CONFIGURE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define GPDS_CONFIGURE_RESP_OFFSET_MSGID                  	1 //size 1 byte(s)
#define GPDS_CONFIGURE_RESP_OFFSET_STATUS                 	2 //size 1 byte(s)
#define SIZE_GPDS_CONFIGURE_RESP                          	3


//Definition for GPDS_ATTACH_REQ
#define GPDS_ATTACH_REQ                                   	0x13                                              	
#define GPDS_ATTACH_REQ_OFFSET_UTID                       	0 //size 1 byte(s)
#define GPDS_ATTACH_REQ_OFFSET_MSGID                      	1 //size 1 byte(s)
#define GPDS_ATTACH_REQ_OFFSET_FOLLOW                     	2 //size 1 byte(s)
#define SIZE_GPDS_ATTACH_REQ                              	3


//Definition for GPDS_ATTACH_RESP
#define GPDS_ATTACH_RESP                                  	0x14                                              	
#define GPDS_ATTACH_RESP_OFFSET_UTID                      	0 //size 1 byte(s)
#define GPDS_ATTACH_RESP_OFFSET_MSGID                     	1 //size 1 byte(s)
#define GPDS_ATTACH_RESP_OFFSET_STATUS                    	2 //size 1 byte(s)
#define GPDS_ATTACH_RESP_OFFSET_CAUSE                     	3 //size 1 byte(s)
#define GPDS_ATTACH_RESP_OFFSET_ATTACHTYPE                	4 //size 1 byte(s)
#define SIZE_GPDS_ATTACH_RESP                             	5


//Definition for GPDS_ATTACH_IND
#define GPDS_ATTACH_IND                                   	0x15                                              	
#define GPDS_ATTACH_IND_OFFSET_UTID                       	0 //size 1 byte(s)
#define GPDS_ATTACH_IND_OFFSET_MSGID                      	1 //size 1 byte(s)
#define GPDS_ATTACH_IND_OFFSET_ATTACHTYPE                 	2 //size 1 byte(s)
#define SIZE_GPDS_ATTACH_IND                              	3


//Definition for GPDS_ATTACH_FAIL_IND
#define GPDS_ATTACH_FAIL_IND                              	0x2D                                              	
#define GPDS_ATTACH_FAIL_IND_OFFSET_UTID                  	0 //size 1 byte(s)
#define GPDS_ATTACH_FAIL_IND_OFFSET_MSGID                 	1 //size 1 byte(s)
#define GPDS_ATTACH_FAIL_IND_OFFSET_CAUSE                 	2 //size 1 byte(s)
#define SIZE_GPDS_ATTACH_FAIL_IND                         	3


//Definition for GPDS_DETACH_REQ
#define GPDS_DETACH_REQ                                   	0x16                                              	
#define GPDS_DETACH_REQ_OFFSET_UTID                       	0 //size 1 byte(s)
#define GPDS_DETACH_REQ_OFFSET_MSGID                      	1 //size 1 byte(s)
#define GPDS_DETACH_REQ_OFFSET_FILLERBYTE1                	2 //size 1 byte(s)
#define GPDS_DETACH_REQ_OFFSET_NUMBEROFSUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_GPDS_DETACH_REQ                              	4


//Definition for GPDS_DETACH_RESP
#define GPDS_DETACH_RESP                                  	0x17                                              	
#define GPDS_DETACH_RESP_OFFSET_UTID                      	0 //size 1 byte(s)
#define GPDS_DETACH_RESP_OFFSET_MSGID                     	1 //size 1 byte(s)
#define GPDS_DETACH_RESP_OFFSET_STATUS                    	2 //size 1 byte(s)
#define GPDS_DETACH_RESP_OFFSET_DETACHTYPE                	3 //size 1 byte(s)
#define SIZE_GPDS_DETACH_RESP                             	4


//Definition for GPDS_DETACH_IND
#define GPDS_DETACH_IND                                   	0x18                                              	
#define GPDS_DETACH_IND_OFFSET_UTID                       	0 //size 1 byte(s)
#define GPDS_DETACH_IND_OFFSET_MSGID                      	1 //size 1 byte(s)
#define GPDS_DETACH_IND_OFFSET_CAUSE                      	2 //size 1 byte(s)
#define GPDS_DETACH_IND_OFFSET_DETACHTYPE                 	3 //size 1 byte(s)
#define SIZE_GPDS_DETACH_IND                              	4


//Definition for GPDS_STATUS_REQ
#define GPDS_STATUS_REQ                                   	0x19                                              	
#define GPDS_STATUS_REQ_OFFSET_UTID                       	0 //size 1 byte(s)
#define GPDS_STATUS_REQ_OFFSET_MSGID                      	1 //size 1 byte(s)
#define SIZE_GPDS_STATUS_REQ                              	2


//Definition for GPDS_STATUS_RESP
#define GPDS_STATUS_RESP                                  	0x1A                                              	
#define GPDS_STATUS_RESP_OFFSET_UTID                      	0 //size 1 byte(s)
#define GPDS_STATUS_RESP_OFFSET_MSGID                     	1 //size 1 byte(s)
#define GPDS_STATUS_RESP_OFFSET_ATTACHSTATUS              	2 //size 1 byte(s)
#define GPDS_STATUS_RESP_OFFSET_CONTEXTCOUNT              	3 //size 1 byte(s)
#define GPDS_STATUS_RESP_OFFSET_TXBYTECOUNT               	4 //size 4 byte(s)
#define GPDS_STATUS_RESP_OFFSET_RXBYTECOUNT               	8 //size 4 byte(s)
#define GPDS_STATUS_RESP_OFFSET_TRANSFERSTATUS            	12 //size 1 byte(s)
#define GPDS_STATUS_RESP_OFFSET_TRANSFERCAUSE             	13 //size 1 byte(s)
#define GPDS_STATUS_RESP_OFFSET_CIDLIST                   	14 //size 1 byte(s)
#define SIZE_GPDS_STATUS_RESP                             	15
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_CONTEXT_STATUS_REQ
#define GPDS_CONTEXT_STATUS_REQ                           	0x22                                              	
#define GPDS_CONTEXT_STATUS_REQ_OFFSET_UTID               	0 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_REQ_OFFSET_MSGID              	1 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_REQ_OFFSET_CID                	2 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_STATUS_REQ                      	3


//Definition for GPDS_CONTEXT_STATUS_RESP
#define GPDS_CONTEXT_STATUS_RESP                          	0x23                                              	
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_UTID              	0 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_MSGID             	1 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_CID               	2 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_STATUS            	3 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_TXBYTECOUNT       	4 //size 4 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_RXBYTECOUNT       	8 //size 4 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_FILLERBYTE1       	12 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_FILLERBYTE2       	13 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_FILLERBYTE3       	14 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_RESP_OFFSET_NUMBEROFSUBBLOCKS 	15 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_STATUS_RESP                     	16


//Definition for GPDS_CONTEXT_STATUS_IND
#define GPDS_CONTEXT_STATUS_IND                           	0x24                                              	
#define GPDS_CONTEXT_STATUS_IND_OFFSET_UTID               	0 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_IND_OFFSET_MSGID              	1 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_IND_OFFSET_FILLERBYTE1        	2 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_IND_OFFSET_CID                	3 //size 1 byte(s)
#define GPDS_CONTEXT_STATUS_IND_OFFSET_TXBYTECOUNT        	4 //size 4 byte(s)
#define GPDS_CONTEXT_STATUS_IND_OFFSET_RXBYTECOUNT        	8 //size 4 byte(s)
#define SIZE_GPDS_CONTEXT_STATUS_IND                      	12


//Definition for GPDS_TRANSFER_STATUS_IND
#define GPDS_TRANSFER_STATUS_IND                          	0x1E                                              	
#define GPDS_TRANSFER_STATUS_IND_OFFSET_UTID              	0 //size 1 byte(s)
#define GPDS_TRANSFER_STATUS_IND_OFFSET_MSGID             	1 //size 1 byte(s)
#define GPDS_TRANSFER_STATUS_IND_OFFSET_STATUS            	2 //size 1 byte(s)
#define GPDS_TRANSFER_STATUS_IND_OFFSET_TRANSFERCAUSE     	3 //size 1 byte(s)
#define SIZE_GPDS_TRANSFER_STATUS_IND                     	4


//Definition for GPDS_CONFIGURATION_INFO_REQ
#define GPDS_CONFIGURATION_INFO_REQ                       	0x30                                              	
#define GPDS_CONFIGURATION_INFO_REQ_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_REQ_OFFSET_MSGID          	1 //size 1 byte(s)
#define SIZE_GPDS_CONFIGURATION_INFO_REQ                  	2


//Definition for GPDS_CONFIGURATION_INFO_RESP
#define GPDS_CONFIGURATION_INFO_RESP                      	0x31                                              	
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_UTID          	0 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_MSGID         	1 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_ATTACHMODE    	2 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_MTACTMODE     	3 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_CLASSCMODE    	4 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_AOLCONTEXT    	5 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_FILLERBYTE1   	6 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_RESP_OFFSET_FILLERBYTE2   	7 //size 1 byte(s)
#define SIZE_GPDS_CONFIGURATION_INFO_RESP                 	8


//Definition for GPDS_CONFIGURATION_INFO_IND
#define GPDS_CONFIGURATION_INFO_IND                       	0x32                                              	
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_MSGID          	1 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_ATTACHMODE     	2 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_MTACTMODE      	3 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_CLASSCMODE     	4 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_AOLCONTEXT     	5 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_FILLERBYTE1    	6 //size 1 byte(s)
#define GPDS_CONFIGURATION_INFO_IND_OFFSET_FILLERBYTE2    	7 //size 1 byte(s)
#define SIZE_GPDS_CONFIGURATION_INFO_IND                  	8


//Definition for GPDS_CONTEXT_AUTH_REQ
#define GPDS_CONTEXT_AUTH_REQ                             	0x33                                              	
#define GPDS_CONTEXT_AUTH_REQ_OFFSET_UTID                 	0 //size 1 byte(s)
#define GPDS_CONTEXT_AUTH_REQ_OFFSET_MSGID                	1 //size 1 byte(s)
#define GPDS_CONTEXT_AUTH_REQ_OFFSET_CID                  	2 //size 1 byte(s)
#define GPDS_CONTEXT_AUTH_REQ_OFFSET_NUMBEROFSUBBLOCKS    	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_AUTH_REQ                        	4


//Definition for GPDS_CONTEXT_AUTH_RESP
#define GPDS_CONTEXT_AUTH_RESP                            	0x34                                              	
#define GPDS_CONTEXT_AUTH_RESP_OFFSET_UTID                	0 //size 1 byte(s)
#define GPDS_CONTEXT_AUTH_RESP_OFFSET_MSGID               	1 //size 1 byte(s)
#define GPDS_CONTEXT_AUTH_RESP_OFFSET_CID                 	2 //size 1 byte(s)
#define GPDS_CONTEXT_AUTH_RESP_OFFSET_STATUS              	3 //size 1 byte(s)
#define SIZE_GPDS_CONTEXT_AUTH_RESP                       	4


//Definition for GPDS_RADIO_ACTIVITY_IND
#define GPDS_RADIO_ACTIVITY_IND                           	0x37                                              	
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_UTID               	0 //size 1 byte(s)
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_MSGID              	1 //size 1 byte(s)
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_DIRECTION          	2 //size 1 byte(s)
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_STATUS             	3 //size 1 byte(s)
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_CID                	4 //size 1 byte(s)
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_FILLERBYTE1        	5 //size 1 byte(s)
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_FILLERBYTE2        	6 //size 1 byte(s)
#define GPDS_RADIO_ACTIVITY_IND_OFFSET_FILLERBYTE3        	7 //size 1 byte(s)
#define SIZE_GPDS_RADIO_ACTIVITY_IND                      	8


//Definition for GPDS_FORCED_READY_STATE_REQ
#define GPDS_FORCED_READY_STATE_REQ                       	0x38                                              	
#define GPDS_FORCED_READY_STATE_REQ_OFFSET_UTID           	0 //size 1 byte(s)
#define GPDS_FORCED_READY_STATE_REQ_OFFSET_MSGID          	1 //size 1 byte(s)
#define GPDS_FORCED_READY_STATE_REQ_OFFSET_STATE          	2 //size 1 byte(s)
#define SIZE_GPDS_FORCED_READY_STATE_REQ                  	3


//Definition for GPDS_FORCED_READY_STATE_RESP
#define GPDS_FORCED_READY_STATE_RESP                      	0x39                                              	
#define GPDS_FORCED_READY_STATE_RESP_OFFSET_UTID          	0 //size 1 byte(s)
#define GPDS_FORCED_READY_STATE_RESP_OFFSET_MSGID         	1 //size 1 byte(s)
#define GPDS_FORCED_READY_STATE_RESP_OFFSET_STATUS        	2 //size 1 byte(s)
#define SIZE_GPDS_FORCED_READY_STATE_RESP                 	3


//Definition for GPDS_CONTEXTS_CLEAR_REQ
#define GPDS_CONTEXTS_CLEAR_REQ                           	0x3A                                              	
#define GPDS_CONTEXTS_CLEAR_REQ_OFFSET_UTID               	0 //size 1 byte(s)
#define GPDS_CONTEXTS_CLEAR_REQ_OFFSET_MSGID              	1 //size 1 byte(s)
#define SIZE_GPDS_CONTEXTS_CLEAR_REQ                      	2


//Definition for GPDS_CONTEXTS_CLEAR_RESP
#define GPDS_CONTEXTS_CLEAR_RESP                          	0x3B                                              	
#define GPDS_CONTEXTS_CLEAR_RESP_OFFSET_UTID              	0 //size 1 byte(s)
#define GPDS_CONTEXTS_CLEAR_RESP_OFFSET_MSGID             	1 //size 1 byte(s)
#define SIZE_GPDS_CONTEXTS_CLEAR_RESP                     	2


//Definition for GPDS_RESOURCE_CONTROL_IND
#define GPDS_RESOURCE_CONTROL_IND                         	0x50                                              	
#define GPDS_RESOURCE_CONTROL_IND_OFFSET_UTID             	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_IND_OFFSET_MSGID            	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_IND_OFFSET_SEQUENCEID       	2 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_IND_OFFSET_SUBBLOCKCOUNT    	3 //size 1 byte(s)
#define SIZE_GPDS_RESOURCE_CONTROL_IND                    	4


//Definition for GPDS_RESOURCE_CONTROL_REQ
#define GPDS_RESOURCE_CONTROL_REQ                         	0x51                                              	
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_UTID             	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_MSGID            	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_SEQUENCEID       	2 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_CCRESULT         	3 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_FILLERBYTE1      	4 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_FILLERBYTE2      	5 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_FILLERBYTE3      	6 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_REQ_OFFSET_SUBBLOCKCOUNT    	7 //size 1 byte(s)
#define SIZE_GPDS_RESOURCE_CONTROL_REQ                    	8


//Definition for GPDS_RESOURCE_CONTROL_RESP
#define GPDS_RESOURCE_CONTROL_RESP                        	0x52                                              	
#define GPDS_RESOURCE_CONTROL_RESP_OFFSET_TRANSID         	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_RESP_OFFSET_MESSAGEID       	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_RESP_OFFSET_SEQUENCEID      	2 //size 1 byte(s)
#define GPDS_RESOURCE_CONTROL_RESP_OFFSET_STATUS          	3 //size 1 byte(s)
#define SIZE_GPDS_RESOURCE_CONTROL_RESP                   	4


//Definition for GPDS_RESOURCE_CONF_REQ
#define GPDS_RESOURCE_CONF_REQ                            	0x54                                              	
#define GPDS_RESOURCE_CONF_REQ_OFFSET_TRANSID             	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_REQ_OFFSET_CONFOPERATION       	2 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_REQ_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_GPDS_RESOURCE_CONF_REQ                       	4


//Definition for GPDS_RESOURCE_CONF_RESP
#define GPDS_RESOURCE_CONF_RESP                           	0x55                                              	
#define GPDS_RESOURCE_CONF_RESP_OFFSET_TRANSID            	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_RESP_OFFSET_CONFOPERATION      	2 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_RESP_OFFSET_STATUS             	3 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_RESP_OFFSET_FILLERBYTE1        	4 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_RESP_OFFSET_FILLERBYTE2        	5 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_RESP_OFFSET_FILLERBYTE3        	6 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_RESP_OFFSET_SUBBLOCKCOUNT      	7 //size 1 byte(s)
#define SIZE_GPDS_RESOURCE_CONF_RESP                      	8


//Definition for GPDS_RESOURCE_CONF_IND
#define GPDS_RESOURCE_CONF_IND                            	0x53                                              	
#define GPDS_RESOURCE_CONF_IND_OFFSET_TRANSID             	0 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_IND_OFFSET_CONFSTATUS          	2 //size 1 byte(s)
#define GPDS_RESOURCE_CONF_IND_OFFSET_SUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_GPDS_RESOURCE_CONF_IND                       	4


//Definition for GPDS_DATA_COUNTER_REQ
#define GPDS_DATA_COUNTER_REQ                             	0x58                                              	
#define GPDS_DATA_COUNTER_REQ_OFFSET_TRANSID              	0 //size 1 byte(s)
#define GPDS_DATA_COUNTER_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define GPDS_DATA_COUNTER_REQ_OFFSET_CONFSTATUS           	2 //size 1 byte(s)
#define GPDS_DATA_COUNTER_REQ_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define GPDS_DATA_COUNTER_REQ_OFFSET_UPDATEINTERVALTIME   	4 //size 4 byte(s)
#define GPDS_DATA_COUNTER_REQ_OFFSET_UPDATEINTERVALDATACOUNT	8 //size 4 byte(s)
#define SIZE_GPDS_DATA_COUNTER_REQ                        	12


//Definition for GPDS_DATA_COUNTER_RESP
#define GPDS_DATA_COUNTER_RESP                            	0x59                                              	
#define GPDS_DATA_COUNTER_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define GPDS_DATA_COUNTER_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define GPDS_DATA_COUNTER_RESP_OFFSET_FILLERBYTE1         	2 //size 2 byte(s)
#define GPDS_DATA_COUNTER_RESP_OFFSET_TOTALRXCOUNT        	4 //size 8 byte(s)
#define GPDS_DATA_COUNTER_RESP_OFFSET_TOTALTXCOUNT        	12 //size 8 byte(s)
#define GPDS_DATA_COUNTER_RESP_OFFSET_FILLERBYTE2         	20 //size 2 byte(s)
#define GPDS_DATA_COUNTER_RESP_OFFSET_FILLERBYTE3         	22 //size 1 byte(s)
#define GPDS_DATA_COUNTER_RESP_OFFSET_SUBBLOCKCOUNT       	23 //size 1 byte(s)
#define SIZE_GPDS_DATA_COUNTER_RESP                       	24
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_DATA_COUNTER_IND
#define GPDS_DATA_COUNTER_IND                             	0x5A                                              	
#define GPDS_DATA_COUNTER_IND_OFFSET_TRANSID              	0 //size 1 byte(s)
#define GPDS_DATA_COUNTER_IND_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define GPDS_DATA_COUNTER_IND_OFFSET_FILLERBYTE1          	2 //size 2 byte(s)
#define GPDS_DATA_COUNTER_IND_OFFSET_TOTALRXCOUNT         	4 //size 8 byte(s)
#define GPDS_DATA_COUNTER_IND_OFFSET_TOTALTXCOUNT         	12 //size 8 byte(s)
#define GPDS_DATA_COUNTER_IND_OFFSET_FILLERBYTE2          	20 //size 2 byte(s)
#define GPDS_DATA_COUNTER_IND_OFFSET_FILLERBYTE3          	22 //size 1 byte(s)
#define GPDS_DATA_COUNTER_IND_OFFSET_SUBBLOCKCOUNT        	23 //size 1 byte(s)
#define SIZE_GPDS_DATA_COUNTER_IND                        	24
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.

#endif