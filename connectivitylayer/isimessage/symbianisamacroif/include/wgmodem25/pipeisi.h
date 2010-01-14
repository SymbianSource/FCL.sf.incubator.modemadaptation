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
* Name      : PN_PIPE [ 0xD9 ] Resource CM Pipe Service
* Version   : 002.003
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_PIPEISI_H_
#define _SYMBIAN_ISI_PIPEISI_H_

#define PIPE_SERVICE_ISI_VERSION_Z	 2
#define PIPE_SERVICE_ISI_VERSION_Y	 7


//CONSTANTS
#define PN_MAX_PEP_CTRL_DATA                              	0x0B                                                 	
#define PN_MAX_PIPE_DATA                                  	0x15                                              	
#define PN_MAX_PEP_IND_DATA                               	0xC9                                              	
#define PIPE_ANY_SIZE                                     	0x01                                                 	


//TABLES

//ConstantTable for PIPE_FC_TYPES
#define PN_NO_FLOW_CONTROL                                	0x00                                                 	
#define PN_LEGACY_FLOW_CONTROL                            	0x01                                                 	
#define PN_ONE_CREDIT_FLOW_CONTROL                        	0x02                                                 	
#define PN_MULTI_CREDIT_FLOW_CONTROL                      	0x03                                                 	

//ConstantTable for PIPE_DATA_MSG_TYPES
#define PN_NORMAL_DATA_MSG                                	0x00                                                 	
#define PN_ALIGNED_DATA_MSG                               	0x01                                                 	

//ConstantTable for PN_PEP_TYPE
#define PN_PEP_TYPE_COMMON                                	0x00                                                 	

//ConstantTable for PN_PEP_TYPE_COMMON_SUB
#define PEP_IND_EMPTY                                     	0x00                                                 	
#define PEP_IND_BUSY                                      	0x01                                                 	
#define PEP_IND_READY                                     	0x02                                                 	
#define PEP_IND_REMOVE                                    	0x03                                                 	
#define PEP_IND_CREDIT                                    	0x04                                                 	

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
#define PN_PIPE_ERR_ALIGN_NOT_SUPPORTED                   	0x0B                                                 	
#define PN_PIPE_ERR_NOT_SUPPORTED                         	0x0C                                                 	

//ConstantTable for PN_PEP_IND_ENUM
#define PN_PEP_IND_FLOW_CONTROL                           	0x00                                                 	
#define PN_PEP_IND_MCFC_GRANT_CREDITS                     	0x01                                                 	

//ConstantTable for PIPE_STATE_AFTER
#define PN_PIPE_DISABLE                                   	0x00                                                 	
#define PN_PIPE_ENABLE                                    	0x01                                                 	


//STRUCTURES


//Definition for PIPE_MSG_HDR
#define PIPE_MSG_HDR_OFFSET_MEDIA                         	0 //size 1 byte(s)
#define PIPE_MSG_HDR_OFFSET_RECEIVERDEV                   	1 //size 1 byte(s)
#define PIPE_MSG_HDR_OFFSET_SENDERDEV                     	2 //size 1 byte(s)
#define PIPE_MSG_HDR_OFFSET_FUNCTION                      	3 //size 1 byte(s)
#define PIPE_MSG_HDR_OFFSET_LENGTH                        	4 //size 2 byte(s)
#define PIPE_MSG_HDR_OFFSET_RECEIVEROBJ                   	6 //size 1 byte(s)
#define PIPE_MSG_HDR_OFFSET_SENDEROBJ                     	7 //size 1 byte(s)
#define SIZE_PIPE_MSG_HDR                                 	8


//Definition for GENERAL_PIPE_MSG
#define GENERAL_PIPE_MSG_OFFSET_HEADER                    	0 //size 8 byte(s)
#define GENERAL_PIPE_MSG_OFFSET_UTID                      	8 //size 1 byte(s)
#define GENERAL_PIPE_MSG_OFFSET_SUBFUNCTION               	9 //size 1 byte(s)
#define SIZE_GENERAL_PIPE_MSG                             	10
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//SUBBLOCKS


//Definition for PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE               	0x01                                                 	
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_SBID   	0 //size 1 byte(s)
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_SBLEN  	1 //size 1 byte(s)
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_PEPSUBTYPE	2 //size 1 byte(s)
#define PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_CONNECT_REQ_PEP_SUB_TYPE          	4


//Definition for PN_PIPE_SB_NEGOTIATED_FC
#define PN_PIPE_SB_NEGOTIATED_FC                          	0x03                                                 	
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_SBID              	0 //size 1 byte(s)
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_SBLEN             	1 //size 1 byte(s)
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_NEGOTIATEDTXFC    	2 //size 1 byte(s)
#define PN_PIPE_SB_NEGOTIATED_FC_OFFSET_NEGOTIATEDRXFC    	3 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_NEGOTIATED_FC                     	4


//Definition for PN_PIPE_SB_REQUIRED_FC_TX
#define PN_PIPE_SB_REQUIRED_FC_TX                         	0x04                                                 	
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_SBID             	0 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_SBLEN            	1 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTXLENGTH	2 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_REQUIREDFCTX     	4 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_FILLERBYTE2      	5 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_FILLERBYTE3      	6 //size 1 byte(s)
#define PN_PIPE_SB_REQUIRED_FC_TX_OFFSET_FILLERBYTE4      	7 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_REQUIRED_FC_TX                    	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PN_PIPE_SB_PREFERRED_FC_RX
#define PN_PIPE_SB_PREFERRED_FC_RX                        	0x05                                                 	
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_SBID            	0 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_SBLEN           	1 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRXLENGTH	2 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_FILLERBYTE1     	3 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_PREFERREDFCRX   	4 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_FILLERBYTE2     	5 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_FILLERBYTE3     	6 //size 1 byte(s)
#define PN_PIPE_SB_PREFERRED_FC_RX_OFFSET_FILLERBYTE4     	7 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_PREFERRED_FC_RX                   	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PN_PIPE_SB_ALIGNED_DATA
#define PN_PIPE_SB_ALIGNED_DATA                           	0x06                                                 	
#define PN_PIPE_SB_ALIGNED_DATA_OFFSET_SBID               	0 //size 1 byte(s)
#define PN_PIPE_SB_ALIGNED_DATA_OFFSET_SBLEN              	1 //size 1 byte(s)
#define PN_PIPE_SB_ALIGNED_DATA_OFFSET_DATAALIGNMENT      	2 //size 1 byte(s)
#define PN_PIPE_SB_ALIGNED_DATA_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_PN_PIPE_SB_ALIGNED_DATA                      	4


//MESSAGES


//Definition for PNS_PIPE_CREATED_IND
#define PNS_PIPE_CREATED_IND                              	0x61                                              	
#define PNS_PIPE_CREATED_IND_OFFSET_UTID                  	0 //size 1 byte(s)
#define PNS_PIPE_CREATED_IND_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define PNS_PIPE_CREATED_IND_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define PNS_PIPE_CREATED_IND_OFFSET_NSB                   	3 //size 1 byte(s)
#define SIZE_PNS_PIPE_CREATED_IND                         	4


//Definition for PNS_PEP_CONNECT_REQ
#define PNS_PEP_CONNECT_REQ                               	0x40                                              	
#define PNS_PEP_CONNECT_REQ_OFFSET_UTID                   	0 //size 1 byte(s)
#define PNS_PEP_CONNECT_REQ_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define PNS_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define PNS_PEP_CONNECT_REQ_OFFSET_STATEAFTERCONNECT      	3 //size 1 byte(s)
#define PNS_PEP_CONNECT_REQ_OFFSET_OTHERPEPTYPE           	4 //size 1 byte(s)
#define PNS_PEP_CONNECT_REQ_OFFSET_FILLERBYTE1            	5 //size 1 byte(s)
#define PNS_PEP_CONNECT_REQ_OFFSET_FILLERBYTE2            	6 //size 1 byte(s)
#define PNS_PEP_CONNECT_REQ_OFFSET_NSB                    	7 //size 1 byte(s)
#define SIZE_PNS_PEP_CONNECT_REQ                          	8


//Definition for PNS_PEP_CONNECT_RESP
#define PNS_PEP_CONNECT_RESP                              	0x41                                              	
#define PNS_PEP_CONNECT_RESP_OFFSET_UTID                  	0 //size 1 byte(s)
#define PNS_PEP_CONNECT_RESP_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define PNS_PEP_CONNECT_RESP_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define PNS_PEP_CONNECT_RESP_OFFSET_ERRORCODE             	3 //size 1 byte(s)
#define PNS_PEP_CONNECT_RESP_OFFSET_FILLERBYTE1           	4 //size 1 byte(s)
#define PNS_PEP_CONNECT_RESP_OFFSET_FILLERBYTE2           	5 //size 1 byte(s)
#define PNS_PEP_CONNECT_RESP_OFFSET_FILLERBYTE3           	6 //size 1 byte(s)
#define PNS_PEP_CONNECT_RESP_OFFSET_NSB                   	7 //size 1 byte(s)
#define SIZE_PNS_PEP_CONNECT_RESP                         	8


//Definition for PNS_PEP_CTRL_REQ
#define PNS_PEP_CTRL_REQ                                  	0x48                                              	
#define PNS_PEP_CTRL_REQ_OFFSET_UTID                      	0 //size 1 byte(s)
#define PNS_PEP_CTRL_REQ_OFFSET_SUBFUNCTION               	1 //size 1 byte(s)
#define PNS_PEP_CTRL_REQ_OFFSET_PIPEHANDLE                	2 //size 1 byte(s)
#define PNS_PEP_CTRL_REQ_OFFSET_PEPTYPE                   	3 //size 1 byte(s)
#define PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLID                 	4 //size 1 byte(s)
#define PNS_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA               	5 //size 1 byte(s)
#define SIZE_PNS_PEP_CTRL_REQ                             	6
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PNS_PEP_CTRL_RESP
#define PNS_PEP_CTRL_RESP                                 	0x49                                              	
#define PNS_PEP_CTRL_RESP_OFFSET_UTID                     	0 //size 1 byte(s)
#define PNS_PEP_CTRL_RESP_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define PNS_PEP_CTRL_RESP_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define PNS_PEP_CTRL_RESP_OFFSET_PEPCTRLID                	3 //size 1 byte(s)
#define PNS_PEP_CTRL_RESP_OFFSET_PEPTYPE                  	4 //size 1 byte(s)
#define PNS_PEP_CTRL_RESP_OFFSET_ERRORCODE                	5 //size 1 byte(s)
#define PNS_PEP_CTRL_RESP_OFFSET_FILLERBYTE1              	6 //size 1 byte(s)
#define PNS_PEP_CTRL_RESP_OFFSET_FILLERBYTE2              	7 //size 1 byte(s)
#define SIZE_PNS_PEP_CTRL_RESP                            	8


//Definition for PNS_PIPE_ENABLED_IND
#define PNS_PIPE_ENABLED_IND                              	0x64                                              	
#define PNS_PIPE_ENABLED_IND_OFFSET_UTID                  	0 //size 1 byte(s)
#define PNS_PIPE_ENABLED_IND_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define PNS_PIPE_ENABLED_IND_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define PNS_PIPE_ENABLED_IND_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_PNS_PIPE_ENABLED_IND                         	4


//Definition for PNS_PEP_ENABLE_REQ
#define PNS_PEP_ENABLE_REQ                                	0x46                                              	
#define PNS_PEP_ENABLE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define PNS_PEP_ENABLE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define PNS_PEP_ENABLE_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define PNS_PEP_ENABLE_REQ_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_PNS_PEP_ENABLE_REQ                           	4


//Definition for PNS_PEP_ENABLE_RESP
#define PNS_PEP_ENABLE_RESP                               	0x47                                              	
#define PNS_PEP_ENABLE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define PNS_PEP_ENABLE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define PNS_PEP_ENABLE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define PNS_PEP_ENABLE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define SIZE_PNS_PEP_ENABLE_RESP                          	4


//Definition for PNS_PIPE_DATA
#define PNS_PIPE_DATA                                     	0x20                                              	
#define PNS_PIPE_DATA_OFFSET_UTID                         	0 //size 1 byte(s)
#define PNS_PIPE_DATA_OFFSET_SUBFUNCTION                  	1 //size 1 byte(s)
#define PNS_PIPE_DATA_OFFSET_PIPEHANDLE                   	2 //size 1 byte(s)
#define PNS_PIPE_DATA_OFFSET_DATA                         	3 //size 1 byte(s)
#define SIZE_PNS_PIPE_DATA                                	4
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PNS_PIPE_ALIGNED_DATA
#define PNS_PIPE_ALIGNED_DATA                             	0x21                                              	
#define PNS_PIPE_ALIGNED_DATA_OFFSET_UTID                 	0 //size 1 byte(s)
#define PNS_PIPE_ALIGNED_DATA_OFFSET_SUBFUNCTION          	1 //size 1 byte(s)
#define PNS_PIPE_ALIGNED_DATA_OFFSET_PIPEHANDLE           	2 //size 1 byte(s)
#define PNS_PIPE_ALIGNED_DATA_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define PNS_PIPE_ALIGNED_DATA_OFFSET_DATA                 	4 //size 1 byte(s)
#define SIZE_PNS_PIPE_ALIGNED_DATA                        	5
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PNS_PEP_STATUS_IND
#define PNS_PEP_STATUS_IND                                	0x60                                              	
#define PNS_PEP_STATUS_IND_OFFSET_UTID                    	0 //size 1 byte(s)
#define PNS_PEP_STATUS_IND_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define PNS_PEP_STATUS_IND_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define PNS_PEP_STATUS_IND_OFFSET_PEPTYPE                 	3 //size 1 byte(s)
#define PNS_PEP_STATUS_IND_OFFSET_INDICATIONID            	4 //size 1 byte(s)
#define PNS_PEP_STATUS_IND_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define PNS_PEP_STATUS_IND_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define PNS_PEP_STATUS_IND_OFFSET_INDICATIONDATA          	7 //size 1 byte(s)
#define SIZE_PNS_PEP_STATUS_IND                           	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PNS_PIPE_RESET_IND
#define PNS_PIPE_RESET_IND                                	0x63                                              	
#define PNS_PIPE_RESET_IND_OFFSET_UTID                    	0 //size 1 byte(s)
#define PNS_PIPE_RESET_IND_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define PNS_PIPE_RESET_IND_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define PNS_PIPE_RESET_IND_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_PNS_PIPE_RESET_IND                           	4


//Definition for PNS_PEP_RESET_REQ
#define PNS_PEP_RESET_REQ                                 	0x44                                              	
#define PNS_PEP_RESET_REQ_OFFSET_UTID                     	0 //size 1 byte(s)
#define PNS_PEP_RESET_REQ_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define PNS_PEP_RESET_REQ_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define PNS_PEP_RESET_REQ_OFFSET_PIPESTATEAFTERRESET      	3 //size 1 byte(s)
#define SIZE_PNS_PEP_RESET_REQ                            	4


//Definition for PNS_PEP_RESET_RESP
#define PNS_PEP_RESET_RESP                                	0x45                                              	
#define PNS_PEP_RESET_RESP_OFFSET_UTID                    	0 //size 1 byte(s)
#define PNS_PEP_RESET_RESP_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define PNS_PEP_RESET_RESP_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define PNS_PEP_RESET_RESP_OFFSET_ERRORCODE               	3 //size 1 byte(s)
#define SIZE_PNS_PEP_RESET_RESP                           	4


//Definition for PNS_PEP_DISCONNECT_REQ
#define PNS_PEP_DISCONNECT_REQ                            	0x42                                              	
#define PNS_PEP_DISCONNECT_REQ_OFFSET_UTID                	0 //size 1 byte(s)
#define PNS_PEP_DISCONNECT_REQ_OFFSET_SUBFUNCTION         	1 //size 1 byte(s)
#define PNS_PEP_DISCONNECT_REQ_OFFSET_PIPEHANDLE          	2 //size 1 byte(s)
#define PNS_PEP_DISCONNECT_REQ_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_PNS_PEP_DISCONNECT_REQ                       	4


//Definition for PNS_PEP_DISCONNECT_RESP
#define PNS_PEP_DISCONNECT_RESP                           	0x43                                              	
#define PNS_PEP_DISCONNECT_RESP_OFFSET_UTID               	0 //size 1 byte(s)
#define PNS_PEP_DISCONNECT_RESP_OFFSET_SUBFUNCTION        	1 //size 1 byte(s)
#define PNS_PEP_DISCONNECT_RESP_OFFSET_PIPEHANDLE         	2 //size 1 byte(s)
#define PNS_PEP_DISCONNECT_RESP_OFFSET_ERRORCODE          	3 //size 1 byte(s)
#define SIZE_PNS_PEP_DISCONNECT_RESP                      	4


//Definition for PNS_PIPE_REDIRECTED_IND
#define PNS_PIPE_REDIRECTED_IND                           	0x65                                              	
#define PNS_PIPE_REDIRECTED_IND_OFFSET_UTID               	0 //size 1 byte(s)
#define PNS_PIPE_REDIRECTED_IND_OFFSET_SUBFUNCTION        	1 //size 1 byte(s)
#define PNS_PIPE_REDIRECTED_IND_OFFSET_PIPEHANDLE         	2 //size 1 byte(s)
#define PNS_PIPE_REDIRECTED_IND_OFFSET_NSB                	3 //size 1 byte(s)
#define SIZE_PNS_PIPE_REDIRECTED_IND                      	4


//Definition for PNS_PEP_DISABLE_REQ
#define PNS_PEP_DISABLE_REQ                               	0x4C                                              	
#define PNS_PEP_DISABLE_REQ_OFFSET_UTID                   	0 //size 1 byte(s)
#define PNS_PEP_DISABLE_REQ_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define PNS_PEP_DISABLE_REQ_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define PNS_PEP_DISABLE_REQ_OFFSET_FILLERBYTE1            	3 //size 1 byte(s)
#define SIZE_PNS_PEP_DISABLE_REQ                          	4


//Definition for PNS_PEP_DISABLE_RESP
#define PNS_PEP_DISABLE_RESP                              	0x4D                                              	
#define PNS_PEP_DISABLE_RESP_OFFSET_UTID                  	0 //size 1 byte(s)
#define PNS_PEP_DISABLE_RESP_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define PNS_PEP_DISABLE_RESP_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define PNS_PEP_DISABLE_RESP_OFFSET_ERRORCODE             	3 //size 1 byte(s)
#define SIZE_PNS_PEP_DISABLE_RESP                         	4

/* CL Pipe defines */
//CONSTANTS
#define CM_PIPE_ANY_SIZE                                  	0x01                                                 	


//STRUCTURES


//Definition for CM_PIPE_CREATE_REQ
#define CM_PIPE_CREATE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_STATEAFTERCREATION      	2 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_PRIORITY                	3 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FIRSTPEPDEV             	4 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FIRSTPEPOBJ             	5 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FIRSTPEPTYPE            	6 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FILLERBYTE1             	7 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SECONDPEPDEV            	8 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SECONDPEPOBJ            	9 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SECONDPEPTYPE           	10 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_NSB                     	11 //size 1 byte(s)
#define SIZE_CM_PIPE_CREATE_REQ                           	12


//Definition for CM_PIPE_CREATE_RESP
#define CM_PIPE_CREATE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_CREATE_RESP                          	6


//Definition for CM_PIPE_CREATED_IND
#define CM_PIPE_CREATED_IND_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_CREATED_IND_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_CREATED_IND_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_CREATED_IND_OFFSET_NSB                    	3 //size 1 byte(s)
#define SIZE_CM_PIPE_CREATED_IND                          	4


//Definition for CM_PEP_CONNECT_REQ
#define CM_PEP_CONNECT_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_STATEAFTERCONNECT       	3 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_OTHERPEPTYPE            	4 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_NSB                     	7 //size 1 byte(s)
#define SIZE_CM_PEP_CONNECT_REQ                           	8


//Definition for CM_PEP_CONNECT_RESP
#define CM_PEP_CONNECT_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_FILLERBYTE1            	4 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_FILLERBYTE2            	5 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_FILLERBYTE3            	6 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_NSB                    	7 //size 1 byte(s)
#define SIZE_CM_PEP_CONNECT_RESP                          	8


//Definition for CM_PEP_CTRL_REQ
#define CM_PEP_CTRL_REQ_OFFSET_UTID                       	0 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_SUBFUNCTION                	1 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PIPEHANDLE                 	2 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PEPTYPE                    	3 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PEPCTRLID                  	4 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA                	5 //size 1 byte(s)
#define SIZE_CM_PEP_CTRL_REQ                              	6
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CM_PEP_CTRL_RESP
#define CM_PEP_CTRL_RESP_OFFSET_UTID                      	0 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_SUBFUNCTION               	1 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_PIPEHANDLE                	2 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_PEPCTRLID                 	3 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_PEPTYPE                   	4 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_ERRORCODE                 	5 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_FILLERBYTE1               	6 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_FILLERBYTE2               	7 //size 1 byte(s)
#define SIZE_CM_PEP_CTRL_RESP                             	8


//Definition for CM_PIPE_ENABLE_REQ
#define CM_PIPE_ENABLE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_ENABLE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_ENABLE_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PIPE_ENABLE_REQ_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_CM_PIPE_ENABLE_REQ                           	4


//Definition for CM_PIPE_ENABLE_RESP
#define CM_PIPE_ENABLE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_ENABLE_RESP                          	6


//Definition for CM_PIPE_ENABLED_IND
#define CM_PIPE_ENABLED_IND_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_ENABLED_IND_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_ENABLED_IND_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_ENABLED_IND_OFFSET_FILLERBYTE1            	3 //size 1 byte(s)
#define SIZE_CM_PIPE_ENABLED_IND                          	4


//Definition for CM_PEP_ENABLE_REQ
#define CM_PEP_ENABLE_REQ_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PEP_ENABLE_REQ_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PEP_ENABLE_REQ_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PEP_ENABLE_REQ_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_CM_PEP_ENABLE_REQ                            	4


//Definition for CM_PEP_ENABLE_RESP
#define CM_PEP_ENABLE_RESP_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PEP_ENABLE_RESP_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PEP_ENABLE_RESP_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PEP_ENABLE_RESP_OFFSET_ERRORCODE               	3 //size 1 byte(s)
#define SIZE_CM_PEP_ENABLE_RESP                           	4


//Definition for CM_PIPE_DATA
#define CM_PIPE_DATA_OFFSET_UTID                          	0 //size 1 byte(s)
#define CM_PIPE_DATA_OFFSET_SUBFUNCTION                   	1 //size 1 byte(s)
#define CM_PIPE_DATA_OFFSET_PIPEHANDLE                    	2 //size 1 byte(s)
#define CM_PIPE_DATA_OFFSET_DATA                          	3 //size 1 byte(s)
#define SIZE_CM_PIPE_DATA                                 	4
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CM_PIPE_REMOVED_IND
#define CM_PIPE_REMOVED_IND_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_REMOVED_IND                          	6


//Definition for CM_PIPE_STATUS_IND_REQ
#define CM_PIPE_STATUS_IND_REQ_OFFSET_UTID                	0 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_REQ_OFFSET_SUBFUNCTION         	1 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_REQ_OFFSET_PIPEHANDLE          	2 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_REQ_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_CM_PIPE_STATUS_IND_REQ                       	4


//Definition for CM_PIPE_STATUS_IND_RESP
#define CM_PIPE_STATUS_IND_RESP_OFFSET_UTID               	0 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_RESP_OFFSET_SUBFUNCTION        	1 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_RESP_OFFSET_PIPEHANDLE         	2 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_RESP_OFFSET_ERRORCODE          	3 //size 1 byte(s)
#define SIZE_CM_PIPE_STATUS_IND_RESP                      	4


//Definition for CM_PEP_STATUS_IND
#define CM_PEP_STATUS_IND_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_PEPTYPE                  	3 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_INDICATIONID             	4 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_FILLERBYTE1              	5 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_FILLERBYTE2              	6 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_INDICATIONDATA           	7 //size 1 byte(s)
#define SIZE_CM_PEP_STATUS_IND                            	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CM_PIPE_RESET_REQ
#define CM_PIPE_RESET_REQ_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PIPE_RESET_REQ_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PIPE_RESET_REQ_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PIPE_RESET_REQ_OFFSET_PIPESTATEAFTERRESET      	3 //size 1 byte(s)
#define SIZE_CM_PIPE_RESET_REQ                            	4


//Definition for CM_PIPE_RESET_RESP
#define CM_PIPE_RESET_RESP_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_ERRORCODE               	3 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_PEP1ERRORCODE           	4 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_PEP2ERRORCODE           	5 //size 1 byte(s)
#define SIZE_CM_PIPE_RESET_RESP                           	6


//Definition for CM_PIPE_RESET_IND
#define CM_PIPE_RESET_IND_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PIPE_RESET_IND_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PIPE_RESET_IND_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PIPE_RESET_IND_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_CM_PIPE_RESET_IND                            	4


//Definition for CM_PEP_RESET_REQ
#define CM_PEP_RESET_REQ_OFFSET_UTID                      	0 //size 1 byte(s)
#define CM_PEP_RESET_REQ_OFFSET_SUBFUNCTION               	1 //size 1 byte(s)
#define CM_PEP_RESET_REQ_OFFSET_PIPEHANDLE                	2 //size 1 byte(s)
#define CM_PEP_RESET_REQ_OFFSET_PIPESTATEAFTERRESET       	3 //size 1 byte(s)
#define SIZE_CM_PEP_RESET_REQ                             	4


//Definition for CM_PEP_RESET_RESP
#define CM_PEP_RESET_RESP_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_ERRORCODE                	3 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_PEP1ERRORCODE            	4 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_PEP2ERRORCODE            	5 //size 1 byte(s)
#define SIZE_CM_PEP_RESET_RESP                            	6


//Definition for CM_PIPE_REMOVE_REQ
#define CM_PIPE_REMOVE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_REMOVE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_REMOVE_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PIPE_REMOVE_REQ_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_CM_PIPE_REMOVE_REQ                           	4


//Definition for CM_PIPE_REMOVE_RESP
#define CM_PIPE_REMOVE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_REMOVE_RESP                          	6


//Definition for CM_PEP_DISCONNECT_REQ
#define CM_PEP_DISCONNECT_REQ_OFFSET_UTID                 	0 //size 1 byte(s)
#define CM_PEP_DISCONNECT_REQ_OFFSET_SUBFUNCTION          	1 //size 1 byte(s)
#define CM_PEP_DISCONNECT_REQ_OFFSET_PIPEHANDLE           	2 //size 1 byte(s)
#define CM_PEP_DISCONNECT_REQ_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISCONNECT_REQ                        	4


//Definition for CM_PEP_DISCONNECT_RESP
#define CM_PEP_DISCONNECT_RESP_OFFSET_UTID                	0 //size 1 byte(s)
#define CM_PEP_DISCONNECT_RESP_OFFSET_SUBFUNCTION         	1 //size 1 byte(s)
#define CM_PEP_DISCONNECT_RESP_OFFSET_PIPEHANDLE          	2 //size 1 byte(s)
#define CM_PEP_DISCONNECT_RESP_OFFSET_ERRORCODE           	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISCONNECT_RESP                       	4


//Definition for CM_PIPE_REDIRECT_REQ
#define CM_PIPE_REDIRECT_REQ_OFFSET_UTID                  	0 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_STATEAFTERCREATION    	3 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_OLDPEPDEV             	4 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_OLDPEPOBJ             	5 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_OLDPEPTYPE            	6 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_FILLERBYTE1           	7 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPDEV     	8 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPOBJ     	9 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPTYPE    	10 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_NSB                   	11 //size 1 byte(s)
#define SIZE_CM_PIPE_REDIRECT_REQ                         	12


//Definition for CM_PIPE_REDIRECT_RESP
#define CM_PIPE_REDIRECT_RESP_OFFSET_UTID                 	0 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_SUBFUNCTION          	1 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE           	2 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_ERRORCODE            	3 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_PEP1ERRORCODE        	4 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_PEP2ERRORCODE        	5 //size 1 byte(s)
#define SIZE_CM_PIPE_REDIRECT_RESP                        	6


//Definition for CM_PIPE_REDIRECTED_IND
#define CM_PIPE_REDIRECTED_IND_OFFSET_UTID                	0 //size 1 byte(s)
#define CM_PIPE_REDIRECTED_IND_OFFSET_SUBFUNCTION         	1 //size 1 byte(s)
#define CM_PIPE_REDIRECTED_IND_OFFSET_PIPEHANDLE          	2 //size 1 byte(s)
#define CM_PIPE_REDIRECTED_IND_OFFSET_NSB                 	3 //size 1 byte(s)
#define SIZE_CM_PIPE_REDIRECTED_IND                       	4


//Definition for CM_PEP_DISABLE_REQ
#define CM_PEP_DISABLE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PEP_DISABLE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PEP_DISABLE_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PEP_DISABLE_REQ_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISABLE_REQ                           	4


//Definition for CM_PEP_DISABLE_RESP
#define CM_PEP_DISABLE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PEP_DISABLE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PEP_DISABLE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PEP_DISABLE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISABLE_RESP                          	4


//Definition for CM_PIPE_DISABLED_IND
#define CM_PIPE_DISABLED_IND_OFFSET_UTID                  	0 //size 1 byte(s)
#define CM_PIPE_DISABLED_IND_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define CM_PIPE_DISABLED_IND_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define CM_PIPE_DISABLED_IND_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_CM_PIPE_DISABLED_IND                         	4

/* end of CL Pipe defines */

#endif