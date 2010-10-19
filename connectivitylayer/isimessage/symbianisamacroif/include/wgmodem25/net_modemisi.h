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
* Name      : PN_MODEM_NETWORK [ 0xC8 ] Resource Modem Network Select Server
* Version   : 016.009
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_NET_MODEMISI_H_
#define _SYMBIAN_ISI_NET_MODEMISI_H_

#define MODEM_NETWORK_SELECT_SERVER_ISI_VERSION_Z	 16
#define MODEM_NETWORK_SELECT_SERVER_ISI_VERSION_Y	 12


//CONSTANTS
#define NET_INVALID_TIME                                  	0x64                                              	
#define NET_FILL                                          	0x00                                                 	
#define NET_ALPHA_TAG_LEN                                 	0x20                                              	
#define NET_ECID_UCID_UNDEFINED                           	0xFFFFFFFF                                        	
#define NET_ECID_CPICH_ECNO_UNDEFINED                     	0xFF                                              	
#define NET_ECID_CPICH_RSCP_UNDEFINED                     	-127                                              	
#define NET_ECID_PATHLOSS_UNDEFINED                       	0xFF                                              	
#define NET_MODEM_ANY_SIZE                                	0x01                                                 	
#define NET_RSSI_UNDEFINED                                	0x00                                                 	
#define NET_INDEX_NOT_USED                                	0x00                                                 	
#define NET_SCRAMBLING_CODE_UNDEFINED                     	0xFFFF                                            	
#define NET_ECID_TA_UNDEFINED                             	0xFF                                              	
#define NET_NO_RSSI_INDICATED                             	0x00                                                 	
#define NET_ALL_RSSI_INDICATED                            	0xFF                                              	


//TABLES

//ConstantTable for NET_MODEM_SUCCESS_CODES
#define NET_CAUSE_OK                                      	0x00                                                 	
#define NET_CAUSE_COMMUNICATION_ERROR                     	0x01                                                 	
#define NET_CAUSE_INVALID_PARAMETER                       	0x02                                                 	
#define NET_CAUSE_NO_SIM                                  	0x03                                                 	
#define NET_CAUSE_SIM_NOT_YET_READY                       	0x04                                                 	
#define NET_CAUSE_NET_NOT_FOUND                           	0x05                                                 	
#define NET_CAUSE_REQUEST_NOT_ALLOWED                     	0x06                                                 	
#define NET_CAUSE_CALL_ACTIVE                             	0x07                                                 	
#define NET_CAUSE_SERVER_BUSY                             	0x08                                                 	
#define NET_CAUSE_SECURITY_CODE_REQUIRED                  	0x09                                                 	
#define NET_CAUSE_NOTHING_TO_CANCEL                       	0x0A                                                 	
#define NET_CAUSE_UNABLE_TO_CANCEL                        	0x0B                                                 	
#define NET_CAUSE_NETWORK_FORBIDDEN                       	0x0C                                                 	
#define NET_CAUSE_REQUEST_REJECTED                        	0x0D                                                 	
#define NET_CAUSE_CS_NOT_SUPPORTED                        	0x0E                                                 	
#define NET_CAUSE_PAR_INFO_NOT_AVAILABLE                  	0x0F                                                 	
#define NET_CAUSE_NOT_DONE                                	0x10                                              	
#define NET_CAUSE_NO_SELECTED_NETWORK                     	0x11                                              	
#define NET_CAUSE_REQUEST_INTERRUPTED                     	0x12                                              	
#define NET_CAUSE_TOO_BIG_INDEX                           	0x14                                              	
#define NET_CAUSE_MEMORY_FULL                             	0x15                                              	
#define NET_CAUSE_SERVICE_NOT_ALLOWED                     	0x16                                              	
#define NET_CAUSE_NOT_SUPPORTED_IN_TECH                   	0x17                                              	

//ConstantTable for NET_MODEM_NETWORK_SEARCH_MODE
#define NET_MANUAL_SEARCH                                 	0x00                                                 	

//ConstantTable for NET_CS_TYPE
#define NET_CS_GSM                                        	0x00                                                 	

//ConstantTable for NET_CS_OPERATION_MODE
#define NET_CS_OP_MODE_NORMAL                             	0x00                                                 	
#define NET_CS_OP_MODE_GAN_ONLY                           	0x01                                                 	

//ConstantTable for NET_MEASUREMENT_TYPES
#define NET_CURRENT_CELL_RSSI                             	0x02                                                 	

//ConstantTable for NET_CONTROL_MESSAGE_TYPES
#define NET_CS_POWER_OFF                                  	0x02                                                 	
#define NET_CS_POWER_ON                                   	0x03                                                 	
#define NET_CS_POWER_WIFI                                 	0x05                                                 	

//ConstantTable for NET_MODEM_REGISTRATION_STATUS
#define NET_REG_STATUS_HOME                               	0x00                                                 	
#define NET_REG_STATUS_ROAM                               	0x01                                                 	
#define NET_REG_STATUS_NOSERV                             	0x03                                                 	
#define NET_REG_STATUS_NOSERV_SEARCHING                   	0x04                                                 	
#define NET_REG_STATUS_NOSERV_NOTSEARCHING                	0x05                                                 	
#define NET_REG_STATUS_NOSERV_NOSIM                       	0x06                                                 	
#define NET_REG_STATUS_POWER_OFF                          	0x08                                                 	
#define NET_REG_STATUS_NSPS                               	0x09                                                 	
#define NET_REG_STATUS_NSPS_NO_COVERAGE                   	0x0A                                                 	
#define NET_REG_STATUS_NOSERV_SIM_REJECTED_BY_NW          	0x0B                                                 	

//ConstantTable for NET_SERVICE_STATUS
#define NET_SERVICE                                       	0x00                                                 	
#define NET_LIMITED_SERVICE                               	0x01                                                 	
#define NET_NO_COVERAGE                                   	0x02                                                 	

//ConstantTable for NET_MODEM_NETWORK_SELECTION_MODE
#define NET_SELECT_MODE_UNKNOWN                           	0x00                                                 	
#define NET_SELECT_MODE_MANUAL                            	0x01                                                 	
#define NET_SELECT_MODE_AUTOMATIC                         	0x02                                                 	
#define NET_SELECT_MODE_USER_RESELECTION                  	0x03                                                 	
#define NET_SELECT_MODE_NO_SELECTION                      	0x04                                                 	
#define NET_SELECT_MODE_POWER_ON_AUTOMATIC                	0x05                                                 	
#define NET_SELECT_MODE_POWER_ON_MANUAL                   	0x06                                                 	
#define NET_SELECT_MODE_POWER_ON_NO_SELECTION             	0x07                                                 	

//ConstantTable for NET_MODEM_NETWORK_STATUS
#define NET_OPER_STATUS_UNKNOWN                           	0x00                                                 	
#define NET_OPER_STATUS_AVAILABLE                         	0x01                                                 	
#define NET_OPER_STATUS_CURRENT                           	0x02                                                 	
#define NET_OPER_STATUS_FORBIDDEN                         	0x03                                                 	

//BitmaskTables for NET_ADDITIONAL_RAT_INFO
#define NET_UMA_RAT                                       	0x02                                                 	

//ConstantTable for NET_TEST_CARRIER_SETTING_OPERATIONS
#define NET_TEST_CARRIER_SET                              	0x00                                                 	
#define NET_TEST_CARRIER_CLEAR                            	0x01                                                 	

//ConstantTable for NET_TEST_CARRIER_TYPE
#define NET_PRODUCTION_CARRIER                            	0x00                                                 	

//ConstantTable for NET_BAND_SELECTION
#define NET_GSM900_1800                                   	0x00                                                 	
#define NET_GSM850_1900                                   	0x01                                                 	

//ConstantTable for NET_CIPHERING_STATUS
#define NET_CIPHERING_INDICATOR_OFF                       	0x00                                                 	
#define NET_CIPHERING_INDICATOR_ON                        	0x01                                                 	
#define NET_CIPHERING_NO_CONNECTION                       	0x02                                                 	

//ConstantTable for NET_CIPHERING_KEY_STATUS
#define NET_KEY_STATUS_UNKNOWN                            	0x00                                                 	
#define NET_KEY_STATUS_DERIVED                            	0x01                                                 	
#define NET_KEY_STATUS_NOT_DERIVED                        	0x02                                                 	

//ConstantTable for NET_CIPHERING_CONTEXT
#define NET_CONTEXT_UNKNOWN                               	0x00                                                 	
#define NET_CONTEXT_GSM                                   	0x01                                                 	
#define NET_CONTEXT_UMTS                                  	0x02                                                 	
#define NET_CONTEXT_EPS                                   	0x04                                                 	

//ConstantTable for NET_ACTIVE_RAT
#define NET_ACTIVE_RAT_UNKNOWN                            	0x00                                                 	
#define NET_ACTIVE_RAT_GSM                                	0x01                                                 	
#define NET_ACTIVE_RAT_UMTS                               	0x02                                                 	
#define NET_ACTIVE_RAT_EPS                                	0x04                                                 	

//ConstantTable for NET_RAT_NAME
#define NET_GSM_RAT                                       	0x01                                                 	
#define NET_UMTS_RAT                                      	0x02                                                 	
#define NET_EPS_RAT                                       	0x04                                                 	

//ConstantTable for NET_RAT_TYPE
#define NET_CURRENT_RAT                                   	0x00                                                 	
#define NET_SUPPORTED_RATS                                	0x01                                                 	

//ConstantTable for NET_DAYLIGHT_SAVING_TIME
#define NET_DST_INFO_NOT_AVAIL                            	0x64                                              	
#define NET_DST_1_HOUR                                    	0x01                                                 	
#define NET_DST_2_HOURS                                   	0x02                                                 	
#define NET_DST_0_HOUR                                    	0x00                                                 	

//ConstantTable for NET_CS_STATES
#define NET_CS_INACTIVE                                   	0x00                                                 	
#define NET_CS_ACTIVE                                     	0x01                                                 	

//ConstantTable for NET_MODEM_GSM_NETWORK_TYPE
#define NET_GSM_HOME_PLMN                                 	0x00                                                 	
#define NET_GSM_PREFERRED_PLMN                            	0x01                                                 	
#define NET_GSM_FORBIDDEN_PLMN                            	0x02                                                 	
#define NET_GSM_OTHER_PLMN                                	0x03                                                 	
#define NET_GSM_NO_PLMN_AVAIL                             	0x04                                                 	

//ConstantTable for NET_MODEM_UMTS_AVAIL_INFO
#define NET_UMTS_NOT_AVAILABLE                            	0x00                                                 	
#define NET_UMTS_AVAILABLE                                	0x01                                                 	

//ConstantTable for NET_MODEM_REJECT_CODES
#define NET_GSM_IMSI_UNKNOWN_IN_HLR                       	0x02                                                 	
#define NET_GSM_ILLEGAL_MS                                	0x03                                                 	
#define NET_GSM_IMSI_UNKNOWN_IN_VLR                       	0x04                                                 	
#define NET_GSM_IMEI_NOT_ACCEPTED                         	0x05                                                 	
#define NET_GSM_ILLEGAL_ME                                	0x06                                                 	
#define NET_GSM_GPRS_SERVICES_NOT_ALLOWED                 	0x07                                                 	
#define NET_GSM_GPRS_AND_NON_GPRS_NA                      	0x08                                                 	
#define NET_GSM_MS_ID_CANNOT_BE_DERIVED                   	0x09                                                 	
#define NET_GSM_IMPLICITLY_DETACHED                       	0x0A                                                 	
#define NET_GSM_PLMN_NOT_ALLOWED                          	0x0B                                                 	
#define NET_GSM_LA_NOT_ALLOWED                            	0x0C                                                 	
#define NET_GSM_ROAMING_NOT_IN_THIS_LA                    	0x0D                                                 	
#define NET_GSM_GPRS_SERV_NA_IN_THIS_PLMN                 	0x0E                                                 	
#define NET_GSM_NO_SUITABLE_CELLS_IN_LA                   	0x0F                                                 	
#define NET_GSM_MSC_TEMP_NOT_REACHABLE                    	0x10                                              	
#define NET_GSM_NETWORK_FAILURE                           	0x11                                              	
#define NET_GSM_MAC_FAILURE                               	0x14                                              	
#define NET_GSM_SYNCH_FAILURE                             	0x15                                              	
#define NET_GSM_CONGESTION                                	0x16                                              	
#define NET_GSM_AUTH_UNACCEPTABLE                         	0x17                                              	
#define NET_GSM_SERV_OPT_NOT_SUPPORTED                    	0x20                                              	
#define NET_GSM_SERV_OPT_NOT_SUBSCRIBED                   	0x21                                              	
#define NET_GSM_SERV_TEMP_OUT_OF_ORDER                    	0x22                                              	
#define NET_GSM_RETRY_ENTRY_NEW_CELL_LOW                  	0x30                                              	
#define NET_GSM_RETRY_ENTRY_NEW_CELL_HIGH                 	0x3F                                              	
#define NET_GSM_SEMANTICALLY_INCORRECT                    	0x5F                                              	
#define NET_GSM_INVALID_MANDATORY_INFO                    	0x60                                              	
#define NET_GSM_MSG_TYPE_NONEXISTENT                      	0x61                                              	
#define NET_GSM_CONDITIONAL_IE_ERROR                      	0x64                                              	
#define NET_GSM_MSG_TYPE_WRONG_STATE                      	0x65                                              	
#define NET_GSM_PROTOCOL_ERROR_UNSPECIFIED                	0x6F                                              	

//ConstantTable for NET_NEIGHBOUR_CELL_INFO_TYPE
#define NET_HOME_ZONE_CELLS                               	0x00                                                 	
#define NET_SIM_NMR                                       	0x01                                                 	
#define NET_SIM_INTRA_FREQ_NMR                            	0x02                                                 	
#define NET_SIM_INTER_FREQ_NMR                            	0x03                                                 	
#define NET_SIM_INTER_RAT_NMR                             	0x04                                                 	
#define NET_ECID_INFORMATION                              	0x05                                                 	

//ConstantTable for NET_MODEM_GSM_BAND_INFO_TBL
#define NET_GSM_BAND_900_1800                             	0x00                                                 	
#define NET_GSM_BAND_850_1900                             	0x01                                                 	
#define NET_GSM_BAND_INFO_NOT_AVAIL                       	0x02                                                 	
#define NET_GSM_BAND_ALL_SUPPORTED_BANDS                  	0x03                                                 	

//BitmaskTables for NET_GSM_BANDS_INFO_TBL
#define NET_GSM_BAND_ALL_MASK                             	0x00                                                 	
#define NET_GSM_BAND_900_MASK                             	0x01                                                 	
#define NET_GSM_BAND_1800_MASK                            	0x02                                                 	
#define NET_GSM_BAND_1900_MASK                            	0x04                                                 	
#define NET_GSM_BAND_850_MASK                             	0x08                                                 	

//BitmaskTables for NET_WCDMA_BANDS_INFO_TBL
#define NET_WCDMA_FDD_BAND_ALL_MASK                       	0x00                                                 	
#define NET_WCDMA_FDD_BAND_1_MASK                         	0x01                                                 	
#define NET_WCDMA_FDD_BAND_2_MASK                         	0x02                                                 	
#define NET_WCDMA_FDD_BAND_3_MASK                         	0x04                                                 	
#define NET_WCDMA_FDD_BAND_4_MASK                         	0x08                                                 	
#define NET_WCDMA_FDD_BAND_5_MASK                         	0x10                                              	
#define NET_WCDMA_FDD_BAND_6_MASK                         	0x20                                              	
#define NET_WCDMA_FDD_BAND_7_MASK                         	0x40                                              	
#define NET_WCDMA_FDD_BAND_8_MASK                         	0x80                                              	
#define NET_WCDMA_FDD_BAND_9_MASK                         	0x100                                             	
#define NET_WCDMA_FDD_BAND_10_MASK                        	0x200                                             	
#define NET_WCDMA_FDD_BAND_11_MASK                        	0x400                                             	
#define NET_WCDMA_FDD_BAND_12_MASK                        	0x800                                             	
#define NET_WCDMA_FDD_BAND_13_MASK                        	0x1000                                            	
#define NET_WCDMA_FDD_BAND_14_MASK                        	0x2000                                            	
#define NET_WCDMA_FDD_BAND_15_MASK                        	0x4000                                            	
#define NET_WCDMA_FDD_BAND_16_MASK                        	0x8000                                            	
#define NET_WCDMA_FDD_BAND_17_MASK                        	0x10000                                           	
#define NET_WCDMA_FDD_BAND_18_MASK                        	0x20000                                           	
#define NET_WCDMA_FDD_BAND_19_MASK                        	0x40000                                           	
#define NET_WCDMA_FDD_BAND_20_MASK                        	0x80000                                           	
#define NET_WCDMA_FDD_BAND_21_MASK                        	0x100000                                          	
#define NET_WCDMA_FDD_BAND_22_MASK                        	0x200000                                          	
#define NET_WCDMA_FDD_BAND_23_MASK                        	0x400000                                          	
#define NET_WCDMA_FDD_BAND_24_MASK                        	0x800000                                          	

//BitmaskTables for NET_EUTRAN_BANDS_INFO_TBL
#define NET_EUTRAN_NO_BANDS                               	0x00                                                 	
#define NET_EUTRAN_BAND_1_MASK                            	0x01                                                 	
#define NET_EUTRAN_BAND_2_MASK                            	0x02                                                 	
#define NET_EUTRAN_BAND_3_MASK                            	0x04                                                 	
#define NET_EUTRAN_BAND_4_MASK                            	0x08                                                 	
#define NET_EUTRAN_BAND_5_MASK                            	0x10                                              	
#define NET_EUTRAN_BAND_6_MASK                            	0x20                                              	
#define NET_EUTRAN_BAND_7_MASK                            	0x40                                              	
#define NET_EUTRAN_BAND_8_MASK                            	0x80                                              	
#define NET_EUTRAN_BAND_9_MASK                            	0x100                                             	
#define NET_EUTRAN_BAND_10_MASK                           	0x200                                             	
#define NET_EUTRAN_BAND_11_MASK                           	0x400                                             	

//ConstantTable for NET_MODEM_GPRS_NETWORK_MODE
#define NET_GPRS_MODE_NONE                                	0x00                                                 	
#define NET_GPRS_MODE_I                                   	0x01                                                 	
#define NET_GPRS_MODE_II                                  	0x02                                                 	
#define NET_GPRS_MODE_III                                 	0x03                                                 	

//ConstantTable for NET_UMA_FAILURE_REASON
#define NET_NETWORK_CONGESTION                            	0x00                                                 	
#define NET_AP_NOT_ALLOWED                                	0x01                                                 	
#define NET_LOCATION_NOT_ALLOWED                          	0x02                                                 	
#define NET_INVALID_UNC                                   	0x03                                                 	
#define NET_GEO_LOCATION_NOT_KNOWN                        	0x04                                                 	
#define NET_IMSI_NOT_ALLOWED                              	0x05                                                 	
#define NET_UNSPECIFIED                                   	0x06                                                 	
#define NET_UNC_SGW_CERTIF_NOT_VALID                      	0x07                                                 	
#define NET_EAP_SIM_AUTH_FAILED                           	0x08                                                 	
#define NET_TCP_ESTABLISHMENT_FAILED                      	0x09                                                 	
#define NET_REDIRECTION                                   	0x0A                                                 	
#define NET_EAP_AKA_AUTH_FAILED                           	0x0B                                                 	
#define NET_TCP_CONNECTION_RESET                          	0x14                                              	
#define NET_TCP_LINK_CREATION_FAILED                      	0x15                                              	
#define NET_DHCP_QUERY_NO_IP_ADDRESS                      	0x16                                              	
#define NET_DNS_QUERY_FAILED                              	0x17                                              	
#define NET_ARP_PROCEDURE_FAILED                          	0x18                                              	
#define NET_NW_LINK_CREATION_FAILED                       	0x19                                              	
#define NET_SEC_TUNNEL_CREATION_FAIL                      	0x1A                                              	
#define NET_WLAN_AUTH_FAILED                              	0x1E                                              	
#define NET_WLAN_COVERAGE_LOSS                            	0x1F                                              	
#define NET_WLAN_GENERIC_FAILURE                          	0x20                                              	
#define NET_UMA_PLMN_REJECTED                             	0x21                                              	
#define NET_UMA_LU_REJECTED                               	0x22                                              	
#define NET_IP_ADDRESS_UPDATED                            	0x23                                              	
#define NET_NW_LINK_LOST                                  	0x24                                              	
#define NET_TCP_CONNECTION_LOST                           	0x25                                              	
#define NET_UMA_UNRECOVERABLE_LOOP                        	0x26                                              	
#define NET_UMA_VOICE_CALL_FAILED                         	0x28                                              	
#define NET_DNS_QUERY_SGW_FAILED                          	0x29                                              	
#define NET_DNS_QUERY_UNC_FAILED                          	0x30                                              	
#define NET_UMA_CONNECTED                                 	0xFE                                              	

//ConstantTable for NET_UMA_FAILURE_SEVERITY
#define NET_SEVERITY_MINOR                                	0x00                                                 	
#define NET_SEVERITY_SEVERE                               	0x01                                                 	
#define NET_SEVERITY_FATAL                                	0x02                                                 	

//ConstantTable for NET_UMA_FAILURE_PHASE
#define NET_DISCOVERY_PROCEDURE                           	0x00                                                 	
#define NET_REGISTRATION_PROCEDURE                        	0x01                                                 	
#define NET_REG_REDIR_PROCEDURE                           	0x02                                                 	
#define NET_ERR_AFTER_UMA_REGISTRATION                    	0x03                                                 	

//ConstantTable for NET_UTRAN_RADIO_STATE
#define NET_UTRAN_RADIO_IDLE                              	0x00                                                 	
#define NET_UTRAN_RADIO_CELL_DCH                          	0x01                                                 	

//ConstantTable for NET_SIGNAL_LEVEL_TYPE_TBL
#define PERCENT                                           	0x00                                                 	
#define DBM                                               	0x01                                                 	


//STRUCTURES


//Definition for NET_GSM_NMR_LIST_SEQ
#define NET_GSM_NMR_LIST_SEQ_OFFSET_ARFCN                 	0 //size 2 byte(s)
#define NET_GSM_NMR_LIST_SEQ_OFFSET_BSIC                  	2 //size 1 byte(s)
#define NET_GSM_NMR_LIST_SEQ_OFFSET_RXLEVEL               	3 //size 1 byte(s)
#define SIZE_NET_GSM_NMR_LIST_SEQ                         	4


//Definition for NET_UTRAN_CELL_LIST_SEQ
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_UCID               	0 //size 4 byte(s)
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_PCPICH             	4 //size 2 byte(s)
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_CPICHECNO          	6 //size 1 byte(s)
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_CPICHRSCP          	7 //size 1 byte(s)
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_PATHLOSS           	8 //size 1 byte(s)
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_FILLERBYTE1        	9 //size 1 byte(s)
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_FILLERBYTE2        	10 //size 1 byte(s)
#define NET_UTRAN_CELL_LIST_SEQ_OFFSET_FILLERBYTE3        	11 //size 1 byte(s)
#define SIZE_NET_UTRAN_CELL_LIST_SEQ                      	12


//Definition for NET_UTRAN_NEIGH_LIST_SEQ
#define NET_UTRAN_NEIGH_LIST_SEQ_OFFSET_UARFCNDL          	0 //size 2 byte(s)
#define NET_UTRAN_NEIGH_LIST_SEQ_OFFSET_UCRSSI            	2 //size 1 byte(s)
#define NET_UTRAN_NEIGH_LIST_SEQ_OFFSET_NCELL             	3 //size 1 byte(s)
#define NET_UTRAN_NEIGH_LIST_SEQ_OFFSET_DETAILEDCELLINFO  	4 //size 12 byte(s)
#define SIZE_NET_UTRAN_NEIGH_LIST_SEQ                     	16
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//SUBBLOCKS


//Definition for NET_MODEM_REG_INFO_COMMON
#define NET_MODEM_REG_INFO_COMMON                         	0x00                                                 	
#define NET_MODEM_REG_INFO_COMMON_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define NET_MODEM_REG_INFO_COMMON_OFFSET_SUBBLOCKLENGTH   	1 //size 1 byte(s)
#define NET_MODEM_REG_INFO_COMMON_OFFSET_REGISTRATIONSTATUS	2 //size 1 byte(s)
#define NET_MODEM_REG_INFO_COMMON_OFFSET_NETWORKSELECTIONMODE	3 //size 1 byte(s)
#define SIZE_NET_MODEM_REG_INFO_COMMON                    	4


//Definition for NET_MODEM_AVAIL_NETWORK_INFO_COMMON
#define NET_MODEM_AVAIL_NETWORK_INFO_COMMON               	0x01                                                 	
#define NET_MODEM_AVAIL_NETWORK_INFO_COMMON_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define NET_MODEM_AVAIL_NETWORK_INFO_COMMON_OFFSET_SUBBLOCKLENGTH	1 //size 1 byte(s)
#define NET_MODEM_AVAIL_NETWORK_INFO_COMMON_OFFSET_NETWORKSTATUS	2 //size 1 byte(s)
#define NET_MODEM_AVAIL_NETWORK_INFO_COMMON_OFFSET_INDEX  	3 //size 1 byte(s)
#define SIZE_NET_MODEM_AVAIL_NETWORK_INFO_COMMON          	4


//Definition for NET_OPERATOR_INFO_COMMON
#define NET_OPERATOR_INFO_COMMON                          	0x02                                                 	
#define NET_OPERATOR_INFO_COMMON_OFFSET_SUBBLOCKID        	0 //size 1 byte(s)
#define NET_OPERATOR_INFO_COMMON_OFFSET_SUBBLOCKLENGTH    	1 //size 1 byte(s)
#define NET_OPERATOR_INFO_COMMON_OFFSET_SELECTIONMODE     	2 //size 1 byte(s)
#define NET_OPERATOR_INFO_COMMON_OFFSET_INDEX             	3 //size 1 byte(s)
#define SIZE_NET_OPERATOR_INFO_COMMON                     	4


//Definition for NET_RSSI_CURRENT
#define NET_RSSI_CURRENT                                  	0x04                                                 	
#define NET_RSSI_CURRENT_OFFSET_SUBBLOCKID                	0 //size 1 byte(s)
#define NET_RSSI_CURRENT_OFFSET_SUBBLOCKLENGTH            	1 //size 1 byte(s)
#define NET_RSSI_CURRENT_OFFSET_SIGNALBARS                	2 //size 1 byte(s)
#define NET_RSSI_CURRENT_OFFSET_RSSIINDBM                 	3 //size 1 byte(s)
#define SIZE_NET_RSSI_CURRENT                             	4


//Definition for NET_TEST_CARRIER_PARAM
#define NET_TEST_CARRIER_PARAM                            	0x05                                                 	
#define NET_TEST_CARRIER_PARAM_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define NET_TEST_CARRIER_PARAM_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define NET_TEST_CARRIER_PARAM_OFFSET_CARRIERCHANNELNBR   	2 //size 2 byte(s)
#define NET_TEST_CARRIER_PARAM_OFFSET_BANDSELECTION       	4 //size 1 byte(s)
#define NET_TEST_CARRIER_PARAM_OFFSET_TESTCARRIERTYPE     	5 //size 1 byte(s)
#define NET_TEST_CARRIER_PARAM_OFFSET_FILLERBYTE1         	6 //size 1 byte(s)
#define NET_TEST_CARRIER_PARAM_OFFSET_FILLERBYTE2         	7 //size 1 byte(s)
#define SIZE_NET_TEST_CARRIER_PARAM                       	8


//Definition for NET_TEST_WCDMA_PARAMS
#define NET_TEST_WCDMA_PARAMS                             	0x2D                                              	
#define NET_TEST_WCDMA_PARAMS_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define NET_TEST_WCDMA_PARAMS_OFFSET_SUBBLOCKLENGTH       	1 //size 1 byte(s)
#define NET_TEST_WCDMA_PARAMS_OFFSET_CARRIERCHANNELNBR    	2 //size 2 byte(s)
#define NET_TEST_WCDMA_PARAMS_OFFSET_CARRIERSCRAMBLINGCODE	4 //size 2 byte(s)
#define NET_TEST_WCDMA_PARAMS_OFFSET_TESTCARRIERTYPE      	6 //size 1 byte(s)
#define NET_TEST_WCDMA_PARAMS_OFFSET_FILLERBYTE1          	7 //size 1 byte(s)
#define SIZE_NET_TEST_WCDMA_PARAMS                        	8


//Definition for NET_CIPHERING_INFO
#define NET_CIPHERING_INFO                                	0x29                                              	
#define NET_CIPHERING_INFO_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define NET_CIPHERING_INFO_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define NET_CIPHERING_INFO_OFFSET_CIPHERINGKEYSTATUS      	2 //size 1 byte(s)
#define NET_CIPHERING_INFO_OFFSET_CIPHERINGCONTEXT        	3 //size 1 byte(s)
#define NET_CIPHERING_INFO_OFFSET_ACTIVERAT               	4 //size 1 byte(s)
#define NET_CIPHERING_INFO_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define NET_CIPHERING_INFO_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define NET_CIPHERING_INFO_OFFSET_FILLERBYTE3             	7 //size 1 byte(s)
#define SIZE_NET_CIPHERING_INFO                           	8


//Definition for NET_MODEM_GSM_REG_INFO
#define NET_MODEM_GSM_REG_INFO                            	0x09                                                 	
#define NET_MODEM_GSM_REG_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_CURRENTLAC          	2 //size 2 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_CURRENTCELLID       	4 //size 4 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_OPERATORCODE        	8 //size 3 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_GSMBANDINFO         	11 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_NETWORKTYPE         	12 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_GPRSSUPPORTINCELL   	13 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_GPRSNETWORKMODE     	14 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_CSSERVICES          	15 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_GPRSSERVICES        	16 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_EGPRSSUPPORTINCELL  	17 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_DTMSUPPORTINCELL    	18 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_CURRENTRAC          	19 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_HSDPAAVAILABLEINCELL	20 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_HSUPAAVAILABLEINCELL	21 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_CAMPEDINHPLMN       	22 //size 1 byte(s)
#define NET_MODEM_GSM_REG_INFO_OFFSET_RATNAME             	23 //size 1 byte(s)
#define SIZE_NET_MODEM_GSM_REG_INFO                       	24
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_MODEM_CURRENT_CELL_INFO
#define NET_MODEM_CURRENT_CELL_INFO                       	0x39                                              	
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_SUBBLOCKLENGTH 	1 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_CURRENTLAC     	2 //size 2 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_CURRENTCELLID  	4 //size 4 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_OPERATORCODE   	8 //size 3 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_GSMBANDINFO    	11 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_NETWORKTYPE    	12 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_GPRSSUPPORTINCELL	13 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_GPRSNETWORKMODE	14 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_CSSERVICES     	15 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_GPRSSERVICES   	16 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_EGPRSSUPPORTINCELL	17 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_DTMSUPPORTINCELL	18 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_CURRENTRAC     	19 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_HSDPAAVAILABLEINCELL	20 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_HSUPAAVAILABLEINCELL	21 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_CAMPEDINHPLMN  	22 //size 1 byte(s)
#define NET_MODEM_CURRENT_CELL_INFO_OFFSET_FILLERBYTE1    	23 //size 1 byte(s)
#define SIZE_NET_MODEM_CURRENT_CELL_INFO                  	24
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_TIME_INFO
#define NET_TIME_INFO                                     	0x10                                              	
#define NET_TIME_INFO_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_YEAR                         	2 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_MONTH                        	3 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_DAY                          	4 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_HOUR                         	5 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_MINUTE                       	6 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_SECOND                       	7 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_TIMEZONE                     	8 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_DAYLIGHTSAVINGTIME           	9 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_FILLERBYTE1                  	10 //size 1 byte(s)
#define NET_TIME_INFO_OFFSET_FILLERBYTE2                  	11 //size 1 byte(s)
#define SIZE_NET_TIME_INFO                                	12


//Definition for NET_MODEM_DETAILED_NETWORK_INFO
#define NET_MODEM_DETAILED_NETWORK_INFO                   	0x0B                                                 	
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_SUBBLOCKLENGTH	1 //size 1 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_OPERCODE   	2 //size 3 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_BANDINFO   	5 //size 1 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_NETWORKTYPE	6 //size 1 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_UMTSAVAILABLE	7 //size 1 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_CURRENTLAC 	8 //size 2 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_FILLERBYTE1	10 //size 1 byte(s)
#define NET_MODEM_DETAILED_NETWORK_INFO_OFFSET_FILLERBYTE2	11 //size 1 byte(s)
#define SIZE_NET_MODEM_DETAILED_NETWORK_INFO              	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_MODEM_GSM_OPERATOR_INFO
#define NET_MODEM_GSM_OPERATOR_INFO                       	0x0C                                                 	
#define NET_MODEM_GSM_OPERATOR_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define NET_MODEM_GSM_OPERATOR_INFO_OFFSET_SUBBLOCKLENGTH 	1 //size 1 byte(s)
#define NET_MODEM_GSM_OPERATOR_INFO_OFFSET_OPERCODE       	2 //size 3 byte(s)
#define NET_MODEM_GSM_OPERATOR_INFO_OFFSET_BANDINFO       	5 //size 1 byte(s)
#define NET_MODEM_GSM_OPERATOR_INFO_OFFSET_FILLERBYTE1    	6 //size 1 byte(s)
#define NET_MODEM_GSM_OPERATOR_INFO_OFFSET_FILLERBYTE2    	7 //size 1 byte(s)
#define SIZE_NET_MODEM_GSM_OPERATOR_INFO                  	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_GSM_HOME_CELLS_INFO
#define NET_GSM_HOME_CELLS_INFO                           	0x0D                                                 	
#define NET_GSM_HOME_CELLS_INFO_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define NET_GSM_HOME_CELLS_INFO_OFFSET_SUBBLOCKLENGTH     	1 //size 1 byte(s)
#define NET_GSM_HOME_CELLS_INFO_OFFSET_NUMBEROFMEASUREMENTS	2 //size 1 byte(s)
#define NET_GSM_HOME_CELLS_INFO_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define NET_GSM_HOME_CELLS_INFO_OFFSET_CELLIDENTITY1      	4 //size 4 byte(s)
#define NET_GSM_HOME_CELLS_INFO_OFFSET_RESTOFSUBBLOCKDATA 	8 //size 2 byte(s)
#define NET_GSM_HOME_CELLS_INFO_OFFSET_RSSI               	10 //size 1 byte(s)
#define NET_GSM_HOME_CELLS_INFO_OFFSET_FILLERBYTE2        	11 //size 1 byte(s)
#define SIZE_NET_GSM_HOME_CELLS_INFO                      	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_GSM_SIM_NMR_INFO
#define NET_GSM_SIM_NMR_INFO                              	0x0E                                                 	
#define NET_GSM_SIM_NMR_INFO_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define NET_GSM_SIM_NMR_INFO_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define NET_GSM_SIM_NMR_INFO_OFFSET_MEASUREMENTRESULTS    	2 //size 16 byte(s)
#define NET_GSM_SIM_NMR_INFO_OFFSET_FILLERBYTE1           	18 //size 1 byte(s)
#define NET_GSM_SIM_NMR_INFO_OFFSET_NUMBEROFCHANNELS      	19 //size 1 byte(s)
#define NET_GSM_SIM_NMR_INFO_OFFSET_ARFCN                 	20 //size 2 byte(s)
#define NET_GSM_SIM_NMR_INFO_OFFSET_FILLERBYTE2           	22 //size 1 byte(s)
#define NET_GSM_SIM_NMR_INFO_OFFSET_FILLERBYTE3           	23 //size 1 byte(s)
#define SIZE_NET_GSM_SIM_NMR_INFO                         	24
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_MODEM_CAUSE_EXTENSION
#define NET_MODEM_CAUSE_EXTENSION                         	0x0F                                                 	
#define NET_MODEM_CAUSE_EXTENSION_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define NET_MODEM_CAUSE_EXTENSION_OFFSET_SUBBLOCKLENGTH   	1 //size 1 byte(s)
#define NET_MODEM_CAUSE_EXTENSION_OFFSET_REJECTCODE       	2 //size 1 byte(s)
#define NET_MODEM_CAUSE_EXTENSION_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_NET_MODEM_CAUSE_EXTENSION                    	4


//Definition for NET_MODEM_GSM_BAND_INFO
#define NET_MODEM_GSM_BAND_INFO                           	0x11                                              	
#define NET_MODEM_GSM_BAND_INFO_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define NET_MODEM_GSM_BAND_INFO_OFFSET_SUBBLOCKLENGTH     	1 //size 1 byte(s)
#define NET_MODEM_GSM_BAND_INFO_OFFSET_BANDINFO           	2 //size 1 byte(s)
#define NET_MODEM_GSM_BAND_INFO_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_NET_MODEM_GSM_BAND_INFO                      	4


//Definition for NET_UTRAN_SIM_NMR_INFO
#define NET_UTRAN_SIM_NMR_INFO                            	0x3D                                              	
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_MEASREPORTLENGTH    	2 //size 1 byte(s)
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_MEASREPORT          	4 //size 1 byte(s)
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_FILLERBYTE2         	5 //size 1 byte(s)
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_FILLERBYTE3         	6 //size 1 byte(s)
#define NET_UTRAN_SIM_NMR_INFO_OFFSET_FILLERBYTE4         	7 //size 1 byte(s)
#define SIZE_NET_UTRAN_SIM_NMR_INFO                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_ECID_GERAN_INFO
#define NET_ECID_GERAN_INFO                               	0x3E                                              	
#define NET_ECID_GERAN_INFO_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_SUBBLOCKLENGTH         	1 //size 1 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_CURRENTMCC             	2 //size 2 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_CURRENTMNC             	4 //size 2 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_CURRENTLAC             	6 //size 2 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_CURRENTCELLID          	8 //size 2 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_CURRENTTA              	10 //size 1 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_NNMR                   	11 //size 1 byte(s)
#define NET_ECID_GERAN_INFO_OFFSET_GSMNMRLIST             	12 //size 4 byte(s)
#define SIZE_NET_ECID_GERAN_INFO                          	16
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_ECID_UTRAN_FDD_INFO
#define NET_ECID_UTRAN_FDD_INFO                           	0x3F                                              	
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_FILLERBYTE1        	1 //size 1 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_SUBBLOCKLENGTH     	2 //size 2 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_UCID               	4 //size 4 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_CURRENTMCC         	8 //size 2 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_CURRENTMNC         	10 //size 2 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_PRISCRAMBLING      	12 //size 2 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_UARFCNDL           	14 //size 2 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_NNMR               	16 //size 1 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_FILLERBYTE2        	17 //size 1 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_FILLERBYTE3        	18 //size 1 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_FILLERBYTE4        	19 //size 1 byte(s)
#define NET_ECID_UTRAN_FDD_INFO_OFFSET_WCDMANMRLIST       	20 //size 16 byte(s)
#define SIZE_NET_ECID_UTRAN_FDD_INFO                      	36
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_RAT_INFO
#define NET_RAT_INFO                                      	0x2C                                              	
#define NET_RAT_INFO_OFFSET_SUBBLOCKID                    	0 //size 1 byte(s)
#define NET_RAT_INFO_OFFSET_SUBBLOCKLENGTH                	1 //size 1 byte(s)
#define NET_RAT_INFO_OFFSET_RATNAME                       	2 //size 1 byte(s)
#define NET_RAT_INFO_OFFSET_ADDITIONALRATINFOLENGTH       	3 //size 1 byte(s)
#define NET_RAT_INFO_OFFSET_ADDITIONALRATINFO             	4 //size 1 byte(s)
#define NET_RAT_INFO_OFFSET_FILLERBYTE1                   	5 //size 1 byte(s)
#define NET_RAT_INFO_OFFSET_FILLERBYTE2                   	6 //size 1 byte(s)
#define NET_RAT_INFO_OFFSET_FILLERBYTE3                   	7 //size 1 byte(s)
#define SIZE_NET_RAT_INFO                                 	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_MODEM_UMA_SERVICE_ZONE_INFO
#define NET_MODEM_UMA_SERVICE_ZONE_INFO                   	0x37                                              	
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_SUBBLOCKLENGTH	1 //size 1 byte(s)
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_UMASERVICEZONEINICATOR	2 //size 1 byte(s)
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_ALPHATAGLENGTH	3 //size 1 byte(s)
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_ALPHATAG   	4 //size 1 byte(s)
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_FILLERBYTE1	5 //size 1 byte(s)
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_FILLERBYTE2	6 //size 1 byte(s)
#define NET_MODEM_UMA_SERVICE_ZONE_INFO_OFFSET_FILLERBYTE3	7 //size 1 byte(s)
#define SIZE_NET_MODEM_UMA_SERVICE_ZONE_INFO              	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_UMA_FAILURE_INFO
#define NET_UMA_FAILURE_INFO                              	0x38                                              	
#define NET_UMA_FAILURE_INFO_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define NET_UMA_FAILURE_INFO_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define NET_UMA_FAILURE_INFO_OFFSET_FAILURECAUSE          	2 //size 1 byte(s)
#define NET_UMA_FAILURE_INFO_OFFSET_FAILURESEVERITY       	3 //size 1 byte(s)
#define NET_UMA_FAILURE_INFO_OFFSET_FAILUREPHASE          	4 //size 1 byte(s)
#define NET_UMA_FAILURE_INFO_OFFSET_FILLERBYTE1           	5 //size 1 byte(s)
#define NET_UMA_FAILURE_INFO_OFFSET_FILLERBYTE2           	6 //size 1 byte(s)
#define NET_UMA_FAILURE_INFO_OFFSET_FILLERBYTE3           	7 //size 1 byte(s)
#define SIZE_NET_UMA_FAILURE_INFO                         	8


//Definition for NET_UTRAN_RADIO_INFO
#define NET_UTRAN_RADIO_INFO                              	0x3C                                              	
#define NET_UTRAN_RADIO_INFO_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define NET_UTRAN_RADIO_INFO_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define NET_UTRAN_RADIO_INFO_OFFSET_RADIOSTATE            	2 //size 1 byte(s)
#define NET_UTRAN_RADIO_INFO_OFFSET_HSDPAALLOCATED        	3 //size 1 byte(s)
#define NET_UTRAN_RADIO_INFO_OFFSET_HSUPAALLOCATED        	4 //size 1 byte(s)
#define NET_UTRAN_RADIO_INFO_OFFSET_FILLERBYTE1           	5 //size 1 byte(s)
#define NET_UTRAN_RADIO_INFO_OFFSET_FILLERBYTE2           	6 //size 1 byte(s)
#define NET_UTRAN_RADIO_INFO_OFFSET_FILLERBYTE3           	7 //size 1 byte(s)
#define SIZE_NET_UTRAN_RADIO_INFO                         	8


//Definition for NET_UARFCN_INFO
#define NET_UARFCN_INFO                                   	0x28                                              	
#define NET_UARFCN_INFO_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define NET_UARFCN_INFO_OFFSET_SUBBLOCKLENGTH             	1 //size 1 byte(s)
#define NET_UARFCN_INFO_OFFSET_UARFCN                     	2 //size 2 byte(s)
#define SIZE_NET_UARFCN_INFO                              	4


//Definition for NET_GSM_CELL_INFO
#define NET_GSM_CELL_INFO                                 	0x46                                              	
#define NET_GSM_CELL_INFO_OFFSET_SUBBLOCKID               	0 //size 1 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_SUBBLOCKLENGTH           	1 //size 1 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_CURRENTLAC               	2 //size 2 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_CURRENTCELLID            	4 //size 4 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_NETGSMCELLINFOBAND       	8 //size 4 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_OPERATORCODE             	12 //size 3 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_SERVICESTATUS            	15 //size 1 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_NETWORKTYPE              	16 //size 1 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_FILLERBYTE1              	17 //size 1 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_FILLERBYTE2              	18 //size 1 byte(s)
#define NET_GSM_CELL_INFO_OFFSET_FILLERBYTE3              	19 //size 1 byte(s)
#define SIZE_NET_GSM_CELL_INFO                            	20
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_WCDMA_CELL_INFO
#define NET_WCDMA_CELL_INFO                               	0x47                                              	
#define NET_WCDMA_CELL_INFO_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_SUBBLOCKLENGTH         	1 //size 1 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_CURRENTLAC             	2 //size 2 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_CURRENTCELLID          	4 //size 4 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_NETWCDMACELLINFOBAND   	8 //size 4 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_OPERATORCODE           	12 //size 3 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_SERVICESTATUS          	15 //size 1 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_NETWORKTYPE            	16 //size 1 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_FILLERBYTE1            	17 //size 1 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_FILLERBYTE2            	18 //size 1 byte(s)
#define NET_WCDMA_CELL_INFO_OFFSET_FILLERBYTE3            	19 //size 1 byte(s)
#define SIZE_NET_WCDMA_CELL_INFO                          	20
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_EPS_CELL_INFO
#define NET_EPS_CELL_INFO                                 	0x50                                              	
#define NET_EPS_CELL_INFO_OFFSET_SUBBLOCKID               	0 //size 1 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_SUBBLOCKLENGTH           	1 //size 1 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_CURRENTTAC               	2 //size 2 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_CURRENTCELLID            	4 //size 4 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_NETEPSCELLINFOBAND       	8 //size 4 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_OPERATORCODE             	12 //size 3 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_SERVICESTATUS            	15 //size 1 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_NETWORKTYPE              	16 //size 1 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_FILLERBYTE1              	17 //size 1 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_FILLERBYTE2              	18 //size 1 byte(s)
#define NET_EPS_CELL_INFO_OFFSET_FILLERBYTE3              	19 //size 1 byte(s)
#define SIZE_NET_EPS_CELL_INFO                            	20
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_FULL_NITZ_NAME
#define NET_FULL_NITZ_NAME                                	0x48                                              	
#define NET_FULL_NITZ_NAME_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define NET_FULL_NITZ_NAME_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define NET_FULL_NITZ_NAME_OFFSET_DATALENGTH              	2 //size 1 byte(s)
#define NET_FULL_NITZ_NAME_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define NET_FULL_NITZ_NAME_OFFSET_DATA                    	4 //size 1 byte(s)
#define NET_FULL_NITZ_NAME_OFFSET_FILLERBYTE2             	5 //size 1 byte(s)
#define NET_FULL_NITZ_NAME_OFFSET_FILLERBYTE3             	6 //size 1 byte(s)
#define NET_FULL_NITZ_NAME_OFFSET_FILLERBYTE4             	7 //size 1 byte(s)
#define SIZE_NET_FULL_NITZ_NAME                           	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_SHORT_NITZ_NAME
#define NET_SHORT_NITZ_NAME                               	0x49                                              	
#define NET_SHORT_NITZ_NAME_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define NET_SHORT_NITZ_NAME_OFFSET_SUBBLOCKLENGTH         	1 //size 1 byte(s)
#define NET_SHORT_NITZ_NAME_OFFSET_DATALENGTH             	2 //size 1 byte(s)
#define NET_SHORT_NITZ_NAME_OFFSET_FILLERBYTE1            	3 //size 1 byte(s)
#define NET_SHORT_NITZ_NAME_OFFSET_DATA                   	4 //size 1 byte(s)
#define NET_SHORT_NITZ_NAME_OFFSET_FILLERBYTE2            	5 //size 1 byte(s)
#define NET_SHORT_NITZ_NAME_OFFSET_FILLERBYTE3            	6 //size 1 byte(s)
#define NET_SHORT_NITZ_NAME_OFFSET_FILLERBYTE4            	7 //size 1 byte(s)
#define SIZE_NET_SHORT_NITZ_NAME                          	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_RSSI_CONF_INFO
#define NET_RSSI_CONF_INFO                                	0x54                                              	
#define NET_RSSI_CONF_INFO_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define NET_RSSI_CONF_INFO_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define NET_RSSI_CONF_INFO_OFFSET_NETSIGNALLEVELTYPE      	2 //size 1 byte(s)
#define NET_RSSI_CONF_INFO_OFFSET_HYSTERESISDOWN          	3 //size 1 byte(s)
#define NET_RSSI_CONF_INFO_OFFSET_HYSTERESISUP            	4 //size 1 byte(s)
#define NET_RSSI_CONF_INFO_OFFSET_LOWLIMIT                	5 //size 1 byte(s)
#define NET_RSSI_CONF_INFO_OFFSET_UPLIMIT                 	6 //size 1 byte(s)
#define NET_RSSI_CONF_INFO_OFFSET_FILLERBYTE1             	7 //size 1 byte(s)
#define SIZE_NET_RSSI_CONF_INFO                           	8


//MESSAGES


//Definition for NET_MODEM_REG_STATUS_GET_REQ
#define NET_MODEM_REG_STATUS_GET_REQ                      	0x00                                                 	
#define NET_MODEM_REG_STATUS_GET_REQ_OFFSET_TRANSID       	0 //size 1 byte(s)
#define NET_MODEM_REG_STATUS_GET_REQ_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define SIZE_NET_MODEM_REG_STATUS_GET_REQ                 	2


//Definition for NET_MODEM_REG_STATUS_GET_RESP
#define NET_MODEM_REG_STATUS_GET_RESP                     	0x01                                                 	
#define NET_MODEM_REG_STATUS_GET_RESP_OFFSET_TRANSID      	0 //size 1 byte(s)
#define NET_MODEM_REG_STATUS_GET_RESP_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define NET_MODEM_REG_STATUS_GET_RESP_OFFSET_SUCCESSCODE  	2 //size 1 byte(s)
#define NET_MODEM_REG_STATUS_GET_RESP_OFFSET_SUBBLOCKCOUNT	3 //size 1 byte(s)
#define SIZE_NET_MODEM_REG_STATUS_GET_RESP                	4


//Definition for NET_MODEM_REG_STATUS_IND
#define NET_MODEM_REG_STATUS_IND                          	0x02                                                 	
#define NET_MODEM_REG_STATUS_IND_OFFSET_TRANSID           	0 //size 1 byte(s)
#define NET_MODEM_REG_STATUS_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define NET_MODEM_REG_STATUS_IND_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define NET_MODEM_REG_STATUS_IND_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_NET_MODEM_REG_STATUS_IND                     	4


//Definition for NET_MODEM_AVAILABLE_GET_REQ
#define NET_MODEM_AVAILABLE_GET_REQ                       	0x03                                                 	
#define NET_MODEM_AVAILABLE_GET_REQ_OFFSET_TRANSID        	0 //size 1 byte(s)
#define NET_MODEM_AVAILABLE_GET_REQ_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define NET_MODEM_AVAILABLE_GET_REQ_OFFSET_MODE           	2 //size 1 byte(s)
#define NET_MODEM_AVAILABLE_GET_REQ_OFFSET_SUBBLOCKCOUNT  	3 //size 1 byte(s)
#define SIZE_NET_MODEM_AVAILABLE_GET_REQ                  	4


//Definition for NET_MODEM_AVAILABLE_GET_RESP
#define NET_MODEM_AVAILABLE_GET_RESP                      	0x04                                                 	
#define NET_MODEM_AVAILABLE_GET_RESP_OFFSET_TRANSID       	0 //size 1 byte(s)
#define NET_MODEM_AVAILABLE_GET_RESP_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define NET_MODEM_AVAILABLE_GET_RESP_OFFSET_SUCCESSCODE   	2 //size 1 byte(s)
#define NET_MODEM_AVAILABLE_GET_RESP_OFFSET_SUBBLOCKCOUNT 	3 //size 1 byte(s)
#define SIZE_NET_MODEM_AVAILABLE_GET_RESP                 	4


//Definition for NET_AVAILABLE_CANCEL_REQ
#define NET_AVAILABLE_CANCEL_REQ                          	0x05                                                 	
#define NET_AVAILABLE_CANCEL_REQ_OFFSET_TRANSID           	0 //size 1 byte(s)
#define NET_AVAILABLE_CANCEL_REQ_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define SIZE_NET_AVAILABLE_CANCEL_REQ                     	2


//Definition for NET_AVAILABLE_CANCEL_RESP
#define NET_AVAILABLE_CANCEL_RESP                         	0x06                                                 	
#define NET_AVAILABLE_CANCEL_RESP_OFFSET_TRANSID          	0 //size 1 byte(s)
#define NET_AVAILABLE_CANCEL_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define NET_AVAILABLE_CANCEL_RESP_OFFSET_SUCCESSCODE      	2 //size 1 byte(s)
#define SIZE_NET_AVAILABLE_CANCEL_RESP                    	3


//Definition for NET_SET_REQ
#define NET_SET_REQ                                       	0x07                                                 	
#define NET_SET_REQ_OFFSET_TRANSID                        	0 //size 1 byte(s)
#define NET_SET_REQ_OFFSET_MESSAGEID                      	1 //size 1 byte(s)
#define NET_SET_REQ_OFFSET_REGINANOTHERPROTOCOL           	2 //size 1 byte(s)
#define NET_SET_REQ_OFFSET_SUBBLOCKCOUNT                  	3 //size 1 byte(s)
#define SIZE_NET_SET_REQ                                  	4


//Definition for NET_SET_RESP
#define NET_SET_RESP                                      	0x08                                                 	
#define NET_SET_RESP_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define NET_SET_RESP_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define NET_SET_RESP_OFFSET_SUCCESSCODE                   	2 //size 1 byte(s)
#define NET_SET_RESP_OFFSET_SUBBLOCKCOUNT                 	3 //size 1 byte(s)
#define SIZE_NET_SET_RESP                                 	4


//Definition for NET_SET_CANCEL_REQ
#define NET_SET_CANCEL_REQ                                	0x09                                                 	
#define NET_SET_CANCEL_REQ_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define NET_SET_CANCEL_REQ_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define SIZE_NET_SET_CANCEL_REQ                           	2


//Definition for NET_SET_CANCEL_RESP
#define NET_SET_CANCEL_RESP                               	0x0A                                                 	
#define NET_SET_CANCEL_RESP_OFFSET_TRANSID                	0 //size 1 byte(s)
#define NET_SET_CANCEL_RESP_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define NET_SET_CANCEL_RESP_OFFSET_SUCCESSCODE            	2 //size 1 byte(s)
#define SIZE_NET_SET_CANCEL_RESP                          	3


//Definition for NET_RSSI_GET_REQ
#define NET_RSSI_GET_REQ                                  	0x0B                                                 	
#define NET_RSSI_GET_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define NET_RSSI_GET_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define NET_RSSI_GET_REQ_OFFSET_CSTYPE                    	2 //size 1 byte(s)
#define NET_RSSI_GET_REQ_OFFSET_MEASUREMENTTYPE           	3 //size 1 byte(s)
#define SIZE_NET_RSSI_GET_REQ                             	4


//Definition for NET_RSSI_GET_RESP
#define NET_RSSI_GET_RESP                                 	0x0C                                                 	
#define NET_RSSI_GET_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define NET_RSSI_GET_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define NET_RSSI_GET_RESP_OFFSET_SUCCESSCODE              	2 //size 1 byte(s)
#define NET_RSSI_GET_RESP_OFFSET_SUBBLOCKCOUNT            	3 //size 1 byte(s)
#define SIZE_NET_RSSI_GET_RESP                            	4


//Definition for NET_RSSI_IND
#define NET_RSSI_IND                                      	0x1E                                              	
#define NET_RSSI_IND_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define NET_RSSI_IND_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define NET_RSSI_IND_OFFSET_SIGNALSBAR                    	2 //size 1 byte(s)
#define NET_RSSI_IND_OFFSET_RSSIINDBM                     	3 //size 1 byte(s)
#define SIZE_NET_RSSI_IND                                 	4


//Definition for NET_CS_CONTROL_REQ
#define NET_CS_CONTROL_REQ                                	0x0D                                                 	
#define NET_CS_CONTROL_REQ_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define NET_CS_CONTROL_REQ_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define NET_CS_CONTROL_REQ_OFFSET_CONTROLMESSAGE          	2 //size 1 byte(s)
#define NET_CS_CONTROL_REQ_OFFSET_SUBBLOCKCOUNT           	3 //size 1 byte(s)
#define SIZE_NET_CS_CONTROL_REQ                           	4


//Definition for NET_CS_CONTROL_RESP
#define NET_CS_CONTROL_RESP                               	0x0E                                                 	
#define NET_CS_CONTROL_RESP_OFFSET_TRANSID                	0 //size 1 byte(s)
#define NET_CS_CONTROL_RESP_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define NET_CS_CONTROL_RESP_OFFSET_CSTYPE                 	2 //size 1 byte(s)
#define NET_CS_CONTROL_RESP_OFFSET_SUCCESSCODE            	3 //size 1 byte(s)
#define NET_CS_CONTROL_RESP_OFFSET_CONTROLMESSAGE         	4 //size 1 byte(s)
#define SIZE_NET_CS_CONTROL_RESP                          	5


//Definition for NET_CS_WAKEUP_REQ
#define NET_CS_WAKEUP_REQ                                 	0x0F                                                 	
#define NET_CS_WAKEUP_REQ_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define NET_CS_WAKEUP_REQ_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define SIZE_NET_CS_WAKEUP_REQ                            	2


//Definition for NET_CS_WAKEUP_RESP
#define NET_CS_WAKEUP_RESP                                	0x10                                              	
#define NET_CS_WAKEUP_RESP_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define NET_CS_WAKEUP_RESP_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define SIZE_NET_CS_WAKEUP_RESP                           	2


//Definition for NET_TEST_CARRIER_REQ
#define NET_TEST_CARRIER_REQ                              	0x11                                              	
#define NET_TEST_CARRIER_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define NET_TEST_CARRIER_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define NET_TEST_CARRIER_REQ_OFFSET_OPERATION             	2 //size 1 byte(s)
#define NET_TEST_CARRIER_REQ_OFFSET_SUBBLOCKCOUNT         	3 //size 1 byte(s)
#define SIZE_NET_TEST_CARRIER_REQ                         	4


//Definition for NET_TEST_CARRIER_RESP
#define NET_TEST_CARRIER_RESP                             	0x12                                              	
#define NET_TEST_CARRIER_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define NET_TEST_CARRIER_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define NET_TEST_CARRIER_RESP_OFFSET_SUCCESSCODE          	2 //size 1 byte(s)
#define NET_TEST_CARRIER_RESP_OFFSET_SUBBLOCKCOUNT        	3 //size 1 byte(s)
#define SIZE_NET_TEST_CARRIER_RESP                        	4


//Definition for NET_NETWORK_SELECT_MODE_SET_REQ
#define NET_NETWORK_SELECT_MODE_SET_REQ                   	0x1C                                              	
#define NET_NETWORK_SELECT_MODE_SET_REQ_OFFSET_TRANSID    	0 //size 1 byte(s)
#define NET_NETWORK_SELECT_MODE_SET_REQ_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define NET_NETWORK_SELECT_MODE_SET_REQ_OFFSET_SELECTIONMODE	2 //size 1 byte(s)
#define SIZE_NET_NETWORK_SELECT_MODE_SET_REQ              	3


//Definition for NET_NETWORK_SELECT_MODE_SET_RESP
#define NET_NETWORK_SELECT_MODE_SET_RESP                  	0x1D                                              	
#define NET_NETWORK_SELECT_MODE_SET_RESP_OFFSET_TRANSID   	0 //size 1 byte(s)
#define NET_NETWORK_SELECT_MODE_SET_RESP_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define NET_NETWORK_SELECT_MODE_SET_RESP_OFFSET_SUCCESSCODE	2 //size 1 byte(s)
#define SIZE_NET_NETWORK_SELECT_MODE_SET_RESP             	3


//Definition for NET_CIPHERING_IND
#define NET_CIPHERING_IND                                 	0x20                                              	
#define NET_CIPHERING_IND_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define NET_CIPHERING_IND_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define NET_CIPHERING_IND_OFFSET_CIPHERINGSTATUS          	2 //size 1 byte(s)
#define NET_CIPHERING_IND_OFFSET_SUBBLOCKCOUNT            	3 //size 1 byte(s)
#define SIZE_NET_CIPHERING_IND                            	4


//Definition for NET_TIME_IND
#define NET_TIME_IND                                      	0x27                                              	
#define NET_TIME_IND_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define NET_TIME_IND_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define NET_TIME_IND_OFFSET_FILLERBYTE1                   	2 //size 1 byte(s)
#define NET_TIME_IND_OFFSET_SUBBLOCKCOUNT                 	3 //size 1 byte(s)
#define SIZE_NET_TIME_IND                                 	4


//Definition for NET_CS_STATE_IND
#define NET_CS_STATE_IND                                  	0x19                                              	
#define NET_CS_STATE_IND_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define NET_CS_STATE_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define NET_CS_STATE_IND_OFFSET_CSSTATE                   	2 //size 1 byte(s)
#define NET_CS_STATE_IND_OFFSET_CSTYPE                    	3 //size 1 byte(s)
#define NET_CS_STATE_IND_OFFSET_CSOPERATIONMODE           	4 //size 1 byte(s)
#define NET_CS_STATE_IND_OFFSET_FILLERBYTE1               	5 //size 3 byte(s)
#define SIZE_NET_CS_STATE_IND                             	8


//Definition for NET_NEIGHBOUR_CELLS_REQ
#define NET_NEIGHBOUR_CELLS_REQ                           	0x1A                                              	
#define NET_NEIGHBOUR_CELLS_REQ_OFFSET_TRANSID            	0 //size 1 byte(s)
#define NET_NEIGHBOUR_CELLS_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define NET_NEIGHBOUR_CELLS_REQ_OFFSET_CELLINFOTYPE       	2 //size 1 byte(s)
#define SIZE_NET_NEIGHBOUR_CELLS_REQ                      	3


//Definition for NET_NEIGHBOUR_CELLS_RESP
#define NET_NEIGHBOUR_CELLS_RESP                          	0x1B                                              	
#define NET_NEIGHBOUR_CELLS_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define NET_NEIGHBOUR_CELLS_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define NET_NEIGHBOUR_CELLS_RESP_OFFSET_SUCCESSCODE       	2 //size 1 byte(s)
#define NET_NEIGHBOUR_CELLS_RESP_OFFSET_SUBBLOCKCOUNT     	3 //size 1 byte(s)
#define SIZE_NET_NEIGHBOUR_CELLS_RESP                     	4


//Definition for NET_RAT_IND
#define NET_RAT_IND                                       	0x35                                              	
#define NET_RAT_IND_OFFSET_TRANSID                        	0 //size 1 byte(s)
#define NET_RAT_IND_OFFSET_MESSAGEID                      	1 //size 1 byte(s)
#define NET_RAT_IND_OFFSET_FILLERBYTE1                    	2 //size 1 byte(s)
#define NET_RAT_IND_OFFSET_SUBBLOCKCOUNT                  	3 //size 1 byte(s)
#define SIZE_NET_RAT_IND                                  	4


//Definition for NET_RAT_REQ
#define NET_RAT_REQ                                       	0x36                                              	
#define NET_RAT_REQ_OFFSET_TRANSID                        	0 //size 1 byte(s)
#define NET_RAT_REQ_OFFSET_MESSAGEID                      	1 //size 1 byte(s)
#define NET_RAT_REQ_OFFSET_RATTYPE                        	2 //size 1 byte(s)
#define SIZE_NET_RAT_REQ                                  	3


//Definition for NET_RAT_RESP
#define NET_RAT_RESP                                      	0x37                                              	
#define NET_RAT_RESP_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define NET_RAT_RESP_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define NET_RAT_RESP_OFFSET_SUCCESSCODE                   	2 //size 1 byte(s)
#define NET_RAT_RESP_OFFSET_SUBBLOCKCOUNT                 	3 //size 1 byte(s)
#define SIZE_NET_RAT_RESP                                 	4


//Definition for NET_CS_STATE_REQ
#define NET_CS_STATE_REQ                                  	0x3A                                              	
#define NET_CS_STATE_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define NET_CS_STATE_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define SIZE_NET_CS_STATE_REQ                             	2


//Definition for NET_CS_STATE_RESP
#define NET_CS_STATE_RESP                                 	0x3B                                              	
#define NET_CS_STATE_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define NET_CS_STATE_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define NET_CS_STATE_RESP_OFFSET_SUCCESSCODE              	2 //size 1 byte(s)
#define NET_CS_STATE_RESP_OFFSET_CSSTATE                  	3 //size 1 byte(s)
#define NET_CS_STATE_RESP_OFFSET_CSOPERATIONMODE          	4 //size 1 byte(s)
#define SIZE_NET_CS_STATE_RESP                            	5


//Definition for NET_UMA_INFO_IND
#define NET_UMA_INFO_IND                                  	0x3C                                              	
#define NET_UMA_INFO_IND_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define NET_UMA_INFO_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define NET_UMA_INFO_IND_OFFSET_FILLERBYTE1               	2 //size 1 byte(s)
#define NET_UMA_INFO_IND_OFFSET_SUBBLOCKCOUNT             	3 //size 1 byte(s)
#define SIZE_NET_UMA_INFO_IND                             	4


//Definition for NET_RADIO_INFO_IND
#define NET_RADIO_INFO_IND                                	0x3F                                              	
#define NET_RADIO_INFO_IND_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define NET_RADIO_INFO_IND_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define NET_RADIO_INFO_IND_OFFSET_FILLERBYTE1             	2 //size 1 byte(s)
#define NET_RADIO_INFO_IND_OFFSET_SUBBLOCKCOUNT           	3 //size 1 byte(s)
#define SIZE_NET_RADIO_INFO_IND                           	4


//Definition for NET_CHANNEL_INFO_IND
#define NET_CHANNEL_INFO_IND                              	0x2C                                              	
#define NET_CHANNEL_INFO_IND_OFFSET_TRANSID               	0 //size 1 byte(s)
#define NET_CHANNEL_INFO_IND_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define NET_CHANNEL_INFO_IND_OFFSET_FILLERBYTE1           	2 //size 1 byte(s)
#define NET_CHANNEL_INFO_IND_OFFSET_SUBBLOCKCOUNT         	3 //size 1 byte(s)
#define SIZE_NET_CHANNEL_INFO_IND                         	4


//Definition for NET_CHANNEL_INFO_REQ
#define NET_CHANNEL_INFO_REQ                              	0x2D                                              	
#define NET_CHANNEL_INFO_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define NET_CHANNEL_INFO_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SIZE_NET_CHANNEL_INFO_REQ                         	2


//Definition for NET_CHANNEL_INFO_RESP
#define NET_CHANNEL_INFO_RESP                             	0x2E                                              	
#define NET_CHANNEL_INFO_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define NET_CHANNEL_INFO_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define NET_CHANNEL_INFO_RESP_OFFSET_SUCCESSCODE          	2 //size 1 byte(s)
#define NET_CHANNEL_INFO_RESP_OFFSET_SUBBLOCKCOUNT        	3 //size 1 byte(s)
#define SIZE_NET_CHANNEL_INFO_RESP                        	4


//Definition for NET_CELL_INFO_GET_REQ
#define NET_CELL_INFO_GET_REQ                             	0x40                                              	
#define NET_CELL_INFO_GET_REQ_OFFSET_TRANSID              	0 //size 1 byte(s)
#define NET_CELL_INFO_GET_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SIZE_NET_CELL_INFO_GET_REQ                        	2


//Definition for NET_CELL_INFO_GET_RESP
#define NET_CELL_INFO_GET_RESP                            	0x41                                              	
#define NET_CELL_INFO_GET_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define NET_CELL_INFO_GET_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define NET_CELL_INFO_GET_RESP_OFFSET_SUCCESSCODE         	2 //size 1 byte(s)
#define NET_CELL_INFO_GET_RESP_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_NET_CELL_INFO_GET_RESP                       	4


//Definition for NET_CELL_INFO_IND
#define NET_CELL_INFO_IND                                 	0x42                                              	
#define NET_CELL_INFO_IND_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define NET_CELL_INFO_IND_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define NET_CELL_INFO_IND_OFFSET_FILLERBYTE1              	2 //size 1 byte(s)
#define NET_CELL_INFO_IND_OFFSET_SUBBLOCKCOUNT            	3 //size 1 byte(s)
#define SIZE_NET_CELL_INFO_IND                            	4


//Definition for NET_NITZ_NAME_IND
#define NET_NITZ_NAME_IND                                 	0x43                                              	
#define NET_NITZ_NAME_IND_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define NET_NITZ_NAME_IND_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define NET_NITZ_NAME_IND_OFFSET_OPERATORCODE             	2 //size 3 byte(s)
#define NET_NITZ_NAME_IND_OFFSET_FILLERBYTE1              	5 //size 2 byte(s)
#define NET_NITZ_NAME_IND_OFFSET_SUBBLOCKCOUNT            	7 //size 1 byte(s)
#define SIZE_NET_NITZ_NAME_IND                            	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for NET_SOR_REQ
#define NET_SOR_REQ                                       	0x44                                              	
#define NET_SOR_REQ_OFFSET_TRANSID                        	0 //size 1 byte(s)
#define NET_SOR_REQ_OFFSET_MESSAGEID                      	1 //size 1 byte(s)
#define NET_SOR_REQ_OFFSET_LISTTAG                        	2 //size 1 byte(s)
#define NET_SOR_REQ_OFFSET_DATALENGTH                     	3 //size 1 byte(s)
#define NET_SOR_REQ_OFFSET_DATA                           	4 //size 1 byte(s)
#define NET_SOR_REQ_OFFSET_FILLERBYTE1                    	5 //size 1 byte(s)
#define NET_SOR_REQ_OFFSET_FILLERBYTE2                    	6 //size 1 byte(s)
#define NET_SOR_REQ_OFFSET_FILLERBYTE3                    	7 //size 1 byte(s)
#define SIZE_NET_SOR_REQ                                  	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for NET_SOR_RESP
#define NET_SOR_RESP                                      	0x45                                              	
#define NET_SOR_RESP_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define NET_SOR_RESP_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define NET_SOR_RESP_OFFSET_SUCCESSCODE                   	2 //size 1 byte(s)
#define NET_SOR_RESP_OFFSET_FILLERBYTE1                   	3 //size 1 byte(s)
#define SIZE_NET_SOR_RESP                                 	4


//Definition for NET_RSSI_CONF_REQ
#define NET_RSSI_CONF_REQ                                 	0x46                                              	
#define NET_RSSI_CONF_REQ_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define NET_RSSI_CONF_REQ_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define NET_RSSI_CONF_REQ_OFFSET_NBROFSEGMENTS            	2 //size 1 byte(s)
#define NET_RSSI_CONF_REQ_OFFSET_SUBBLOCKCOUNT            	3 //size 1 byte(s)
#define SIZE_NET_RSSI_CONF_REQ                            	4


//Definition for NET_RSSI_CONF_RESP
#define NET_RSSI_CONF_RESP                                	0x47                                              	
#define NET_RSSI_CONF_RESP_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define NET_RSSI_CONF_RESP_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define NET_RSSI_CONF_RESP_OFFSET_SUCCESSCODE             	2 //size 1 byte(s)
#define NET_RSSI_CONF_RESP_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_NET_RSSI_CONF_RESP                           	4

#endif