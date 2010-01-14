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
* Name      : PN_GPDS [ 0x31 ] Resource GPDS Shared Subblock Descriptions
* Version   : 019.001
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_GPDS_SHAREDISI_H_
#define _SYMBIAN_ISI_GPDS_SHAREDISI_H_

#define GPDS_SHARED_SUBBLOCK_DESCRIPTIONS_ISI_VERSION_Z	 19
#define GPDS_SHARED_SUBBLOCK_DESCRIPTIONS_ISI_VERSION_Y	 1


//CONSTANTS
#define GPDS_MAX_APN_STRING_SIZE                          	0x64                                              	
#define GPDS_MAX_TFT_INFO_SB_DATA                         	0x100                                             	
#define GPDS_MAX_TFT_AUTH_TOKEN_SIZE                      	0xF4                                              	


//TABLES

//ConstantTable for GPDS_DATA_COMPRESSION
#define GPDS_PDP_DCOMP_OFF                                	0x00                                                 	
#define GPDS_PDP_DCOMP_ON                                 	0x01                                                 	
#define GPDS_PDP_DCOMP_DEFAULT                            	0xFF                                              	

//ConstantTable for GPDS_HEADER_COMPRESSION
#define GPDS_PDP_HCOMP_OFF                                	0x00                                                 	
#define GPDS_PDP_HCOMP_ON                                 	0x01                                                 	
#define GPDS_PDP_HCOMP_DEFAULT                            	0xFF                                              	

//ConstantTable for GPDS_QOS99_TRAFFIC_CLASSES
#define GPDS_QOS99_TRAC_SUBSCRIBED                        	0x00                                                 	
#define GPDS_QOS99_TRAC_CONVERSATIONAL                    	0x01                                                 	
#define GPDS_QOS99_TRAC_STREAMING                         	0x02                                                 	
#define GPDS_QOS99_TRAC_INTERACTIVE                       	0x03                                                 	
#define GPDS_QOS99_TRAC_BACKGROUND                        	0x04                                                 	
#define GPDS_QOS99_TRAC_DEFAULT                           	0xFF                                              	

//ConstantTable for GPDS_QOS99_DELIVERY_ORDERS
#define GPDS_QOS99_DELO_SUBSCRIBED                        	0x00                                                 	
#define GPDS_QOS99_DELO_YES                               	0x01                                                 	
#define GPDS_QOS99_DELO_NO                                	0x02                                                 	
#define GPDS_QOS99_DELO_DEFAULT                           	0xFF                                              	

//ConstantTable for GPDS_QOS99_DELIVERY_OF_ERRONEOUS_SDUS
#define GPDS_QOS99_DOES_SUBSCRIBED                        	0x00                                                 	
#define GPDS_QOS99_DOES_NO_DETECT                         	0x01                                                 	
#define GPDS_QOS99_DOES_YES                               	0x02                                                 	
#define GPDS_QOS99_DOES_NO                                	0x03                                                 	
#define GPDS_QOS99_DOES_DEFAULT                           	0xFF                                              	

//ConstantTable for GPDS_QOS99_RESIDUAL_BERS
#define GPDS_QOS99_RBER_SUBSCRIBED                        	0x00                                                 	
#define GPDS_QOS99_RBER_1                                 	0x52                                              	
#define GPDS_QOS99_RBER_2                                 	0x12                                              	
#define GPDS_QOS99_RBER_3                                 	0x53                                              	
#define GPDS_QOS99_RBER_4                                 	0x43                                              	
#define GPDS_QOS99_RBER_5                                 	0x13                                              	
#define GPDS_QOS99_RBER_6                                 	0x14                                              	
#define GPDS_QOS99_RBER_7                                 	0x15                                              	
#define GPDS_QOS99_RBER_8                                 	0x16                                              	
#define GPDS_QOS99_RBER_9                                 	0x68                                              	
#define GPDS_QOS99_RBER_DEFAULT                           	0xFF                                              	

//ConstantTable for GPDS_QOS99_ERROR_RATIOS
#define GPDS_QOS99_SDER_SUBSCRIBED                        	0x00                                                 	
#define GPDS_QOS99_SDER_1                                 	0x12                                              	
#define GPDS_QOS99_SDER_2                                 	0x73                                              	
#define GPDS_QOS99_SDER_3                                 	0x13                                              	
#define GPDS_QOS99_SDER_4                                 	0x14                                              	
#define GPDS_QOS99_SDER_5                                 	0x15                                              	
#define GPDS_QOS99_SDER_6                                 	0x16                                              	
#define GPDS_QOS99_SDER_7                                 	0x11                                              	
#define GPDS_QOS99_SDER_DEFAULT                           	0xFF                                              	

//ConstantTable for GPDS_QOS99_TRAFFIC_HANDLING_PRIORITIES
#define GPDS_QOS99_TRHP_SUBSCRIBED                        	0x00                                                 	
#define GPDS_QOS99_TRHP_1                                 	0x01                                                 	
#define GPDS_QOS99_TRHP_2                                 	0x02                                                 	
#define GPDS_QOS99_TRHP_3                                 	0x03                                                 	
#define GPDS_QOS99_TRHP_DEFAULT                           	0xFF                                              	

//ConstantTable for GPDS_TFT_OPERATION
#define GPDS_TFT_CREATE_NEW                               	0x01                                                 	
#define GPDS_TFT_DELETE_EXISTING                          	0x02                                                 	
#define GPDS_TFT_ADD_PACKET_FILTERS                       	0x03                                                 	
#define GPDS_TFT_REPLACE_PACKET_FILTERS                   	0x04                                                 	
#define GPDS_TFT_DELETE_PACKET_FILTERS                    	0x05                                                 	
#define GPDS_TFT_NO_OPERATION                             	0x06                                                 	

//ConstantTable for GPDS_REL5_SOURCE_DESC
#define GPDS_SOURCE_UNKNOWN                               	0x00                                                 	
#define GPDS_SOURCE_SPEECH                                	0x01                                                 	
#define GPDS_SOURCE_DEFAULT                               	0xFF                                              	

//ConstantTable for GPDS_REL5_SGN_IND_FLAG
#define GPDS_QOS_NOT_OPT_SGN                              	0x00                                                 	
#define GPDS_QOS_OPT_SGN                                  	0x01                                                 	
#define GPDS_QOS_OPT_DEFAULT                              	0xFF                                              	

//ConstantTable for GPDS_IM_CN_SIGN_FLAG
#define GPDS_CTX_NOT_FOR_DEDICATED_SIGN                   	0x00                                                 	
#define GPDS_CTX_FOR_DEDICATED_SIGN                       	0x01                                                 	


//STRUCTURES


//SUBBLOCKS


//Definition for GPDS_SHARED_APN_INFO
#define GPDS_SHARED_APN_INFO                              	0xE1                                              	
#define GPDS_SHARED_APN_INFO_OFFSET_ID                    	0 //size 1 byte(s)
#define GPDS_SHARED_APN_INFO_OFFSET_LENGTH                	1 //size 1 byte(s)
#define GPDS_SHARED_APN_INFO_OFFSET_ADDRLENGTH            	2 //size 1 byte(s)
#define GPDS_SHARED_APN_INFO_OFFSET_APNNAME               	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_APN_INFO                         	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GPDS_SHARED_COMP_INFO
#define GPDS_SHARED_COMP_INFO                             	0xE0                                              	
#define GPDS_SHARED_COMP_INFO_OFFSET_ID                   	0 //size 1 byte(s)
#define GPDS_SHARED_COMP_INFO_OFFSET_LENGTH               	1 //size 1 byte(s)
#define GPDS_SHARED_COMP_INFO_OFFSET_DATACOMPRESSION      	2 //size 1 byte(s)
#define GPDS_SHARED_COMP_INFO_OFFSET_HEADERCOMPRESSION    	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_COMP_INFO                        	4


//Definition for GPDS_SHARED_QOS99_REQ_INFO
#define GPDS_SHARED_QOS99_REQ_INFO                        	0xE2                                              	
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_ID              	0 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_LENGTH          	1 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_TRAFFICCLASS    	2 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_DELIVERYORDER   	3 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_DELIVERYOFERRONEUSSDU	4 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_RESIDUALBER     	5 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_ERRORRATIO      	6 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_PRIORITY        	7 //size 1 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_TRANSFERDELAY   	8 //size 2 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_MAXSDUSIZE      	10 //size 2 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_MAXBITRATEUPLINK	12 //size 2 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_MAXBITRATEDOWNLINK	14 //size 2 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_GUABITRATEUPLINK	16 //size 2 byte(s)
#define GPDS_SHARED_QOS99_REQ_INFO_OFFSET_GUABITRATEDOWNLINK	18 //size 2 byte(s)
#define SIZE_GPDS_SHARED_QOS99_REQ_INFO                   	20


//Definition for GPDS_SHARED_QOS99_MIN_INFO
#define GPDS_SHARED_QOS99_MIN_INFO                        	0xE3                                              	
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_ID              	0 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_LENGTH          	1 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_TRAFFICCLASS    	2 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_DELIVERYORDER   	3 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_DELIVERYOFERRONEUSSDU	4 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_RESIDUALBER     	5 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_ERRORRATIO      	6 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_PRIORITY        	7 //size 1 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_TRANSFERDELAY   	8 //size 2 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_MAXSDUSIZE      	10 //size 2 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_MAXBITRATEUPLINK	12 //size 2 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_MAXBITRATEDOWNLINK	14 //size 2 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_GUABITRATEUPLINK	16 //size 2 byte(s)
#define GPDS_SHARED_QOS99_MIN_INFO_OFFSET_GUABITRATEDOWNLINK	18 //size 2 byte(s)
#define SIZE_GPDS_SHARED_QOS99_MIN_INFO                   	20


//Definition for GPDS_SHARED_QOS99_NEG_INFO
#define GPDS_SHARED_QOS99_NEG_INFO                        	0xE4                                              	
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_ID              	0 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_LENGTH          	1 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_TRAFFICCLASS    	2 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_DELIVERYORDER   	3 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_DELIVERYOFERRONEUSSDU	4 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_RESIDUALBER     	5 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_ERRORRATIO      	6 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_PRIORITY        	7 //size 1 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_TRANSFERDELAY   	8 //size 2 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_MAXSDUSIZE      	10 //size 2 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_MAXBITRATEUPLINK	12 //size 2 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_MAXBITRATEDOWNLINK	14 //size 2 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_GUABITRATEUPLINK	16 //size 2 byte(s)
#define GPDS_SHARED_QOS99_NEG_INFO_OFFSET_GUABITRATEDOWNLINK	18 //size 2 byte(s)
#define SIZE_GPDS_SHARED_QOS99_NEG_INFO                   	20


//Definition for GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO
#define GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO               	0xE6                                              	
#define GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO_OFFSET_ID     	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO_OFFSET_LENGTH 	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO_OFFSET_IPV4ADDRMASK	2 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO_OFFSET_IPV4ADDRESS	4 //size 4 byte(s)
#define SIZE_GPDS_SHARED_FILT_SRC_IPV4_ADDR_INFO          	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO
#define GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO               	0xE7                                              	
#define GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO_OFFSET_ID     	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO_OFFSET_LENGTH 	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO_OFFSET_IPV6ADDRMASK	2 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO_OFFSET_IPV6ADDRESS	4 //size 16 byte(s)
#define SIZE_GPDS_SHARED_FILT_SRC_IPV6_ADDR_INFO          	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_SHARED_FILT_PROTOCOL_INFO
#define GPDS_SHARED_FILT_PROTOCOL_INFO                    	0xE8                                              	
#define GPDS_SHARED_FILT_PROTOCOL_INFO_OFFSET_ID          	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_PROTOCOL_INFO_OFFSET_LENGTH      	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_PROTOCOL_INFO_OFFSET_PROTOCOLID  	2 //size 1 byte(s)
#define GPDS_SHARED_FILT_PROTOCOL_INFO_OFFSET_FILLERBYTE1 	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_FILT_PROTOCOL_INFO               	4


//Definition for GPDS_SHARED_FILT_DST_PORT_INFO
#define GPDS_SHARED_FILT_DST_PORT_INFO                    	0xE9                                              	
#define GPDS_SHARED_FILT_DST_PORT_INFO_OFFSET_ID          	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_DST_PORT_INFO_OFFSET_LENGTH      	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_DST_PORT_INFO_OFFSET_DESTINATIONPORT	2 //size 2 byte(s)
#define SIZE_GPDS_SHARED_FILT_DST_PORT_INFO               	4


//Definition for GPDS_SHARED_FILT_DST_PORT_RANGE_INFO
#define GPDS_SHARED_FILT_DST_PORT_RANGE_INFO              	0xEA                                              	
#define GPDS_SHARED_FILT_DST_PORT_RANGE_INFO_OFFSET_ID    	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_DST_PORT_RANGE_INFO_OFFSET_LENGTH	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_DST_PORT_RANGE_INFO_OFFSET_LOWLIMITDSTPORTRANGE	2 //size 2 byte(s)
#define GPDS_SHARED_FILT_DST_PORT_RANGE_INFO_OFFSET_HIGHLIMITDSTPORTRANGE	4 //size 2 byte(s)
#define GPDS_SHARED_FILT_DST_PORT_RANGE_INFO_OFFSET_FILLERBYTE1	6 //size 1 byte(s)
#define GPDS_SHARED_FILT_DST_PORT_RANGE_INFO_OFFSET_FILLERBYTE2	7 //size 1 byte(s)
#define SIZE_GPDS_SHARED_FILT_DST_PORT_RANGE_INFO         	8


//Definition for GPDS_SHARED_FILT_SRC_PORT_INFO
#define GPDS_SHARED_FILT_SRC_PORT_INFO                    	0xEB                                              	
#define GPDS_SHARED_FILT_SRC_PORT_INFO_OFFSET_ID          	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_PORT_INFO_OFFSET_LENGTH      	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_PORT_INFO_OFFSET_SOURCEPORT  	2 //size 2 byte(s)
#define SIZE_GPDS_SHARED_FILT_SRC_PORT_INFO               	4


//Definition for GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO
#define GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO              	0xEC                                              	
#define GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO_OFFSET_ID    	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO_OFFSET_LENGTH	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO_OFFSET_LOWLIMITSRCPORTRANGE	2 //size 2 byte(s)
#define GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO_OFFSET_HIGHLIMITSRCPORTRANGE	4 //size 2 byte(s)
#define GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO_OFFSET_FILLERBYTE1	6 //size 1 byte(s)
#define GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO_OFFSET_FILLERBYTE2	7 //size 1 byte(s)
#define SIZE_GPDS_SHARED_FILT_SRC_PORT_RANGE_INFO         	8


//Definition for GPDS_SHARED_FILT_SPI_INFO
#define GPDS_SHARED_FILT_SPI_INFO                         	0xED                                              	
#define GPDS_SHARED_FILT_SPI_INFO_OFFSET_ID               	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_SPI_INFO_OFFSET_LENGTH           	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_SPI_INFO_OFFSET_FILLERBYTE1      	2 //size 1 byte(s)
#define GPDS_SHARED_FILT_SPI_INFO_OFFSET_FILLERBYTE2      	3 //size 1 byte(s)
#define GPDS_SHARED_FILT_SPI_INFO_OFFSET_SECURITYPARAMIND 	4 //size 4 byte(s)
#define SIZE_GPDS_SHARED_FILT_SPI_INFO                    	8


//Definition for GPDS_SHARED_FILT_TOS_INFO
#define GPDS_SHARED_FILT_TOS_INFO                         	0xEE                                              	
#define GPDS_SHARED_FILT_TOS_INFO_OFFSET_ID               	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_TOS_INFO_OFFSET_LENGTH           	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_TOS_INFO_OFFSET_TOS              	2 //size 1 byte(s)
#define GPDS_SHARED_FILT_TOS_INFO_OFFSET_TOSMASK          	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_FILT_TOS_INFO                    	4


//Definition for GPDS_SHARED_FILT_FLOW_LABEL_INFO
#define GPDS_SHARED_FILT_FLOW_LABEL_INFO                  	0xEF                                              	
#define GPDS_SHARED_FILT_FLOW_LABEL_INFO_OFFSET_ID        	0 //size 1 byte(s)
#define GPDS_SHARED_FILT_FLOW_LABEL_INFO_OFFSET_LENGTH    	1 //size 1 byte(s)
#define GPDS_SHARED_FILT_FLOW_LABEL_INFO_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define GPDS_SHARED_FILT_FLOW_LABEL_INFO_OFFSET_FILLERBYTE2	3 //size 1 byte(s)
#define GPDS_SHARED_FILT_FLOW_LABEL_INFO_OFFSET_FLOWLABEL 	4 //size 4 byte(s)
#define SIZE_GPDS_SHARED_FILT_FLOW_LABEL_INFO             	8


//Definition for GPDS_SHARED_TFT_PACKET_FILTER_INFO
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO                	0xF6                                              	
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_ID      	0 //size 1 byte(s)
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_LENGTH  	1 //size 1 byte(s)
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_PACKETFILTERID	2 //size 1 byte(s)
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_FILTEREVALPREC	3 //size 1 byte(s)
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_FILLERBYTE1	4 //size 1 byte(s)
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_FILLERBYTE2	5 //size 1 byte(s)
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_FILLERBYTE3	6 //size 1 byte(s)
#define GPDS_SHARED_TFT_PACKET_FILTER_INFO_OFFSET_NUMBEROFSUBBLOCKS	7 //size 1 byte(s)
#define SIZE_GPDS_SHARED_TFT_PACKET_FILTER_INFO           	8


//Definition for GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO
#define GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO            	0xF2                                              	
#define GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO_OFFSET_ID  	0 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO_OFFSET_LENGTH	1 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO_OFFSET_FILLERBYTE2	3 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO_OFFSET_MEDIACOMPNO	4 //size 2 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO_OFFSET_IPFLOWNO	6 //size 2 byte(s)
#define SIZE_GPDS_SHARED_TFT_PARAMETER_IP_FLOW_INFO       	8


//Definition for GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO         	0xF1                                              	
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_ID	0 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_LENGTH	1 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_AUTHTOKENLENGTH	2 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_AUTHTOKEN	4 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_FILLERBYTE2	5 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_FILLERBYTE3	6 //size 1 byte(s)
#define GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO_OFFSET_FILLERBYTE4	7 //size 1 byte(s)
#define SIZE_GPDS_SHARED_TFT_PARAMETER_AUTH_TOKEN_INFO    	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GPDS_SHARED_TFT_INFO
#define GPDS_SHARED_TFT_INFO                              	0xE5                                              	
#define GPDS_SHARED_TFT_INFO_OFFSET_ID                    	0 //size 1 byte(s)
#define GPDS_SHARED_TFT_INFO_OFFSET_LENGTH                	1 //size 1 byte(s)
#define GPDS_SHARED_TFT_INFO_OFFSET_TFTOPERATION          	2 //size 1 byte(s)
#define GPDS_SHARED_TFT_INFO_OFFSET_NUMBEROFSUBBLOCKS     	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_TFT_INFO                         	4


//Definition for GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO
#define GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO                	0xF3                                              	
#define GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO_OFFSET_ID      	0 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO_OFFSET_LENGTH  	1 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO_OFFSET_FILLERBYTE2	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_PCSCF_ADDRESS_REQ_INFO           	4


//Definition for GPDS_SHARED_PCSCF_ADDRESS_INFO
#define GPDS_SHARED_PCSCF_ADDRESS_INFO                    	0xF4                                              	
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_ID          	0 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_LENGTH      	1 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_FILLERBYTE1 	2 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_ADDRLENGTH  	3 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_PCSCFADDRESS	4 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_FILLERBYTE2 	5 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_FILLERBYTE3 	6 //size 1 byte(s)
#define GPDS_SHARED_PCSCF_ADDRESS_INFO_OFFSET_FILLERBYTE4 	7 //size 1 byte(s)
#define SIZE_GPDS_SHARED_PCSCF_ADDRESS_INFO               	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO
#define GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO          	0xF5                                              	
#define GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO_OFFSET_ID	0 //size 1 byte(s)
#define GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO_OFFSET_LENGTH	1 //size 1 byte(s)
#define GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO_OFFSET_REJECTIONCAUSE	2 //size 1 byte(s)
#define GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_POLICY_CONTROL_REJ_CODE_INFO     	4


//Definition for GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO
#define GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO             	0xF7                                              	
#define GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO_OFFSET_ID   	0 //size 1 byte(s)
#define GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO_OFFSET_LENGTH	1 //size 1 byte(s)
#define GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO_OFFSET_IMCNSIGFLAG	2 //size 1 byte(s)
#define GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_IM_CN_SIGNALING_FLAG_INFO        	4


//Definition for GPDS_SHARED_REL5_QOS_INFO
#define GPDS_SHARED_REL5_QOS_INFO                         	0xF8                                              	
#define GPDS_SHARED_REL5_QOS_INFO_OFFSET_ID               	0 //size 1 byte(s)
#define GPDS_SHARED_REL5_QOS_INFO_OFFSET_LENGTH           	1 //size 1 byte(s)
#define GPDS_SHARED_REL5_QOS_INFO_OFFSET_SRCSTATDESC      	2 //size 1 byte(s)
#define GPDS_SHARED_REL5_QOS_INFO_OFFSET_SIGINDFLAG       	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_REL5_QOS_INFO                    	4


//Definition for GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO
#define GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO               	0xF9                                              	
#define GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO_OFFSET_ID     	0 //size 1 byte(s)
#define GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO_OFFSET_LENGTH 	1 //size 1 byte(s)
#define GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO_OFFSET_FILLERBYTE2	3 //size 1 byte(s)
#define SIZE_GPDS_SHARED_RADIO_ACTIVITY_REQ_INFO          	4


//Definition for GPDS_SHARED_INITIAL_DL_DCH_RATE
#define GPDS_SHARED_INITIAL_DL_DCH_RATE                   	0xFC                                              	
#define GPDS_SHARED_INITIAL_DL_DCH_RATE_OFFSET_ID         	0 //size 1 byte(s)
#define GPDS_SHARED_INITIAL_DL_DCH_RATE_OFFSET_LENGTH     	1 //size 1 byte(s)
#define GPDS_SHARED_INITIAL_DL_DCH_RATE_OFFSET_MAXBITRATEDOWNLINK	2 //size 2 byte(s)
#define SIZE_GPDS_SHARED_INITIAL_DL_DCH_RATE              	4

#endif