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
* Name      : PN_SMS [ 0x02 ] Resource SMS Server
* Version   : 008.002
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_SMSISI_H_
#define _SYMBIAN_ISI_SMSISI_H_

#define SMS_SERVER_ISI_VERSION_Z	 8
#define SMS_SERVER_ISI_VERSION_Y	 4


//CONSTANTS
#define SMS_ADDRESS_MAX_LEN                               	0x0C                                                 	
#define SMS_DELIVER_UD_MAX_LEN                            	0x8C                                              	
#define SMS_STATUS_RPRT_UD_MAX_LEN                        	0x8F                                              	
#define SMS_SUBMIT_UD_MAX_LEN                             	0x8C                                              	
#define SMS_COMMAND_DATA_MAX_LEN                          	0x9C                                              	
#define SMS_MT_UNDEFINED_MAX_LEN                          	0xE7                                              	
#define SMS_MO_UNDEFINED_MAX_LEN                          	0xE6                                              	
#define SMS_DELIVER_ACK_UD_MAX_LEN                        	0x9F                                              	
#define SMS_DELIVER_ERR_UD_MAX_LEN                        	0x9E                                              	
#define SMS_SUBMIT_ACK_UD_MAX_LEN                         	0x98                                              	
#define SMS_SUBMIT_ERR_UD_MAX_LEN                         	0x97                                              	
#define SMS_TPDU_MAX_LEN                                  	0xE8                                              	
#define SMS_CB_MESSAGE_CONTENT_SIZE                       	0x52                                              	
#define SMS_SETTING_REPLY_PATH                            	0x01                                                 	
#define SMS_SETTING_REPLY_PATH_MASK                       	0xFE                                              	
#define SMS_SETTING_CONF_REQUEST                          	0x02                                                 	
#define SMS_SETTING_CONF_REQUEST_MASK                     	0xFD                                              	
#define SMS_TP_FCS_TEL_INTERWRK_NOT_SUPPORTED             	0x80                                              	
#define SMS_TP_FCS_SM_TYPE0_NOT_SUPPORTED                 	0x81                                              	
#define SMS_TP_FCS_CANNOT_REPLACE_SM                      	0x82                                              	
#define SMS_TP_FCS_UNSPECIFIED_PID_ERR                    	0x8F                                              	
#define SMS_TP_FCS_DCS_NOT_SUPPORTED                      	0x90                                              	
#define SMS_TP_FCS_MESSAGE_CLASS_NOT_SUPPORTED            	0x91                                              	
#define SMS_TP_FCS_UNSPECIFIED_DCS_ERR                    	0x9F                                              	
#define SMS_TP_FCS_CMD_CANNOT_BE_ACTIONED                 	0xA0                                              	
#define SMS_TP_FCS_CMD_UNSUPPORTED                        	0xA1                                              	
#define SMS_TP_FCS_UNSPECIFIED_CMD_ERR                    	0xAF                                              	
#define SMS_TP_FCS_TPDU_NOT_SUPPORTED                     	0xB0                                              	
#define SMS_TP_FCS_SC_BUSY                                	0xC0                                              	
#define SMS_TP_FCS_NO_SC_SUBSCRIPTION                     	0xC1                                              	
#define SMS_TP_FCS_SC_SYSTEM_FAILURE                      	0xC2                                              	
#define SMS_TP_FCS_INVALID_SME_ADDR                       	0xC3                                              	
#define SMS_TP_FCS_DEST_SME_BARRED                        	0xC4                                              	
#define SMS_TP_FCS_SM_REJECTED_DUPLICATE_SM               	0xC5                                              	
#define SMS_TP_FCS_TP_VPF_NOT_SUPPORTED                   	0xC6                                              	
#define SMS_TP_FCS_TP_VP_NOT_SUPPORTED                    	0xC7                                              	
#define SMS_TP_FCS_SIM_STORAGE_FULL                       	0xD0                                              	
#define SMS_TP_FCS_NO_STORAGE_IN_SIM                      	0xD1                                              	
#define SMS_TP_FCS_ERROR_IN_MS                            	0xD2                                              	
#define SMS_TP_FCS_MEM_CAP_EXCEEDED                       	0xD3                                              	
#define SMS_TP_FCS_SIM_ATK_BUSY                           	0xD4                                              	
#define SMS_TP_FCS_SIM_DATA_DOWNLOAD_ERROR                	0xD5                                              	
#define SMS_TP_FCS_UNSPECIFIED                            	0xFF                                              	
#define SMS_TP_MTI_MASK                                   	0x03                                                 	
#define SMS_TP_MTI_DELIVER                                	0x00                                                 	
#define SMS_TP_MTI_DELIVER_REPORT                         	0x00                                                 	
#define SMS_TP_MTI_STATUS_REPORT                          	0x02                                                 	
#define SMS_TP_MTI_COMMAND                                	0x02                                                 	
#define SMS_TP_MTI_SUBMIT                                 	0x01                                                 	
#define SMS_TP_MTI_SUBMIT_REPORT                          	0x01                                                 	
#define SMS_TP_MTI_UNDEFINED                              	0x03                                                 	
#define SMS_TP_MMS_MASK                                   	0x04                                                 	
#define SMS_TP_MMS_MORE_MESSAGES                          	0x00                                                 	
#define SMS_TP_MMS_NO_MORE_MESSAGES                       	0x04                                                 	
#define SMS_TP_VPF_MASK                                   	0x18                                              	
#define SMS_TP_VPF_NOT_PRESENT                            	0x00                                                 	
#define SMS_TP_VPF_RELATIVE_FORMAT                        	0x10                                              	
#define SMS_TP_VPF_ENHANCED_FORMAT                        	0x08                                                 	
#define SMS_TP_VPF_ABSOLUTE_FORMAT                        	0x18                                              	
#define SMS_TP_VP_RF_WEEK_BASE_VALUE                      	0xC0                                              	
#define SMS_TP_VP_RF_DAY_BASE_VALUE                       	0xA6                                              	
#define SMS_TP_VP_RF_HOUR_BASE_VALUE                      	0x8F                                              	
#define SMS_TP_SRI_MASK                                   	0x20                                              	
#define SMS_TP_SRI_NOT_RETURNED                           	0x00                                                 	
#define SMS_TP_SRI_RETURNED                               	0x20                                              	
#define SMS_TP_SRR_MASK                                   	0x20                                              	
#define SMS_TP_SRR_NOT_REQUESTED                          	0x00                                                 	
#define SMS_TP_SRR_REQUESTED                              	0x20                                              	
#define SMS_TP_RP_MASK                                    	0x80                                              	
#define SMS_TP_RP_NOT_SET                                 	0x00                                                 	
#define SMS_TP_RP_SET                                     	0x80                                              	
#define SMS_TP_RD_MASK                                    	0x04                                                 	
#define SMS_TP_RD_ACCEPT                                  	0x00                                                 	
#define SMS_TP_RD_REJECT                                  	0x04                                                 	
#define SMS_TP_SRQ_MASK                                   	0x20                                              	
#define SMS_TP_SRQ_SUBMIT                                 	0x00                                                 	
#define SMS_TP_SRQ_COMMAND                                	0x20                                              	
#define SMS_TP_UDHI_MASK                                  	0x40                                              	
#define SMS_TP_UDHI_MESSAGE                               	0x00                                                 	
#define SMS_TP_UDHI_HEADER_MESSAGE                        	0x40                                              	
#define SMS_TP_PI_EXTENSION_BIT_MASK                      	0x80                                              	
#define SMS_TP_PI_EXTENSION_AVAIL                         	0x80                                              	
#define SMS_TP_PI_EXTENSION_NOT_AVAIL                     	0x00                                                 	
#define SMS_TP_PI_UDL_AVAIL                               	0x04                                                 	
#define SMS_TP_PI_UDL_NOT_AVAIL                           	0x00                                                 	
#define SMS_TP_PI_DCS_AVAIL                               	0x02                                                 	
#define SMS_TP_PI_DCS_NOT_AVAIL                           	0x00                                                 	
#define SMS_TP_PI_PID_AVAIL                               	0x01                                                 	
#define SMS_TP_PI_PID_NOT_AVAIL                           	0x00                                                 	
#define SMS_TP_DCS_SMS_GDC_UNCOMPRESSED                   	0x00                                                 	
#define SMS_TP_DCS_SMS_GDC_COMPRESSED                     	0x20                                              	
#define SMS_TP_DCS_SMS_GDC_NO_CLASS_MEANING               	0x00                                                 	
#define SMS_TP_DCS_SMS_GDC_CLASS_MEANING                  	0x10                                              	
#define SMS_TP_DCS_SMS_GDC_CS_GSM_7BIT                    	0x00                                                 	
#define SMS_TP_DCS_SMS_GDC_CS_8BIT                        	0x04                                                 	
#define SMS_TP_DCS_SMS_GDC_CS_UCS2                        	0x08                                                 	
#define SMS_TP_DCS_SMS_GDC_CS_RESERVED                    	0x0C                                                 	
#define SMS_TP_DCS_SMS_GDC_CLASS0                         	0x00                                                 	
#define SMS_TP_DCS_SMS_GDC_CLASS1_ME                      	0x01                                                 	
#define SMS_TP_DCS_SMS_GDC_CLASS2_SIM                     	0x02                                                 	
#define SMS_TP_DCS_SMS_GDC_CLASS3_TE                      	0x03                                                 	
#define SMS_TP_DCS_SMS_MWI_IS_INACTIVE                    	0x00                                                 	
#define SMS_TP_DCS_SMS_MWI_IS_ACTIVE                      	0x08                                                 	
#define SMS_TP_DCS_SMS_MWI_VOICEMAIL                      	0x00                                                 	
#define SMS_TP_DCS_SMS_MWI_FAX                            	0x01                                                 	
#define SMS_TP_DCS_SMS_MWI_EMAIL                          	0x02                                                 	
#define SMS_TP_DCS_SMS_MWI_OTHER                          	0x03                                                 	
#define SMS_TP_DCS_SMS_DATA_CODING_MESSAGE_CLASS          	0xF0                                              	
#define SMS_TP_DCS_SMS_DCMC_MC_GSM_7BIT                   	0x00                                                 	
#define SMS_TP_DCS_SMS_DCMC_MC_GSM_8BIT                   	0x02                                                 	
#define SMS_TP_DCS_SMS_DCMC_MC_UCS2                       	0x04                                                 	
#define SMS_TP_DCS_SMS_DCMC_CLASS0                        	0x00                                                 	
#define SMS_TP_DCS_SMS_DCMC_CLASS1_ME                     	0x01                                                 	
#define SMS_TP_DCS_SMS_DCMC_CLASS2_SIM                    	0x02                                                 	
#define SMS_TP_DCS_SMS_DCMC_CLASS3_TE                     	0x03                                                 	
#define SMS_TP_DCS_SMS_ADG_MASK_FOR_GDC                   	0x40                                              	
#define SMS_TP_DCS_MSG_WAITING_MASK                       	0xF0                                              	
#define SMS_TP_DCS_MSG_WAITING_DISCARD                    	0xC0                                              	
#define SMS_TP_DCS_MSG_WAITING_STORE                      	0xD0                                              	
#define SMS_TP_DCS_MSG_WAITING_UCS2                       	0xE0                                              	
#define SMS_TP_DCS_CBS_LANGUAGE_GSM_7BIT                  	0x00                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_GERMAN                    	0x00                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_ENGLISH                   	0x01                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_ITALIAN                   	0x02                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_FRENCH                    	0x03                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_SPANISH                   	0x04                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_DUTCH                     	0x05                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_SWEDISH                   	0x06                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_DANISH                    	0x07                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_PORTUGESE                 	0x08                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_FINNISH                   	0x09                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_NORWEGIAN                 	0x0A                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_GREEK                     	0x0B                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_TURKISH                   	0x0C                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_HUNGARIAN                 	0x0D                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_POLISH                    	0x0E                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_UNSPECIFIED               	0x0F                                                 	
#define SMS_TP_DCS_CBS_LANGUAGE_INDICATED_GSM_7BIT        	0x10                                              	
#define SMS_TP_DCS_CBS_LANGUAGE_INDICATED_UCS2            	0x11                                              	
#define SMS_TP_DCS_CBS_LANGUAGE_GSM_7BIT_EXTENDED         	0x10                                              	
#define SMS_TP_DCS_CBS_GSM_7BIT_CZECH                     	0x00                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_HEBREW                    	0x01                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_ARABIC                    	0x02                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_RUSSIAN                   	0x03                                                 	
#define SMS_TP_DCS_CBS_GSM_7BIT_ICELANDIC                 	0x04                                                 	
#define SMS_TP_DCS_CBS_GENERAL_DATA_CODING                	0x40                                              	
#define SMS_TP_DCS_CBS_GDC_UNCOMPRESSED                   	0x00                                                 	
#define SMS_TP_DCS_CBS_GDC_COMPRESSED                     	0x20                                              	
#define SMS_TP_DCS_CBS_GDC_NO_CLASS_MEANING               	0x00                                                 	
#define SMS_TP_DCS_CBS_GDC_CLASS_MEANING                  	0x10                                              	
#define SMS_TP_DCS_CBS_GDC_CS_GSM_7BIT                    	0x00                                                 	
#define SMS_TP_DCS_CBS_GDC_CS_8BIT                        	0x04                                                 	
#define SMS_TP_DCS_CBS_GDC_CS_UCS2                        	0x08                                                 	
#define SMS_TP_DCS_CBS_GDC_CS_RESERVED                    	0x0C                                                 	
#define SMS_TP_DCS_CBS_GDC_CLASS0                         	0x00                                                 	
#define SMS_TP_DCS_CBS_GDC_CLASS1_ME                      	0x01                                                 	
#define SMS_TP_DCS_CBS_GDC_CLASS2_SIM                     	0x02                                                 	
#define SMS_TP_DCS_CBS_GDC_CLASS3_TE                      	0x03                                                 	
#define SMS_TP_DCS_CBS_WAP_MESSAGES                       	0xF0                                              	
#define SMS_TP_DCS_CBS_DATA_CODING_MSG_HANDLING           	0xF0                                              	
#define SMS_TP_DCS_CBS_DCMH_MC_GSM_7BIT                   	0x00                                                 	
#define SMS_TP_DCS_CBS_DCMH_MC_GSM_8BIT                   	0x02                                                 	
#define SMS_TP_DCS_CBS_DCMH_MC_UCS2                       	0x04                                                 	
#define SMS_TP_DCS_CBS_DCMH_CLASS0                        	0x00                                                 	
#define SMS_TP_DCS_CBS_DCMH_CLASS1_ME                     	0x01                                                 	
#define SMS_TP_DCS_CBS_DCMH_CLASS2_SIM                    	0x02                                                 	
#define SMS_TP_DCS_CBS_DCMH_CLASS3_TE                     	0x03                                                 	
#define SMS_TP_ADDR_TON_MASK                              	0x70                                              	
#define SMS_TP_ADDR_TON_UNKNOWN                           	0x00                                                 	
#define SMS_TP_ADDR_TON_INTERNATIONAL                     	0x10                                              	
#define SMS_TP_ADDR_TON_NATIONAL                          	0x20                                              	
#define SMS_TP_ADDR_TON_NETW_SPECIFIC                     	0x30                                              	
#define SMS_TP_ADDR_TON_SUBSCRIBER                        	0x40                                              	
#define SMS_TP_ADDR_TON_ALPHANUMERIC                      	0x50                                              	
#define SMS_TP_ADDR_TON_ABBREVIATED                       	0x60                                              	
#define SMS_TP_ADDR_TON_RESERVED                          	0x70                                              	
#define SMS_RP_ADDR_TON_MASK                              	0x70                                              	
#define SMS_RP_ADDR_TON_UNKNOWN                           	0x00                                                 	
#define SMS_RP_ADDR_TON_INTERNATIONAL                     	0x10                                              	
#define SMS_RP_ADDR_TON_NATIONAL                          	0x20                                              	
#define SMS_RP_ADDR_TON_NETW_SPECIFIC                     	0x30                                              	
#define SMS_RP_ADDR_TON_DEDIC_ACCESS                      	0x40                                              	
#define SMS_RP_ADDR_TON_RESERVED                          	0x70                                              	
#define SMS_RP_ADDR_NPI_MASK                              	0x0F                                                 	
#define SMS_RP_ADDR_NPI_UNKNOWN                           	0x00                                                 	
#define SMS_RP_ADDR_NPI_ISDN                              	0x01                                                 	
#define SMS_RP_ADDR_NPI_DATA                              	0x03                                                 	
#define SMS_RP_ADDR_NPI_TELEX                             	0x04                                                 	
#define SMS_RP_ADDR_NPI_NATIONAL                          	0x08                                                 	
#define SMS_RP_ADDR_NPI_PRIVATE                           	0x09                                                 	
#define SMS_RP_ADDR_NPI_RES_CTS                           	0x0B                                                 	
#define SMS_RP_ADDR_NPI_RESERVED                          	0x0F                                                 	
#define SMS_TP_ADDR_NPI_MASK                              	0x0F                                                 	
#define SMS_TP_ADDR_NPI_UNKNOWN                           	0x00                                                 	
#define SMS_TP_ADDR_NPI_ISDN                              	0x01                                                 	
#define SMS_TP_ADDR_NPI_DATA                              	0x03                                                 	
#define SMS_TP_ADDR_NPI_TELEX                             	0x04                                                 	
#define SMS_TP_ADDR_NPI_SC_SPECIFIC1                      	0x05                                                 	
#define SMS_TP_ADDR_NPI_SC_SPECIFIC2                      	0x06                                                 	
#define SMS_TP_ADDR_NPI_NATIONAL                          	0x08                                                 	
#define SMS_TP_ADDR_NPI_PRIVATE                           	0x09                                                 	
#define SMS_TP_ADDR_NPI_ERMES                             	0x0A                                                 	
#define SMS_TP_ADDR_NPI_RESERVED                          	0x0F                                                 	
#define SMS_SRV_CB_MAX_SUBSCRIPTION_NR                    	0xFF                                              	
#define SMS_SRV_CB_MAX_CLIENT_AMOUNT                      	0x0B                                                 	
#define SMS_SRV_CB_MAX_CBMID_COUNT                        	0xC8                                              	
#define SMS_SRV_CB_MAX_LANGUAGE_COUNT                     	0x11                                              	
#define SMS_SRV_CB_DCS_ELEMENT_SIZE                       	0x01                                                 	
#define SMS_SRV_CB_CBMID_ELEMENT_SIZE                     	0x02                                                 	
#define SMS_CB_SUBSCRIPTION_ID_SIZE                       	0x01                                                 	
#define SMS_ANY_SIZE                                      	0x01                                                 	
#define SMS_CONCATENATED_UD_MAX_LEN                       	0x857A                                            	
//#define SMS_TP_VPF_RELATIVE                               	0x01                                                 	
//#define SMS_TP_VPF_ABSOLUTE_OR_ENHANCED                   	0x07                                                 	
#define SMS_PARAM_INDICATOR_MAX_LEN                       	0x0A                                                 	
#define SMS_SC_TIME_STAMP_SIZE                            	0x07                                                 	
#define SMS_DEFAULT_PARAMETER_LOCATION                    	0x00                                                 	
#define SMS_NEW_SUBSCRIPTION                              	0x00                                                 	

#define SMS_VPF_RELATIVE                                  	0x01                                                 	
#define SMS_VPF_ABSOLUTE_OR_ENHANCED                      	0x07                                                 	


//TABLES

//ConstantTable for SMS_CAPABILITY_ID

//ConstantTable for SMS_ROUTE

//ConstantTable for SMS_ROUTE
#define SMS_ROUTE_DEFAULT                                 	0x00                                                 	
#define SMS_ROUTE_CS                                      	0x01                                                 	
#define SMS_ROUTE_PS                                      	0x02                                                 	

//ConstantTable for SMS_SENDER

//ConstantTable for SMS_CAUSE_TYPE

//ConstantTable for SMS_CAUSE_TYPE
#define SMS_CAUSE_TYPE_COMMON                             	0x00                                                 	
#define SMS_CAUSE_TYPE_EXT                                	0x01                                                 	

//BitmaskTables for SMS_CAPABILITY_DATA
#define SMS_MT_RECEIVING                                  	0x01                                                 	
#define SMS_MO_SENDING                                    	0x02                                                 	
#define SMS_APPL_PORT_ROUTING                             	0x04                                                 	
#define SMS_CB_MESSAGE_RECEIVING                          	0x08                                                 	
#define SMS_APPL_PORT_COPY_NOTIFYING                      	0x10                                              	
#define SMS_GPRS_MESSAGE_PASSING                          	0x20                                              	
#define SMS_MWI_ROUTING_MODES                             	0x40                                              	
#define SMS_CLASS_ROUTING_MODES                           	0x80                                              	
#define SMS_DATACODE_ROUTING_MODES                        	0x100                                             	
#define SMS_MO_UNDEFINED_SENDING                          	0x200                                             	
#define SMS_TPDU_ROUTING                                  	0x400                                             	
#define SMS_MULTIMODE_SUPPORT                             	0x800                                             	
#define SMS_MO_SEGMENTATION_SUPPORT                       	0x1000                                            	

//ConstantTable for SMS_CAUSE
#define SMS_OK                                            	0x00                                                 	
#define SMS_ERR_ROUTING_RELEASED                          	0x01                                                 	
#define SMS_ERR_INVALID_PARAMETER                         	0x02                                                 	
#define SMS_ERR_DEVICE_FAILURE                            	0x03                                                 	
#define SMS_ERR_PP_RESERVED                               	0x04                                                 	
#define SMS_ERR_ROUTE_NOT_AVAILABLE                       	0x05                                                 	
#define SMS_ERR_ROUTE_NOT_ALLOWED                         	0x06                                                 	
#define SMS_ERR_SERVICE_RESERVED                          	0x07                                                 	
#define SMS_ERR_INVALID_LOCATION                          	0x08                                                 	
#define SMS_ERR_NO_NETW_RESPONSE                          	0x0B                                                 	
#define SMS_ERR_DEST_ADDR_FDN_RESTRICTED                  	0x0C                                                 	
#define SMS_ERR_SMSC_ADDR_FDN_RESTRICTED                  	0x0D                                                 	
#define SMS_ERR_RESEND_ALREADY_DONE                       	0x0E                                                 	
#define SMS_ERR_SMSC_ADDR_NOT_AVAILABLE                   	0x0F                                                 	
#define SMS_ERR_ROUTING_FAILED                            	0x10                                              	
#define SMS_ERR_CS_INACTIVE                               	0x11                                              	
#define SMS_ERR_SENDING_ONGOING                           	0x15                                              	
#define SMS_ERR_SERVER_NOT_READY                          	0x16                                              	
#define SMS_ERR_NO_TRANSACTION                            	0x17                                              	
#define SMS_ERR_INVALID_SUBSCRIPTION_NR                   	0x19                                              	
#define SMS_ERR_RECEPTION_FAILED                          	0x1A                                              	
#define SMS_ERR_RC_REJECTED                               	0x1B                                              	
#define SMS_ERR_ALL_SUBSCRIPTIONS_ALLOCATED               	0x1C                                              	
#define SMS_ERR_SUBJECT_COUNT_OVERFLOW                    	0x1D                                              	
#define SMS_ERR_DCS_COUNT_OVERFLOW                        	0x1E                                              	

//ConstantTable for SMS_GSM_CAUSE

//ConstantTable for SMS_EXT_CAUSE
#define SMS_EXT_ERR_UNASSIGNED_NUMBER                     	0x01                                                 	
#define SMS_EXT_ERR_OPER_DETERMINED_BARR                  	0x08                                                 	
#define SMS_EXT_ERR_CALL_BARRED                           	0x0A                                                 	
#define SMS_EXT_ERR_RESERVED                              	0x0B                                                 	
#define SMS_EXT_ERR_MSG_TRANSFER_REJ                      	0x15                                              	
#define SMS_EXT_ERR_MEMORY_CAPACITY_EXC                   	0x16                                              	
#define SMS_EXT_ERR_DEST_OUT_OF_ORDER                     	0x1B                                              	
#define SMS_EXT_ERR_UNDEFINED_SUBSCRIBER                  	0x1C                                              	
#define SMS_EXT_ERR_FACILITY_REJECTED                     	0x1D                                              	
#define SMS_EXT_ERR_UNKNOWN_SUBSCRIBER                    	0x1E                                              	
#define SMS_EXT_ERR_NETW_OUT_OF_ORDER                     	0x26                                              	
#define SMS_EXT_ERR_TEMPORARY_FAILURE                     	0x29                                              	
#define SMS_EXT_ERR_CONGESTION                            	0x2A                                              	
#define SMS_EXT_ERR_RESOURCE_UNAVAILABLE                  	0x2F                                              	
#define SMS_EXT_ERR_REQ_FACILITY_NOT_SUB                  	0x32                                              	
#define SMS_EXT_ERR_REQ_FACILITY_NOT_IMP                  	0x45                                              	
#define SMS_EXT_ERR_INVALID_REFERENCE                     	0x51                                              	
#define SMS_EXT_ERR_INCORRECT_MESSAGE                     	0x5F                                              	
#define SMS_EXT_ERR_INVALID_MAND_INFO                     	0x60                                              	
#define SMS_EXT_ERR_INVALID_MSG_TYPE                      	0x61                                              	
#define SMS_EXT_ERR_MSG_NOT_COMP_WITH_ST                  	0x62                                              	
#define SMS_EXT_ERR_INVALID_INFO_ELEMENT                  	0x63                                              	
#define SMS_EXT_ERR_PROTOCOL_ERROR                        	0x6F                                              	
#define SMS_EXT_ERR_INTERWORKING                          	0x7F                                              	
#define SMS_EXT_ERR_NO_CAUSE                              	0x80                                              	
#define SMS_EXT_ERR_IMSI_UNKNOWN_HLR                      	0x82                                              	
#define SMS_EXT_ERR_ILLEGAL_MS                            	0x83                                              	
#define SMS_EXT_ERR_IMSI_UNKNOWN_VLR                      	0x84                                              	
#define SMS_EXT_ERR_IMEI_NOT_ACCEPTED                     	0x85                                              	
#define SMS_EXT_ERR_ILLEGAL_ME                            	0x86                                              	
#define SMS_EXT_ERR_PLMN_NOT_ALLOWED                      	0x8B                                              	
#define SMS_EXT_ERR_LA_NOT_ALLOWED                        	0x8C                                              	
#define SMS_EXT_ERR_ROAM_NOT_ALLOWED_LA                   	0x8D                                              	
#define SMS_EXT_ERR_NO_SUITABLE_CELLS_LA                  	0x8F                                              	
#define SMS_EXT_ERR_NETWORK_FAILURE                       	0x91                                              	
#define SMS_EXT_ERR_MAC_FAILURE                           	0x94                                              	
#define SMS_EXT_ERR_SYNC_FAILURE                          	0x95                                              	
#define SMS_EXT_ERR_LOW_LAYER_CONGESTION                  	0x96                                              	
#define SMS_EXT_ERR_AUTH_UNACCEPTABLE                     	0x97                                              	
#define SMS_EXT_ERR_SERV_OPT_NOT_SUPPORTED                	0xA0                                              	
#define SMS_EXT_ERR_SERV_OPT_NOT_SUBSCRIBED               	0xA1                                              	
#define SMS_EXT_ERR_SERV_OPT_TEMP_OUT_OF_ORDER            	0xA2                                              	
#define SMS_EXT_ERR_CALL_CANNOT_BE_IDENTIFIED             	0xA6                                              	
#define SMS_EXT_ERR_SEMANTICALLY_INCORR_MSG               	0xDF                                              	
#define SMS_EXT_ERR_LOW_LAYER_INVALID_MAND_INFO           	0xE0                                              	
#define SMS_EXT_ERR_LOW_LAYER_INVALID_MSG_TYPE            	0xE1                                              	
#define SMS_EXT_ERR_LOW_LAYER_MSG_TYPE_NOT_COMP_WITH_ST   	0xE2                                              	
#define SMS_EXT_ERR_LOW_LAYER_INVALID_INFO_ELEMENT        	0xE3                                              	
#define SMS_EXT_ERR_CONDITIONAL_IE_ERROR                  	0xE4                                              	
#define SMS_EXT_ERR_LOW_LAYER_MSG_NOT_COMP_WITH_ST        	0xE5                                              	
#define SMS_EXT_ERR_CS_BARRED                             	0xE8                                              	
#define SMS_EXT_ERR_LOW_LAYER_PROTOCOL_ERROR              	0xEF                                              	

//ConstantTable for SMS_MESSAGE_TYPE

//ConstantTable for SMS_ROUTING_TYPE

//ConstantTable for SMS_ROUTING_COMMAND
#define SMS_ROUTING_RELEASE                               	0x00                                                 	
#define SMS_ROUTING_SET                                   	0x01                                                 	
#define SMS_ROUTING_QUERY                                 	0x05                                                 	
#define SMS_ROUTING_QUERY_ALL                             	0x06                                                 	

//ConstantTable for SMS_GSM_SETTING_TYPE

//ConstantTable for SMS_SETTING_TYPE
#define SMS_SETTING_TYPE_SMS                              	0x00                                                 	
#define SMS_SETTING_TYPE_RESEND                           	0x01                                                 	
#define SMS_SETTING_TYPE_ROUTE                            	0x02                                                 	
#define SMS_SETTING_TYPE_EFSMSP                           	0x03                                                 	
#define SMS_SETTING_TYPE_FDN                              	0x05                                                 	

//ConstantTable for SMS_ADDRESS_TYPE

//ConstantTable for SMS_ADDRESS_TYPE
#define SMS_UNICODE_ADDRESS                               	0x00                                                 	
#define SMS_DESTINATION_ADDRESS                           	0x01                                                 	
#define SMS_SMSC_ADDRESS                                  	0x02                                                 	

//ConstantTable for SMS_APPL_PORT_ADDRESS_TYPE

//ConstantTable for SMS_ALPHA_TAG_TYPE

//ConstantTable for SMS_STORAGE_STATUS

//ConstantTable for SMS_SEND_STATUS
#define SMS_MSG_REROUTED                                  	0x00                                                 	
#define SMS_MSG_REPEATED                                  	0x01                                                 	
#define SMS_MSG_WAITING_NETWORK                           	0x02                                                 	
#define SMS_MSG_IDLE                                      	0x03                                                 	

//ConstantTable for SMS_PACKING
#define SMS_PACKING_NOT_REQUIRED                          	0x00                                                 	
#define SMS_PACKING_REQUIRED                              	0x01                                                 	

//ConstantTable for SMS_MO_SM_RESULT
#define SMS_MO_RESULT_OK                                  	0x00                                                 	
#define SMS_MO_RESULT_FAIL                                	0x01                                                 	

//ConstantTable for SMS_GSM_ROUTING_MODE

//ConstantTable for SMS_GSM_CB_SUBJECT_LIST_TYPE

//ConstantTable for SMS_GSM_CB_LANGUAGES
#define SMS_CB_GERMAN                                     	0x00                                                 	
#define SMS_CB_ENGLISH                                    	0x01                                                 	
#define SMS_CB_ITALIAN                                    	0x02                                                 	
#define SMS_CB_FRENCH                                     	0x03                                                 	
#define SMS_CB_SPANISH                                    	0x04                                                 	
#define SMS_CB_DUTCH                                      	0x05                                                 	
#define SMS_CB_SWEDISH                                    	0x06                                                 	
#define SMS_CB_DANISH                                     	0x07                                                 	
#define SMS_CB_PORTUGUESE                                 	0x08                                                 	
#define SMS_CB_FINNISH                                    	0x09                                                 	
#define SMS_CB_NORWEGIAN                                  	0x0A                                                 	
#define SMS_CB_GREEK                                      	0x0B                                                 	
#define SMS_CB_TURKISH                                    	0x0C                                                 	
#define SMS_CB_HUNGARIAN                                  	0x0D                                                 	
#define SMS_CB_POLISH                                     	0x0E                                                 	
#define SMS_CB_UNSPECIFIED                                	0x0F                                                 	
#define SMS_CB_CZECH                                      	0x20                                              	
#define SMS_CB_HEBREW                                     	0x21                                              	
#define SMS_CB_ARABIC                                     	0x22                                              	
#define SMS_CB_RUSSIAN                                    	0x23                                              	
#define SMS_CB_ICELANDIC                                  	0x24                                              	

//ConstantTable for SMS_SM_CONTROL_ACTION

//ConstantTable for SMS_CONTENT_TYPE

//ConstantTable for SMS_VALIDITY_PERIOD_FORMAT

//ConstantTable for SMS_SERVER_STATUS
#define SMS_SERVER_READY                                  	0x00                                                 	
#define SMS_SERVER_NOT_READY                              	0x01                                                 	

//ConstantTable for SMS_SERVER_STATE
#define SMS_SERVER_IDLE                                   	0x00                                                 	
#define SMS_SERVER_PARTIAL_MODE                           	0x01                                                 	
#define SMS_SERVER_CS_INACTIVE                            	0x02                                                 	

//ConstantTable for SMS_ROUTE_PRIORITY
#define SMS_ROUTE_NOT_AVAILABLE                           	0x00                                                 	
#define SMS_ROUTE_PRIORITY_1                              	0x01                                                 	
#define SMS_ROUTE_PRIORITY_2                              	0x02                                                 	

//ConstantTable for SMS_SUBSCRIPTION_TYPE
#define SMS_TYPE_DEFAULT                                  	0x00                                                 	
#define SMS_TYPE_SIM                                      	0x01                                                 	

//BitmaskTables for SMS_CHECK_INFO
#define SMS_CHECK_DISABLE_NONE                            	0x00                                                 	
#define SMS_CHECK_DISABLE_FDN                             	0x01                                                 	

//ConstantTable for SMS_RECEPTION_COMMAND
#define SMS_RECEPTION_QUERY_STATUS                        	0x00                                                 	
#define SMS_RECEPTION_ACTIVATE                            	0x01                                                 	
#define SMS_RECEPTION_DEACTIVATE                          	0x02                                                 	
#define SMS_RECEPTION_STORAGE_STATUS_UPDATE               	0x03                                                 	

//ConstantTable for SMS_RECEPTION_STATUS
#define SMS_RECEPTION_ACTIVE                              	0x01                                                 	
#define SMS_RECEPTION_INACTIVE                            	0x02                                                 	

//ConstantTable for SMS_RESEND_SETTING
#define SMS_RESEND_DISABLED                               	0x00                                                 	
#define SMS_RESEND_ONCE                                   	0x01                                                 	
#define SMS_RESEND_TWICE                                  	0x02                                                 	
#define SMS_RESEND_THRICE                                 	0x03                                                 	

//ConstantTable for SMS_FDN_SETTING
#define SMS_FDN_CHECK_DISABLED                            	0x00                                                 	
#define SMS_FDN_CHECK_ENABLED                             	0x01                                                 	

//ConstantTable for SMS_RES_CONF_STATUS
#define SMS_RES_CONF_STARTUP                              	0x01                                                 	
#define SMS_RES_CONF_RECONFIGURED                         	0x02                                                 	

//ConstantTable for SMS_RES_CONF_OPERATION
#define SMS_RES_CONF_GET                                  	0x01                                                 	
#define SMS_RES_CONF_SET                                  	0x02                                                 	

//BitmaskTables for SMS_RESOURCE_IDS
#define SMS_RES_ID_MO_SM_INIT                             	0x01                                                 	

//BitmaskTables for SMS_RESOURCE_IDS_MASK
#define SMS_RES_ID_MASK_MO_SM_INIT                        	0x01                                                 	

//BitmaskTables for SMS_RESOURCE_IDS_CONF_REQ
#define SMS_RES_ID_CONF_MO_SM_INIT                        	0x01                                                 	

//ConstantTable for SMS_RESOURCE_STATUS
#define SMS_RESOURCE_ALLOWED                              	0x01                                                 	
#define SMS_RESOURCE_DENIED                               	0x02                                                 	

//BitmaskTables for SMS_PARAMETER_INDICATOR
#define SMS_PI_DESTINATION_ADDRESS                        	0x01                                                 	
#define SMS_PI_SERVICE_CENTRE_ADDRESS                     	0x02                                                 	
#define SMS_PI_PROTOCOL_ID                                	0x04                                                 	
#define SMS_PI_DATA_CODING_SCHEME                         	0x08                                                 	
#define SMS_PI_VALIDITY_PERIOD                            	0x10                                              	

//ConstantTable for SMS_CB_SUBSCRIPTION_STATUS

//ConstantTable for SMS_CB_SUBSCR_STATUS
#define SMS_CB_SBS_ALL_SUBSCRIBED                         	0x00                                                 	
#define SMS_CB_SBS_PARTIALLY_SUBSCRIBED                   	0x01                                                 	
#define SMS_CB_SBS_NONE_SUBSCRIBED                        	0x02                                                 	


//SUBBLOCKS


//Definition for SMS_SB_ADDRESS
#define SMS_SB_ADDRESS                                    	0x82                                              	
#define SMS_SB_ADDRESS_OFFSET_SUBBLOCKID                  	0 //size 2 byte(s)
#define SMS_SB_ADDRESS_OFFSET_SUBBLOCKLENGTH              	2 //size 2 byte(s)
#define SMS_SB_ADDRESS_OFFSET_ADDRESSTYPE                 	4 //size 1 byte(s)
#define SMS_SB_ADDRESS_OFFSET_ADDRESSDATALENGTH           	5 //size 1 byte(s)
#define SMS_SB_ADDRESS_OFFSET_ADDRESSDATA                 	6 //size 1 byte(s)
#define SMS_SB_ADDRESS_OFFSET_FILLERBYTE1                 	7 //size 1 byte(s)
#define SIZE_SMS_SB_ADDRESS                               	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_USER_DATA
#define SMS_SB_USER_DATA                                  	0x20                                              	
#define SMS_SB_USER_DATA_OFFSET_SUBBLOCKID                	0 //size 2 byte(s)
#define SMS_SB_USER_DATA_OFFSET_SUBBLOCKLENGTH            	2 //size 2 byte(s)
#define SMS_SB_USER_DATA_OFFSET_DATALENGTH                	4 //size 2 byte(s)
#define SMS_SB_USER_DATA_OFFSET_CHARACTERAMOUNT           	6 //size 2 byte(s)
#define SMS_SB_USER_DATA_OFFSET_DATABYTES                 	8 //size 1 byte(s)
#define SMS_SB_USER_DATA_OFFSET_FILLERBYTE1               	9 //size 1 byte(s)
#define SMS_SB_USER_DATA_OFFSET_FILLERBYTE2               	10 //size 1 byte(s)
#define SMS_SB_USER_DATA_OFFSET_FILLERBYTE3               	11 //size 1 byte(s)
#define SIZE_SMS_SB_USER_DATA                             	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_VALIDITY_PERIOD
#define SMS_SB_VALIDITY_PERIOD                            	0x08                                                 	
#define SMS_SB_VALIDITY_PERIOD_OFFSET_SUBBLOCKID          	0 //size 2 byte(s)
#define SMS_SB_VALIDITY_PERIOD_OFFSET_SUBBLOCKLENGTH      	2 //size 2 byte(s)
#define SMS_SB_VALIDITY_PERIOD_OFFSET_VALIDITYPERIODLENGTH	4 //size 1 byte(s)
#define SMS_SB_VALIDITY_PERIOD_OFFSET_VALIDITYPERIODDATA  	5 //size 1 byte(s)
#define SMS_SB_VALIDITY_PERIOD_OFFSET_FILLERBYTE1         	6 //size 1 byte(s)
#define SMS_SB_VALIDITY_PERIOD_OFFSET_FILLERBYTE2         	7 //size 1 byte(s)
#define SIZE_SMS_SB_VALIDITY_PERIOD                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_PARAM_INDICATOR
#define SMS_SB_PARAM_INDICATOR                            	0x1D                                              	
#define SMS_SB_PARAM_INDICATOR_OFFSET_SUBBLOCKID          	0 //size 2 byte(s)
#define SMS_SB_PARAM_INDICATOR_OFFSET_SUBBLOCKLENGTH      	2 //size 2 byte(s)
#define SMS_SB_PARAM_INDICATOR_OFFSET_PROTOCOLID          	4 //size 1 byte(s)
#define SMS_SB_PARAM_INDICATOR_OFFSET_DATACODINGSCHEME    	5 //size 1 byte(s)
#define SMS_SB_PARAM_INDICATOR_OFFSET_PARAMETERINDICATORLENGTH	6 //size 1 byte(s)
#define SMS_SB_PARAM_INDICATOR_OFFSET_PARAMETERINDICATORS 	7 //size 1 byte(s)
#define SIZE_SMS_SB_PARAM_INDICATOR                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_SUBMIT
#define SMS_SB_SUBMIT                                     	0x16                                              	
#define SMS_SB_SUBMIT_OFFSET_SUBBLOCKID                   	0 //size 2 byte(s)
#define SMS_SB_SUBMIT_OFFSET_SUBBLOCKLENGTH               	2 //size 2 byte(s)
#define SMS_SB_SUBMIT_OFFSET_MESSAGEPARAMETERS            	4 //size 1 byte(s)
#define SMS_SB_SUBMIT_OFFSET_MESSAGEREFERENCE             	5 //size 1 byte(s)
#define SMS_SB_SUBMIT_OFFSET_PROTOCOLID                   	6 //size 1 byte(s)
#define SMS_SB_SUBMIT_OFFSET_DATACODINGSCHEME             	7 //size 1 byte(s)
#define SIZE_SMS_SB_SUBMIT                                	8


//Definition for SMS_SB_COMMAND
#define SMS_SB_COMMAND                                    	0x15                                              	
#define SMS_SB_COMMAND_OFFSET_SUBBLOCKID                  	0 //size 2 byte(s)
#define SMS_SB_COMMAND_OFFSET_SUBBLOCKLENGTH              	2 //size 2 byte(s)
#define SMS_SB_COMMAND_OFFSET_MESSAGEPARAMETERS           	4 //size 1 byte(s)
#define SMS_SB_COMMAND_OFFSET_MESSAGEREFERENCE            	5 //size 1 byte(s)
#define SMS_SB_COMMAND_OFFSET_PROTOCOLID                  	6 //size 1 byte(s)
#define SMS_SB_COMMAND_OFFSET_COMMANDTYPE                 	7 //size 1 byte(s)
#define SMS_SB_COMMAND_OFFSET_MESSAGENUMBER               	8 //size 1 byte(s)
#define SMS_SB_COMMAND_OFFSET_FILLERBYTE1                 	9 //size 3 byte(s)
#define SIZE_SMS_SB_COMMAND                               	12


//Definition for SMS_SB_DELIVER_REPORT
#define SMS_SB_DELIVER_REPORT                             	0x19                                              	
#define SMS_SB_DELIVER_REPORT_OFFSET_SUBBLOCKID           	0 //size 2 byte(s)
#define SMS_SB_DELIVER_REPORT_OFFSET_SUBBLOCKLENGTH       	2 //size 2 byte(s)
#define SMS_SB_DELIVER_REPORT_OFFSET_MESSAGEPARAMETERS    	4 //size 1 byte(s)
#define SMS_SB_DELIVER_REPORT_OFFSET_GSMTPFAILURECAUSE    	5 //size 1 byte(s)
#define SMS_SB_DELIVER_REPORT_OFFSET_FILLERBYTE1          	6 //size 2 byte(s)
#define SIZE_SMS_SB_DELIVER_REPORT                        	8


//Definition for SMS_SB_SUBMIT_REPORT
#define SMS_SB_SUBMIT_REPORT                              	0x1A                                              	
#define SMS_SB_SUBMIT_REPORT_OFFSET_SUBBLOCKID            	0 //size 2 byte(s)
#define SMS_SB_SUBMIT_REPORT_OFFSET_SUBBLOCKLENGTH        	2 //size 2 byte(s)
#define SMS_SB_SUBMIT_REPORT_OFFSET_MESSAGEPARAMETERS     	4 //size 1 byte(s)
#define SMS_SB_SUBMIT_REPORT_OFFSET_SCTIMESTAMP           	5 //size 7 byte(s)
#define SMS_SB_SUBMIT_REPORT_OFFSET_TPFAILURECAUSE        	12 //size 1 byte(s)
#define SMS_SB_SUBMIT_REPORT_OFFSET_FILLERBYTE1           	13 //size 3 byte(s)
#define SIZE_SMS_SB_SUBMIT_REPORT                         	16
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_CB_MESSAGE
#define SMS_SB_CB_MESSAGE                                 	0x0E                                                 	
#define SMS_SB_CB_MESSAGE_OFFSET_SUBBLOCKID               	0 //size 2 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_SUBBLOCKLENGTH           	2 //size 2 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_SERIALNUMBER             	4 //size 2 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_CBMESSAGEID              	6 //size 2 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_DATACODINGSCHEME         	8 //size 1 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_PAGE                     	9 //size 1 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_INFOLENGTH               	10 //size 1 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_CONTENTOFMESSAGE         	11 //size 82 byte(s)
#define SMS_SB_CB_MESSAGE_OFFSET_FILLERBYTE1              	93 //size 3 byte(s)
#define SIZE_SMS_SB_CB_MESSAGE                            	96
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_RESEND_SETTING
#define SMS_SB_RESEND_SETTING                             	0x14                                              	
#define SMS_SB_RESEND_SETTING_OFFSET_SUBBLOCKID           	0 //size 2 byte(s)
#define SMS_SB_RESEND_SETTING_OFFSET_SUBBLOCKLENGTH       	2 //size 2 byte(s)
#define SMS_SB_RESEND_SETTING_OFFSET_RESENDSETTING        	4 //size 1 byte(s)
#define SMS_SB_RESEND_SETTING_OFFSET_FILLERBYTE1          	5 //size 3 byte(s)
#define SIZE_SMS_SB_RESEND_SETTING                        	8


//Definition for SMS_SB_FDN_SETTING
#define SMS_SB_FDN_SETTING                                	0x34                                              	
#define SMS_SB_FDN_SETTING_OFFSET_SUBBLOCKID              	0 //size 2 byte(s)
#define SMS_SB_FDN_SETTING_OFFSET_SUBBLOCKLENGTH          	2 //size 2 byte(s)
#define SMS_SB_FDN_SETTING_OFFSET_FDNSETTING              	4 //size 1 byte(s)
#define SMS_SB_FDN_SETTING_OFFSET_FILLERBYTE1             	5 //size 3 byte(s)
#define SIZE_SMS_SB_FDN_SETTING                           	8


//Definition for SMS_SB_TPDU_PACKING
#define SMS_SB_TPDU_PACKING                               	0x33                                              	
#define SMS_SB_TPDU_PACKING_OFFSET_SUBBLOCKID             	0 //size 2 byte(s)
#define SMS_SB_TPDU_PACKING_OFFSET_SUBBLOCKLENGTH         	2 //size 2 byte(s)
#define SMS_SB_TPDU_PACKING_OFFSET_PACKINGREQUIRED        	4 //size 1 byte(s)
#define SMS_SB_TPDU_PACKING_OFFSET_FILLERBYTE1            	5 //size 3 byte(s)
#define SIZE_SMS_SB_TPDU_PACKING                          	8


//Definition for SMS_SB_MO_SM_RESULT
#define SMS_SB_MO_SM_RESULT                               	0x35                                              	
#define SMS_SB_MO_SM_RESULT_OFFSET_SUBBLOCKID             	0 //size 2 byte(s)
#define SMS_SB_MO_SM_RESULT_OFFSET_SUBBLOCKLENGTH         	2 //size 2 byte(s)
#define SMS_SB_MO_SM_RESULT_OFFSET_SMSMOSMRESULT          	4 //size 1 byte(s)
#define SMS_SB_MO_SM_RESULT_OFFSET_FILLERBYTE1            	5 //size 3 byte(s)
#define SIZE_SMS_SB_MO_SM_RESULT                          	8


//Definition for SMS_SB_TPDU
#define SMS_SB_TPDU                                       	0x1C                                              	
#define SMS_SB_TPDU_OFFSET_SUBBLOCKID                     	0 //size 2 byte(s)
#define SMS_SB_TPDU_OFFSET_SUBBLOCKLENGTH                 	2 //size 2 byte(s)
#define SMS_SB_TPDU_OFFSET_DATALENGTH                     	4 //size 1 byte(s)
#define SMS_SB_TPDU_OFFSET_FILLERBYTE1                    	5 //size 1 byte(s)
#define SMS_SB_TPDU_OFFSET_DATABYTES                      	6 //size 1 byte(s)
#define SMS_SB_TPDU_OFFSET_FILLERBYTE2                    	7 //size 1 byte(s)
#define SIZE_SMS_SB_TPDU                                  	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_CB_DCS_LIST
#define SMS_SB_CB_DCS_LIST                                	0x25                                              	
#define SMS_SB_CB_DCS_LIST_OFFSET_SUBBLOCKID              	0 //size 2 byte(s)
#define SMS_SB_CB_DCS_LIST_OFFSET_SUBBLOCKLENGTH          	2 //size 2 byte(s)
#define SMS_SB_CB_DCS_LIST_OFFSET_FILLERBYTE1             	4 //size 1 byte(s)
#define SMS_SB_CB_DCS_LIST_OFFSET_DCSCOUNT                	5 //size 1 byte(s)
#define SMS_SB_CB_DCS_LIST_OFFSET_DCSVALUE                	6 //size 1 byte(s)
#define SMS_SB_CB_DCS_LIST_OFFSET_FILLERBYTE2             	7 //size 1 byte(s)
#define SIZE_SMS_SB_CB_DCS_LIST                           	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_CB_SUBJECTS
#define SMS_SB_CB_SUBJECTS                                	0x26                                              	
#define SMS_SB_CB_SUBJECTS_OFFSET_SUBBLOCKID              	0 //size 2 byte(s)
#define SMS_SB_CB_SUBJECTS_OFFSET_SUBBLOCKLENGTH          	2 //size 2 byte(s)
#define SMS_SB_CB_SUBJECTS_OFFSET_FILLERBYTE1             	4 //size 1 byte(s)
#define SMS_SB_CB_SUBJECTS_OFFSET_SUBJECTCOUNT            	5 //size 1 byte(s)
#define SMS_SB_CB_SUBJECTS_OFFSET_SUBJECTLISTIDS          	6 //size 2 byte(s)
#define SIZE_SMS_SB_CB_SUBJECTS                           	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_SETTINGS
#define SMS_SB_SETTINGS                                   	0x21                                              	
#define SMS_SB_SETTINGS_OFFSET_SUBBLOCKID                 	0 //size 2 byte(s)
#define SMS_SB_SETTINGS_OFFSET_SUBBLOCKLENGTH             	2 //size 2 byte(s)
#define SMS_SB_SETTINGS_OFFSET_SETTINGSINDICATOR          	4 //size 1 byte(s)
#define SMS_SB_SETTINGS_OFFSET_REPLYPATHREQUEST           	5 //size 1 byte(s)
#define SMS_SB_SETTINGS_OFFSET_CONFIRMATIONREQUEST        	6 //size 1 byte(s)
#define SMS_SB_SETTINGS_OFFSET_FILLERBYTE1                	7 //size 1 byte(s)
#define SIZE_SMS_SB_SETTINGS                              	8


//Definition for SMS_SB_SERVER_STATE_INFO
#define SMS_SB_SERVER_STATE_INFO                          	0x22                                              	
#define SMS_SB_SERVER_STATE_INFO_OFFSET_SUBBLOCKID        	0 //size 2 byte(s)
#define SMS_SB_SERVER_STATE_INFO_OFFSET_SUBBLOCKLENGTH    	2 //size 2 byte(s)
#define SMS_SB_SERVER_STATE_INFO_OFFSET_SERVERSTATE       	4 //size 1 byte(s)
#define SMS_SB_SERVER_STATE_INFO_OFFSET_FILLERBYTE1       	5 //size 3 byte(s)
#define SIZE_SMS_SB_SERVER_STATE_INFO                     	8


//Definition for SMS_SB_ROUTE_INFO
#define SMS_SB_ROUTE_INFO                                 	0x23                                              	
#define SMS_SB_ROUTE_INFO_OFFSET_SUBBLOCKID               	0 //size 2 byte(s)
#define SMS_SB_ROUTE_INFO_OFFSET_SUBBLOCKLENGTH           	2 //size 2 byte(s)
#define SMS_SB_ROUTE_INFO_OFFSET_CSROUTEINFO              	4 //size 1 byte(s)
#define SMS_SB_ROUTE_INFO_OFFSET_PSROUTEINFO              	5 //size 1 byte(s)
#define SMS_SB_ROUTE_INFO_OFFSET_FILLERBYTE1              	6 //size 2 byte(s)
#define SIZE_SMS_SB_ROUTE_INFO                            	8


//Definition for SMS_SB_CHECK_INFO
#define SMS_SB_CHECK_INFO                                 	0x87                                              	
#define SMS_SB_CHECK_INFO_OFFSET_SUBBLOCKID               	0 //size 2 byte(s)
#define SMS_SB_CHECK_INFO_OFFSET_SUBBLOCKLENGTH           	2 //size 2 byte(s)
#define SMS_SB_CHECK_INFO_OFFSET_INTCHECKINFO             	4 //size 1 byte(s)
#define SMS_SB_CHECK_INFO_OFFSET_FILLERBYTE1              	5 //size 3 byte(s)
#define SIZE_SMS_SB_CHECK_INFO                            	8


//Definition for SMS_SB_RECEPTION_STATUS
#define SMS_SB_RECEPTION_STATUS                           	0x2C                                              	
#define SMS_SB_RECEPTION_STATUS_OFFSET_SUBBLOCKID         	0 //size 2 byte(s)
#define SMS_SB_RECEPTION_STATUS_OFFSET_SUBBLOCKLENGTH     	2 //size 2 byte(s)
#define SMS_SB_RECEPTION_STATUS_OFFSET_STATUS             	4 //size 1 byte(s)
#define SMS_SB_RECEPTION_STATUS_OFFSET_FILLERBYTE1        	5 //size 3 byte(s)
#define SIZE_SMS_SB_RECEPTION_STATUS                      	8


//Definition for SMS_SB_CBS_SUBSCRIPTION
#define SMS_SB_CBS_SUBSCRIPTION                           	0x2D                                              	
#define SMS_SB_CBS_SUBSCRIPTION_OFFSET_SUBBLOCKID         	0 //size 2 byte(s)
#define SMS_SB_CBS_SUBSCRIPTION_OFFSET_SUBBLOCKLENGTH     	2 //size 2 byte(s)
#define SMS_SB_CBS_SUBSCRIPTION_OFFSET_CBSSUBSCRIPTIONCLIENTCOUNT	4 //size 1 byte(s)
#define SMS_SB_CBS_SUBSCRIPTION_OFFSET_CBSSUBSCRIBERID    	5 //size 1 byte(s)
#define SMS_SB_CBS_SUBSCRIPTION_OFFSET_FILLERBYTE1        	6 //size 1 byte(s)
#define SMS_SB_CBS_SUBSCRIPTION_OFFSET_FILLERBYTE2        	7 //size 1 byte(s)
#define SIZE_SMS_SB_CBS_SUBSCRIPTION                      	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SMS_SB_RESOURCE_CONF
#define SMS_SB_RESOURCE_CONF                              	0x2B                                              	
#define SMS_SB_RESOURCE_CONF_OFFSET_SUBBLOCKID            	0 //size 2 byte(s)
#define SMS_SB_RESOURCE_CONF_OFFSET_SUBBLOCKLENGTH        	2 //size 2 byte(s)
#define SMS_SB_RESOURCE_CONF_OFFSET_RESOURCES             	4 //size 2 byte(s)
#define SMS_SB_RESOURCE_CONF_OFFSET_RESOURCEMASK          	6 //size 2 byte(s)
#define SIZE_SMS_SB_RESOURCE_CONF                         	8


//Definition for SMS_SB_RESOURCE
#define SMS_SB_RESOURCE                                   	0x2A                                              	
#define SMS_SB_RESOURCE_OFFSET_SUBBLOCKID                 	0 //size 2 byte(s)
#define SMS_SB_RESOURCE_OFFSET_SUBBLOCKLENGTH             	2 //size 2 byte(s)
#define SMS_SB_RESOURCE_OFFSET_RESOURCES                  	4 //size 2 byte(s)
#define SMS_SB_RESOURCE_OFFSET_FILLERBYTE1                	6 //size 2 byte(s)
#define SIZE_SMS_SB_RESOURCE                              	8


//Definition for SMS_SB_CAUSE
#define SMS_SB_CAUSE                                      	0x29                                              	
#define SMS_SB_CAUSE_OFFSET_SUBBLOCKID                    	0 //size 2 byte(s)
#define SMS_SB_CAUSE_OFFSET_SUBBLOCKLENGTH                	2 //size 2 byte(s)
#define SMS_SB_CAUSE_OFFSET_CAUSETYPE                     	4 //size 1 byte(s)
#define SMS_SB_CAUSE_OFFSET_SMSCAUSE                      	5 //size 1 byte(s)
#define SMS_SB_CAUSE_OFFSET_FILLERBYTE1                   	6 //size 2 byte(s)
#define SIZE_SMS_SB_CAUSE                                 	8


//Definition for SMS_SB_RESOURCE_CONF_REQUIRED
#define SMS_SB_RESOURCE_CONF_REQUIRED                     	0x28                                              	
#define SMS_SB_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKID   	0 //size 2 byte(s)
#define SMS_SB_RESOURCE_CONF_REQUIRED_OFFSET_SUBBLOCKLENGTH	2 //size 2 byte(s)
#define SMS_SB_RESOURCE_CONF_REQUIRED_OFFSET_RESPP        	4 //size 2 byte(s)
#define SMS_SB_RESOURCE_CONF_REQUIRED_OFFSET_FILLERBYTE1  	6 //size 2 byte(s)
#define SIZE_SMS_SB_RESOURCE_CONF_REQUIRED                	8


//Definition for SMS_SB_RESOURCE_SEQ_ID
#define SMS_SB_RESOURCE_SEQ_ID                            	0x2E                                              	
#define SMS_SB_RESOURCE_SEQ_ID_OFFSET_SUBBLOCKID          	0 //size 2 byte(s)
#define SMS_SB_RESOURCE_SEQ_ID_OFFSET_SUBBLOCKLENGTH      	2 //size 2 byte(s)
#define SMS_SB_RESOURCE_SEQ_ID_OFFSET_SEQUENCEID          	4 //size 1 byte(s)
#define SMS_SB_RESOURCE_SEQ_ID_OFFSET_FILLERBYTE1         	5 //size 3 byte(s)
#define SIZE_SMS_SB_RESOURCE_SEQ_ID                       	8


//Definition for SMS_SB_RESOURCE_STATUS
#define SMS_SB_RESOURCE_STATUS                            	0x2F                                              	
#define SMS_SB_RESOURCE_STATUS_OFFSET_SUBBLOCKID          	0 //size 2 byte(s)
#define SMS_SB_RESOURCE_STATUS_OFFSET_SUBBLOCKLENGTH      	2 //size 2 byte(s)
#define SMS_SB_RESOURCE_STATUS_OFFSET_RESSTATUS           	4 //size 1 byte(s)
#define SMS_SB_RESOURCE_STATUS_OFFSET_FILLERBYTE1         	5 //size 3 byte(s)
#define SIZE_SMS_SB_RESOURCE_STATUS                       	8


//Definition for SMS_SB_REQ_INFO
#define SMS_SB_REQ_INFO                                   	0x30                                              	
#define SMS_SB_REQ_INFO_OFFSET_SUBBLOCKID                 	0 //size 2 byte(s)
#define SMS_SB_REQ_INFO_OFFSET_SUBBLOCKLENGTH             	2 //size 2 byte(s)
#define SMS_SB_REQ_INFO_OFFSET_REQTRANSACTIONID           	4 //size 1 byte(s)
#define SMS_SB_REQ_INFO_OFFSET_FILLERBYTE1                	5 //size 3 byte(s)
#define SIZE_SMS_SB_REQ_INFO                              	8


//Definition for SMS_SB_SMS_PARAMETERS
#define SMS_SB_SMS_PARAMETERS                             	0x31                                              	
#define SMS_SB_SMS_PARAMETERS_OFFSET_SUBBLOCKID           	0 //size 2 byte(s)
#define SMS_SB_SMS_PARAMETERS_OFFSET_SUBBLOCKLENGTH       	2 //size 2 byte(s)
#define SMS_SB_SMS_PARAMETERS_OFFSET_LOCATIONNUMBER       	4 //size 1 byte(s)
#define SMS_SB_SMS_PARAMETERS_OFFSET_SMSPARAMETERS        	5 //size 1 byte(s)
#define SMS_SB_SMS_PARAMETERS_OFFSET_FILLERBYTE1          	6 //size 2 byte(s)
#define SIZE_SMS_SB_SMS_PARAMETERS                        	8


//MESSAGES


//Definition for SMS_MESSAGE_SEND_STATUS_IND
#define SMS_MESSAGE_SEND_STATUS_IND                       	0x22                                              	
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_TRANSID        	0 //size 1 byte(s)
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_STATUS         	2 //size 1 byte(s)
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_MESSAGEREFERENCE	3 //size 1 byte(s)
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_ROUTE          	4 //size 1 byte(s)
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_CURRENTSEGMENT 	5 //size 1 byte(s)
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_SEGMENTSTOTAL  	6 //size 1 byte(s)
#define SMS_MESSAGE_SEND_STATUS_IND_OFFSET_FILLERBYTE1    	7 //size 1 byte(s)
#define SIZE_SMS_MESSAGE_SEND_STATUS_IND                  	8


//Definition for SMS_MEMORY_CAPACITY_EXC_IND
#define SMS_MEMORY_CAPACITY_EXC_IND                       	0x1D                                              	
#define SMS_MEMORY_CAPACITY_EXC_IND_OFFSET_TRANSID        	0 //size 1 byte(s)
#define SMS_MEMORY_CAPACITY_EXC_IND_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define SMS_MEMORY_CAPACITY_EXC_IND_OFFSET_FILLERBYTE1    	2 //size 2 byte(s)
#define SIZE_SMS_MEMORY_CAPACITY_EXC_IND                  	4


//Definition for SMS_MESSAGE_SEND_REQ
#define SMS_MESSAGE_SEND_REQ                              	0x02                                                 	
#define SMS_MESSAGE_SEND_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define SMS_MESSAGE_SEND_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SMS_MESSAGE_SEND_REQ_OFFSET_MOREMESSAGESTOSEND    	2 //size 1 byte(s)
#define SMS_MESSAGE_SEND_REQ_OFFSET_SMSROUTE              	3 //size 1 byte(s)
#define SMS_MESSAGE_SEND_REQ_OFFSET_REPEATEDMESSAGE       	4 //size 1 byte(s)
#define SMS_MESSAGE_SEND_REQ_OFFSET_FILLERBYTE1           	5 //size 2 byte(s)
#define SMS_MESSAGE_SEND_REQ_OFFSET_SUBBLOCKCOUNT         	7 //size 1 byte(s)
#define SIZE_SMS_MESSAGE_SEND_REQ                         	8


//Definition for SMS_MESSAGE_SEND_RESP
#define SMS_MESSAGE_SEND_RESP                             	0x03                                                 	
#define SMS_MESSAGE_SEND_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define SMS_MESSAGE_SEND_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SMS_MESSAGE_SEND_RESP_OFFSET_CAUSETYPE            	2 //size 1 byte(s)
#define SMS_MESSAGE_SEND_RESP_OFFSET_SMSCAUSE             	3 //size 1 byte(s)
#define SMS_MESSAGE_SEND_RESP_OFFSET_REFERENCEFORMESSAGE  	4 //size 1 byte(s)
#define SMS_MESSAGE_SEND_RESP_OFFSET_FILLERBYTE1          	5 //size 2 byte(s)
#define SMS_MESSAGE_SEND_RESP_OFFSET_SUBBLOCKCOUNT        	7 //size 1 byte(s)
#define SIZE_SMS_MESSAGE_SEND_RESP                        	8


//Definition for SMS_RECEIVED_MSG_REPORT_REQ
#define SMS_RECEIVED_MSG_REPORT_REQ                       	0x3B                                              	
#define SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_TRANSID        	0 //size 1 byte(s)
#define SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_CAUSETYPE      	2 //size 1 byte(s)
#define SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_SMSCAUSE       	3 //size 1 byte(s)
#define SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_FILLERBYTE1    	4 //size 3 byte(s)
#define SMS_RECEIVED_MSG_REPORT_REQ_OFFSET_SUBBLOCKCOUNT  	7 //size 1 byte(s)
#define SIZE_SMS_RECEIVED_MSG_REPORT_REQ                  	8


//Definition for SMS_RECEIVED_MSG_REPORT_RESP
#define SMS_RECEIVED_MSG_REPORT_RESP                      	0x3C                                              	
#define SMS_RECEIVED_MSG_REPORT_RESP_OFFSET_TRANSID       	0 //size 1 byte(s)
#define SMS_RECEIVED_MSG_REPORT_RESP_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define SMS_RECEIVED_MSG_REPORT_RESP_OFFSET_SMSCAUSE      	2 //size 1 byte(s)
#define SMS_RECEIVED_MSG_REPORT_RESP_OFFSET_FILLERBYTE1   	3 //size 1 byte(s)
#define SIZE_SMS_RECEIVED_MSG_REPORT_RESP                 	4


//Definition for SMS_RECEIVE_MESSAGE_REQ
#define SMS_RECEIVE_MESSAGE_REQ                           	0x41                                              	
#define SMS_RECEIVE_MESSAGE_REQ_OFFSET_TRANSID            	0 //size 1 byte(s)
#define SMS_RECEIVE_MESSAGE_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define SMS_RECEIVE_MESSAGE_REQ_OFFSET_ACTION             	2 //size 1 byte(s)
#define SMS_RECEIVE_MESSAGE_REQ_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_SMS_RECEIVE_MESSAGE_REQ                      	4


//Definition for SMS_RECEIVE_MESSAGE_RESP
#define SMS_RECEIVE_MESSAGE_RESP                          	0x42                                              	
#define SMS_RECEIVE_MESSAGE_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define SMS_RECEIVE_MESSAGE_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define SMS_RECEIVE_MESSAGE_RESP_OFFSET_STATUS            	2 //size 1 byte(s)
#define SMS_RECEIVE_MESSAGE_RESP_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_SMS_RECEIVE_MESSAGE_RESP                     	4


//Definition for SMS_RECEIVED_MSG_IND
#define SMS_RECEIVED_MSG_IND                              	0x43                                              	
#define SMS_RECEIVED_MSG_IND_OFFSET_TRANSID               	0 //size 1 byte(s)
#define SMS_RECEIVED_MSG_IND_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SMS_RECEIVED_MSG_IND_OFFSET_FILLERBYTE1           	2 //size 1 byte(s)
#define SMS_RECEIVED_MSG_IND_OFFSET_SUBBLOCKCOUNT         	3 //size 1 byte(s)
#define SIZE_SMS_RECEIVED_MSG_IND                         	4


//Definition for SMS_RECEIVED_SIM_MSG_IND
#define SMS_RECEIVED_SIM_MSG_IND                          	0x44                                              	
#define SMS_RECEIVED_SIM_MSG_IND_OFFSET_TRANSID           	0 //size 1 byte(s)
#define SMS_RECEIVED_SIM_MSG_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define SMS_RECEIVED_SIM_MSG_IND_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define SMS_RECEIVED_SIM_MSG_IND_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_SMS_RECEIVED_SIM_MSG_IND                     	4


//Definition for SMS_CB_ROUTING_REQ
#define SMS_CB_ROUTING_REQ                                	0x34                                              	
#define SMS_CB_ROUTING_REQ_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define SMS_CB_ROUTING_REQ_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define SMS_CB_ROUTING_REQ_OFFSET_ROUTINGCOMMAND          	2 //size 1 byte(s)
#define SMS_CB_ROUTING_REQ_OFFSET_SUBSCRIPTIONNUMBER      	3 //size 1 byte(s)
#define SMS_CB_ROUTING_REQ_OFFSET_SUBSCRIPTIONTYPE        	4 //size 1 byte(s)
#define SMS_CB_ROUTING_REQ_OFFSET_FILLERBYTE1             	5 //size 2 byte(s)
#define SMS_CB_ROUTING_REQ_OFFSET_SUBBLOCKCOUNT           	7 //size 1 byte(s)
#define SIZE_SMS_CB_ROUTING_REQ                           	8


//Definition for SMS_CB_ROUTING_RESP
#define SMS_CB_ROUTING_RESP                               	0x35                                              	
#define SMS_CB_ROUTING_RESP_OFFSET_TRANSID                	0 //size 1 byte(s)
#define SMS_CB_ROUTING_RESP_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define SMS_CB_ROUTING_RESP_OFFSET_SUBSCRIPTIONNUMBER     	2 //size 1 byte(s)
#define SMS_CB_ROUTING_RESP_OFFSET_SMSCAUSE               	3 //size 1 byte(s)
#define SMS_CB_ROUTING_RESP_OFFSET_CBSUBSCRIPTIONSTATUS   	4 //size 1 byte(s)
#define SMS_CB_ROUTING_RESP_OFFSET_FILLERBYTE1            	5 //size 2 byte(s)
#define SMS_CB_ROUTING_RESP_OFFSET_SUBBLOCKCOUNT          	7 //size 1 byte(s)
#define SIZE_SMS_CB_ROUTING_RESP                          	8


//Definition for SMS_CB_ROUTING_IND
#define SMS_CB_ROUTING_IND                                	0x36                                              	
#define SMS_CB_ROUTING_IND_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define SMS_CB_ROUTING_IND_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define SMS_CB_ROUTING_IND_OFFSET_FILLERBYTE1             	2 //size 1 byte(s)
#define SMS_CB_ROUTING_IND_OFFSET_SUBBLOCKCOUNT           	3 //size 1 byte(s)
#define SIZE_SMS_CB_ROUTING_IND                           	4


//Definition for SMS_CB_SIM_ROUTING_IND
#define SMS_CB_SIM_ROUTING_IND                            	0x37                                              	
#define SMS_CB_SIM_ROUTING_IND_OFFSET_TRANSID             	0 //size 1 byte(s)
#define SMS_CB_SIM_ROUTING_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define SMS_CB_SIM_ROUTING_IND_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define SMS_CB_SIM_ROUTING_IND_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_SMS_CB_SIM_ROUTING_IND                       	4


//Definition for SMS_SETTINGS_UPDATE_REQ
#define SMS_SETTINGS_UPDATE_REQ                           	0x30                                              	
#define SMS_SETTINGS_UPDATE_REQ_OFFSET_TRANSID            	0 //size 1 byte(s)
#define SMS_SETTINGS_UPDATE_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define SMS_SETTINGS_UPDATE_REQ_OFFSET_SETTINGTYPE        	2 //size 1 byte(s)
#define SMS_SETTINGS_UPDATE_REQ_OFFSET_SUBBLOCKCOUNT      	3 //size 1 byte(s)
#define SIZE_SMS_SETTINGS_UPDATE_REQ                      	4


//Definition for SMS_SETTINGS_UPDATE_RESP
#define SMS_SETTINGS_UPDATE_RESP                          	0x31                                              	
#define SMS_SETTINGS_UPDATE_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define SMS_SETTINGS_UPDATE_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define SMS_SETTINGS_UPDATE_RESP_OFFSET_SMSGSMSETTINGTYPE 	2 //size 1 byte(s)
#define SMS_SETTINGS_UPDATE_RESP_OFFSET_SMSCAUSE          	3 //size 1 byte(s)
#define SIZE_SMS_SETTINGS_UPDATE_RESP                     	4


//Definition for SMS_SETTINGS_READ_REQ
#define SMS_SETTINGS_READ_REQ                             	0x32                                              	
#define SMS_SETTINGS_READ_REQ_OFFSET_TRANSID              	0 //size 1 byte(s)
#define SMS_SETTINGS_READ_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SMS_SETTINGS_READ_REQ_OFFSET_SMSSETTINGTYPE       	2 //size 1 byte(s)
#define SMS_SETTINGS_READ_REQ_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_SMS_SETTINGS_READ_REQ                        	4


//Definition for SMS_SETTINGS_READ_RESP
#define SMS_SETTINGS_READ_RESP                            	0x33                                              	
#define SMS_SETTINGS_READ_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define SMS_SETTINGS_READ_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define SMS_SETTINGS_READ_RESP_OFFSET_SMSCAUSE            	2 //size 1 byte(s)
#define SMS_SETTINGS_READ_RESP_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_SMS_SETTINGS_READ_RESP                       	4


//Definition for SMS_STATUS_REQ
#define SMS_STATUS_REQ                                    	0x2E                                              	
#define SMS_STATUS_REQ_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define SMS_STATUS_REQ_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define SMS_STATUS_REQ_OFFSET_FILLERBYTE1                 	2 //size 2 byte(s)
#define SIZE_SMS_STATUS_REQ                               	4


//Definition for SMS_STATUS_RESP
#define SMS_STATUS_RESP                                   	0x2F                                              	
#define SMS_STATUS_RESP_OFFSET_TRANSID                    	0 //size 1 byte(s)
#define SMS_STATUS_RESP_OFFSET_MESSAGEID                  	1 //size 1 byte(s)
#define SMS_STATUS_RESP_OFFSET_SERVERSTATUS               	2 //size 1 byte(s)
#define SMS_STATUS_RESP_OFFSET_SUBBLOCKCOUNT              	3 //size 1 byte(s)
#define SIZE_SMS_STATUS_RESP                              	4


//Definition for SMS_STATUS_IND
#define SMS_STATUS_IND                                    	0x2D                                              	
#define SMS_STATUS_IND_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define SMS_STATUS_IND_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define SMS_STATUS_IND_OFFSET_SERVERSTATUS                	2 //size 1 byte(s)
#define SMS_STATUS_IND_OFFSET_SUBBLOCKCOUNT               	3 //size 1 byte(s)
#define SIZE_SMS_STATUS_IND                               	4


//Definition for SMS_RESOURCE_CONF_REQ
#define SMS_RESOURCE_CONF_REQ                             	0x45                                              	
#define SMS_RESOURCE_CONF_REQ_OFFSET_TRANSID              	0 //size 1 byte(s)
#define SMS_RESOURCE_CONF_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SMS_RESOURCE_CONF_REQ_OFFSET_CONFOPERATION        	2 //size 1 byte(s)
#define SMS_RESOURCE_CONF_REQ_OFFSET_SUBBLOCKCOUNT        	3 //size 1 byte(s)
#define SIZE_SMS_RESOURCE_CONF_REQ                        	4


//Definition for SMS_RESOURCE_CONF_RESP
#define SMS_RESOURCE_CONF_RESP                            	0x46                                              	
#define SMS_RESOURCE_CONF_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define SMS_RESOURCE_CONF_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define SMS_RESOURCE_CONF_RESP_OFFSET_CONFOPERATION       	2 //size 1 byte(s)
#define SMS_RESOURCE_CONF_RESP_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_SMS_RESOURCE_CONF_RESP                       	4


//Definition for SMS_RESOURCE_CONF_IND
#define SMS_RESOURCE_CONF_IND                             	0x47                                              	
#define SMS_RESOURCE_CONF_IND_OFFSET_TRANSID              	0 //size 1 byte(s)
#define SMS_RESOURCE_CONF_IND_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SMS_RESOURCE_CONF_IND_OFFSET_CONFSTATUS           	2 //size 1 byte(s)
#define SMS_RESOURCE_CONF_IND_OFFSET_SUBBLOCKCOUNT        	3 //size 1 byte(s)
#define SIZE_SMS_RESOURCE_CONF_IND                        	4


//Definition for SMS_RESOURCE_CLEAR_IND
#define SMS_RESOURCE_CLEAR_IND                            	0x48                                              	
#define SMS_RESOURCE_CLEAR_IND_OFFSET_TRANSID             	0 //size 1 byte(s)
#define SMS_RESOURCE_CLEAR_IND_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define SMS_RESOURCE_CLEAR_IND_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define SMS_RESOURCE_CLEAR_IND_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_SMS_RESOURCE_CLEAR_IND                       	4


//Definition for SMS_RESOURCE_REQ
#define SMS_RESOURCE_REQ                                  	0x49                                              	
#define SMS_RESOURCE_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define SMS_RESOURCE_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define SMS_RESOURCE_REQ_OFFSET_FILLERBYTE1               	2 //size 1 byte(s)
#define SMS_RESOURCE_REQ_OFFSET_SUBBLOCKCOUNT             	3 //size 1 byte(s)
#define SIZE_SMS_RESOURCE_REQ                             	4


//Definition for SMS_RESOURCE_RESP
#define SMS_RESOURCE_RESP                                 	0x4A                                              	
#define SMS_RESOURCE_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define SMS_RESOURCE_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define SMS_RESOURCE_RESP_OFFSET_FILLERBYTE1              	2 //size 1 byte(s)
#define SMS_RESOURCE_RESP_OFFSET_SUBBLOCKCOUNT            	3 //size 1 byte(s)
#define SIZE_SMS_RESOURCE_RESP                            	4


//Definition for SMS_RESOURCE_IND
#define SMS_RESOURCE_IND                                  	0x4B                                              	
#define SMS_RESOURCE_IND_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define SMS_RESOURCE_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define SMS_RESOURCE_IND_OFFSET_FILLERBYTE1               	2 //size 1 byte(s)
#define SMS_RESOURCE_IND_OFFSET_SUBBLOCKCOUNT             	3 //size 1 byte(s)
#define SIZE_SMS_RESOURCE_IND                             	4

#endif