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
* Name      : PN_PIPE [ 0xD9 ] Resource Shared CM and PN Pipe types
* Version   : 002.006
*
*/


#ifndef _SYMBIAN_ISI_PIPE_SHAREDISI_H_
#define _SYMBIAN_ISI_PIPE_SHAREDISI_H_

#define SHARED_CM_AND_PN_PIPE_TYPES_ISI_VERSION_Z	 2
#define SHARED_CM_AND_PN_PIPE_TYPES_ISI_VERSION_Y	 6


//CONSTANTS
#define PN_MAX_PEP_CTRL_DATA                              	0x0B                                                 	
#define PN_MAX_PIPE_DATA                                  	0x15                                              	
#define PN_MAX_PEP_IND_DATA                               	0xC9                                              	
#define PIPE_ANY_SIZE                                     	0x01                                                 	


//TABLES

//ConstantTable for PIPE_MESSAGE_IDS
#define PNS_PIPE_CREATE_REQ                               	0x00                                                 	
#define PNS_PIPE_CREATE_RESP                              	0x01                                                 	
#define PNS_PIPE_REMOVE_REQ                               	0x02                                                 	
#define PNS_PIPE_REMOVE_RESP                              	0x03                                                 	
#define PNS_PIPE_RESET_REQ                                	0x04                                                 	
#define PNS_PIPE_RESET_RESP                               	0x05                                                 	
#define PNS_PIPE_ENABLE_REQ                               	0x06                                                 	
#define PNS_PIPE_ENABLE_RESP                              	0x07                                                 	
#define PNS_PIPE_REDIRECT_REQ                             	0x08                                                 	
#define PNS_PIPE_REDIRECT_RESP                            	0x09                                                 	
#define PNS_PIPE_DISABLE_REQ                              	0x0A                                                 	
#define PNS_PIPE_DISABLE_RESP                             	0x0B                                                 	
#define PNS_PIPE_DATA                                     	0x20                                              	
#define PNS_PEP_CONNECT_REQ                               	0x40                                              	
#define PNS_PEP_CONNECT_RESP                              	0x41                                              	
#define PNS_PEP_DISCONNECT_REQ                            	0x42                                              	
#define PNS_PEP_DISCONNECT_RESP                           	0x43                                              	
#define PNS_PEP_RESET_REQ                                 	0x44                                              	
#define PNS_PEP_RESET_RESP                                	0x45                                              	
#define PNS_PEP_ENABLE_REQ                                	0x46                                              	
#define PNS_PEP_ENABLE_RESP                               	0x47                                              	
#define PNS_PEP_CTRL_REQ                                  	0x48                                              	
#define PNS_PEP_CTRL_RESP                                 	0x49                                              	
#define PNS_PEP_STATUS_IND_REQ                            	0x4A                                              	
#define PNS_PEP_STATUS_IND_RESP                           	0x4B                                              	
#define PNS_PEP_DISABLE_REQ                               	0x4C                                              	
#define PNS_PEP_DISABLE_RESP                              	0x4D                                              	
#define PNS_PEP_STATUS_IND                                	0x60                                              	
#define PNS_PIPE_CREATED_IND                              	0x61                                              	
#define PNS_PIPE_REMOVED_IND                              	0x62                                              	
#define PNS_PIPE_RESET_IND                                	0x63                                              	
#define PNS_PIPE_ENABLED_IND                              	0x64                                              	
#define PNS_PIPE_REDIRECTED_IND                           	0x65                                              	
#define PNS_PIPE_DISABLED_IND                             	0x66                                              	
#define PN_PIPE_INVALID_HANDLE                            	0xFF                                              	

//ConstantTable for PIPE_MESSAGE_ALIAS_IDS
#define PNS_PIPE_RECREATE_REQ                             	0x08                                                 	
#define PNS_PIPE_RECREATE_RESP                            	0x09                                                 	
#define PNS_PIPE_RECREATED_IND                            	0x65                                              	

//ConstantTable for PIPE_SUBBLOCK_IDS
#define PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE                	0x00                                                 	
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE               	0x01                                                 	
#define PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE              	0x02                                                 	
#define PN_PIPE_SB_NEGOTIATED_FC                          	0x03                                                 	
#define PN_PIPE_SB_REQUIRED_FC_TX                         	0x04                                                 	
#define PN_PIPE_SB_PREFERRED_FC_RX                        	0x05                                                 	

//ConstantTable for PIPE_FC_TYPES
#define PN_NO_FLOW_CONTROL                                	0x00                                                 	
#define PN_LEGACY_FLOW_CONTROL                            	0x01                                                 	
#define PN_ONE_CREDIT_FLOW_CONTROL                        	0x02                                                 	
#define PN_MULTI_CREDIT_FLOW_CONTROL                      	0x03                                                 	

//ConstantTable for PIPE_MSG_PRIORITY
#define PN_MSG_PRIORITY_LOW                               	0x01                                                 	
#define PN_MSG_PRIORITY_HIGH                              	0x02                                                 	

//ConstantTable for PN_PEP_TYPE
#define PN_PEP_TYPE_COMMON                                	0x00                                                 	
#define PN_PEP_TYPE_COMM                                  	0x01                                                 	
#define PN_PEP_TYPE_SOCKET_BEARER                         	0x02                                                 	
#define PN_PEP_TYPE_DCS                                   	0x03                                                 	
#define PN_PEP_TYPE_GPRS                                  	0x04                                                 	
#define PN_PEP_TYPE_OBEX                                  	0x05                                                 	
#define PN_PEP_TYPE_PRINT                                 	0x06                                                 	
#define PN_PEP_TYPE_TCH                                   	0x07                                                 	
#define PN_PEP_TYPE_GAMES                                 	0x08                                                 	
#define PN_PEP_TYPE_DATA_BEARER                           	0x09                                                 	
#define PN_PEP_TYPE_TCPIP                                 	0x0B                                                 	
#define PN_PEP_TYPE_RAN_SYNC                              	0x0C                                                 	
#define PN_PEP_TYPE_BT_ACC                                	0x0D                                                 	
#define PN_PEP_TYPE_TLP                                   	0x0E                                                 	
#define PN_PEP_TYPE_SAP                                   	0x10                                              	
#define PN_PEP_TYPE_COMMON_ONE_CREDIT                     	0x11                                              	
#define PN_PEP_TYPE_BT_HFP_HF                             	0x12                                              	
#define PN_PEP_TYPE_LCIF                                  	0x13                                              	
#define PN_PEP_TYPE_FLUSH                                 	0x14                                              	
#define PN_PEP_TYPE_ETHERNET                              	0x15                                              	
#define PN_PEP_TYPE_MTP                                   	0x16                                              	

//ConstantTable for PN_PEP_TYPE_COMMON_SUB
#define PEP_IND_EMPTY                                     	0x00                                                 	
#define PEP_IND_BUSY                                      	0x01                                                 	
#define PEP_IND_READY                                     	0x02                                                 	
#define PEP_IND_REMOVE                                    	0x03                                                 	
#define PEP_IND_CREDIT                                    	0x04                                                 	

//ConstantTable for PN_PEP_TYPE_COMM_SUB
#define PN_PEP_SUB_TYPE_COMM_IRDA                         	0x01                                                 	
#define PN_PEP_SUB_TYPE_COMM_RS232                        	0x02                                                 	
#define PN_PEP_SUB_TYPE_COMM_BLUETOOTH                    	0x03                                                 	
#define PN_PEP_SUB_TYPE_USB_AT_COMMAND_N                  	0x04                                                 	
#define PN_PEP_SUB_TYPE_USB_AT_COMMAND_U                  	0x05                                                 	
#define PN_PEP_SUB_TYPE_USB_MODEM_CTRL_N                  	0x06                                                 	
#define PN_PEP_SUB_TYPE_USB_MODEM_CTRL_U                  	0x07                                                 	
#define PN_PEP_SUB_TYPE_USB_MODEM_DATA_N                  	0x08                                                 	
#define PN_PEP_SUB_TYPE_USB_MODEM_DATA_U                  	0x09                                                 	
#define PN_PEP_SUB_TYPE_SYMBIAN_INTERNAL                  	0x0A                                                 	
#define PN_PEP_SUB_TYPE_COMM_VIDEO_TELEPHONY              	0x0B                                                 	
#define PN_PEP_SUB_TYPE_COMM_ATK_AT_COMMAND               	0x0C                                                 	
#define PN_PEP_SUB_TYPE_OBEX_BT_PBAP                      	0x0D                                                 	
#define PN_PEP_SUB_TYPE_OBEX_BT_BPP_DPS                   	0x0E                                                 	
#define PN_PEP_SUB_TYPE_OBEX_BT_BIP_IMAGE_PUSH            	0x0F                                                 	
#define PN_PEP_SUB_TYPE_USB_RNDIS_CTRL                    	0x10                                              	
#define PN_PEP_SUB_TYPE_USB_RNDIS_DATA                    	0x11                                              	

//ConstantTable for PN_PEP_TYPE_DCS_SUB
#define PN_PEP_SUB_TYPE_GSM_DCS                           	0x01                                                 	
#define PN_PEP_SUB_TYPE_PCN_DCS                           	0x02                                                 	
#define PN_PEP_SUB_TYPE_PDC_DCS                           	0x03                                                 	
#define PN_PEP_SUB_TYPE_TDMA_DCS                          	0x04                                                 	
#define PN_PEP_SUB_TYPE_GUTS_DCS                          	0x05                                                 	

//ConstantTable for PN_PEP_TYPE_GPRS_SUB
#define PN_PEP_SUB_TYPE_GPRS                              	0x01                                                 	

//ConstantTable for PN_PEP_TYPE_OBEX_SUB
#define PN_PEP_SUB_TYPE_OBEX_USB                          	0x01                                                 	
#define PN_PEP_SUB_TYPE_OBEX_BT_OPP                       	0x02                                                 	
#define PN_PEP_SUB_TYPE_OBEX_BT_FTP                       	0x03                                                 	
#define PN_PEP_SUB_TYPE_OBEX_BT_SYNC                      	0x04                                                 	
#define PN_PEP_SUB_TYPE_OBEX_RS232                        	0x05                                                 	
#define PN_PEP_SUB_TYPE_OBEX_IRDA                         	0x06                                                 	
#define PN_PEP_SUB_TYPE_OBEX_PCSUITE_IRDA                 	0x07                                                 	
#define PN_PEP_SUB_TYPE_OBEX_PCSUITE_BT                   	0x08                                                 	
#define PN_PEP_SUB_TYPE_OBEX_PCSUITE_USB                  	0x09                                                 	
#define PN_PEP_SUB_TYPE_OBEX_PCSUITE_FBUS                 	0x0A                                                 	
#define PN_PEP_SUB_TYPE_OBEX_ODIN                         	0x0B                                                 	
#define PN_PEP_SUB_TYPE_OBEX_BT_IRMC_SYNC                 	0x0C                                                 	

//ConstantTable for PN_PEP_OTHER_CONSTANTS
#define PN_PEP_IND_DATA_SIZE                              	0x01                                                 	
#define PN_PEP_IND_ALIVE                                  	0x02                                                 	

//ConstantTable for PN_PEP_TYPE_TCH_SUB
#define PN_PEP_SUB_TYPE_GSM_TCH                           	0x01                                                 	

//ConstantTable for PN_PEP_TYPE_TCPIP_SUB
#define PN_PEP_SUB_TYPE_TCPIP_BLUETOOTH                   	0x01                                                 	

//ConstantTable for PN_PEP_TYPE_BT_ACC_SUB
#define PN_PEP_SUB_TYPE_BT_ACC_HS                         	0x01                                                 	
#define PN_PEP_SUB_TYPE_BT_ACC_HF                         	0x02                                                 	

//ConstantTable for PN_PEP_TYPE_TLP_SUB
#define PN_PEP_SUB_TYPE_TLPC                              	0x01                                                 	
#define PN_PEP_SUB_TYPE_TLPU_ASYNC                        	0x02                                                 	
#define PN_PEP_SUB_TYPE_TLPU_SYNC                         	0x03                                                 	
#define PN_PEP_SUB_TYPE_OMT                               	0x04                                                 	

//ConstantTable for PN_PEP_TYPE_ETHERNET_SUB
#define PN_PEP_SUB_TYPE_ETHERNET_WLAN                     	0x01                                                 	
#define PN_PEP_SUB_TYPE_ETHERNET_BT_PAN                   	0x02                                                 	

//ConstantTable for PN_PEP_TYPE_MTP_SUB
#define PN_PEP_SUB_TYPE_MTP_OVER_BT                       	0x01                                                 	
#define PN_PEP_SUB_TYPE_MTP_OVER_USB                      	0x02                                                 	

//ConstantTable for PN_PEP_IND_ENUM
#define PN_PEP_IND_FLOW_CONTROL                           	0x00                                                 	
#define PN_PEP_IND_MCFC_GRANT_CREDITS                     	0x01                                                 	

//ConstantTable for PIPE_SERVICE_CC
#define PN_PIPE_NO_ERROR                                  	0x00                                                 	
#define PN_PIPE_ERR_INVALID_PARAM                         	0x01                                                 	
#define PN_PIPE_ERR_INVALID_HANDLE                        	0x02                                                 	
#define PN_PIPE_ERR_INVALID_CTRL_ID                       	0x03                                                 	
#define PN_PIPE_ERR_NOT_ALLOWED                           	0x04                                                 	
#define PN_PIPE_ERR_PEP_IN_USE                            	0x05                                                 	
#define PN_PIPE_ERR_OVERLOAD                              	0x06                                                 	
#define PN_PIPE_ERR_DEV_DISCONNECTED                      	0x07                                                 	
#define PN_PIPE_ERR_TIMEOUT                               	0x08                                                 	
#define PN_PIPE_ERR_ALL_PIPES_IN_USE                      	0x09                                                 	
#define PN_PIPE_ERR_GENERAL                               	0x0A                                                 	
#define PN_PIPE_ERR_NOT_SUPPORTED                         	0x0B                                                 	

//ConstantTable for PIPE_STATE_AFTER
#define PN_PIPE_DISABLE                                   	0x00                                                 	
#define PN_PIPE_ENABLE                                    	0x01                                                 	

//ConstantTable for PEP_STATE_AFTER
#define PN_PEP_DISABLE                                    	0x00                                                 	
#define PN_PEP_ENABLE                                     	0x01                                                 	


//SUBBLOCKS


//Definition for PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE
//#define PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE              	0x00                   	 //Already defined as 0x00
#define PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE_OFFSET_SBID    	0 //size 1 byte(s)
#define PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE_OFFSET_SBLEN   	1 //size 1 byte(s)
#define PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE_OFFSET_FIRSTPEPSUBTYPE	2 //size 1 byte(s)
#define PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE_OFFSET_SECONDPEPSUBTYPE	3 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_CREATE_REQ_PEP_SUB_TYPE           	4


//Definition for PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE
//#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE             	0x01                   	 //Already defined as 0x01
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_SBID   	0 //size 1 byte(s)
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_SBLEN  	1 //size 1 byte(s)
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_PEPSUBTYPE	2 //size 1 byte(s)
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE          	4


//Definition for PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE
//#define PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE            	0x02                   	 //Already defined as 0x02
#define PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE_OFFSET_SBID  	0 //size 1 byte(s)
#define PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE_OFFSET_SBLEN 	1 //size 1 byte(s)
#define PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE_OFFSET_OLDPEPSUBTYPE	2 //size 1 byte(s)
#define PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE_OFFSET_REPLACEMENTPEPSUBTYPE	3 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_REDIRECT_REQ_PEP_SUB_TYPE         	4


//Definition for PN_PIPE_SB_NEGOTIATED_FC
//#define PN_PIPE_SB_NEGOTIATED_FC                        	0x03                   	 //Already defined as 0x03
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_SBID              	0 //size 1 byte(s)
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_SBLEN             	1 //size 1 byte(s)
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_NEGOTIATEDTXFC    	2 //size 1 byte(s)
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_NEGOTIATEDRXFC    	3 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_NEGOTIATED_FC                     	4


//Definition for PN_PIPE_SB_REQUIRED_FC_TX
//#define PN_PIPE_SB_REQUIRED_FC_TX                       	0x04                   	 //Already defined as 0x04
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_SBID             	0 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_SBLEN            	1 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTXLENGTH	2 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTX     	4 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_REQUIRED_FC_TX                    	5
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PN_PIPE_SB_PREFERRED_FC_RX
//#define PN_PIPE_SB_PREFERRED_FC_RX                      	0x05                   	 //Already defined as 0x05
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_SBID            	0 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_SBLEN           	1 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRXLENGTH	2 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_FILLERBYTE1     	3 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRX   	4 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_PREFERRED_FC_RX                   	5
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.

#endif