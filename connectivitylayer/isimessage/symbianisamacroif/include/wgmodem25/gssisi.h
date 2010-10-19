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
* Name      : PN_GSS [ 0x32 ] Resource GSM Stack Server
* Version   : 008.005
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_GSSISI_H_
#define _SYMBIAN_ISI_GSSISI_H_

#define GSM_STACK_SERVER_ISI_VERSION_Z	 9
#define GSM_STACK_SERVER_ISI_VERSION_Y	 0


//CONSTANTS
#define GSS_FILL                                          	0x00                                                 	
#define GSS_GENERAL_ISI_DATA_HDR_LEN                      	0x02                                                 	
#define GSS_DATA_IND_LENGTH                               	0x04                                                 	
#define GSS_SUBBLOCK_HEADER_LENGTH                        	0x04                                                 	
#define GSS_CS_MSG_HDR_LENGTH                             	0x04                                                 	
#define GSS_IND_DATA_MAX_SIZE                             	0x258                                             	
#define GSS_TIMING_ADVANCE_NOT_AVAILABLE                  	0xFF                                              	
#define GSS_SGW_IP_ADDRESS_LENGTH                         	0x10                                              	
#define GSS_UNC_IP_ADDRESS_LENGTH                         	0x10                                              	
#define GSS_SRV_NO_FQDN                                   	0x00                                                 	
#define GSS_ENV_INFO_STATUS_UNKNOWN                       	0x00                                                 	
#define GSS_ANY_SIZE                                      	0x01                                                 	


//TABLES

//ConstantTable for GSS_OPERATION
#define GSS_CS_STATUS_GET                                 	0x00                                                 	
#define GSS_CS_TASKS_SUSPEND                              	0x01                                                 	
#define GSS_CS_LOCAL_INFO_GET                             	0x03                                                 	
#define GSS_POWER_CLASS_CHANGE                            	0x04                                                 	
#define GSS_BAND_INFO_GET                                 	0x05                                                 	
#define GSS_SELECTED_RAT_WRITE                            	0x0E                                                 	
#define GSS_ATK_TIMING_ADVANCE_GET                        	0x96                                              	
#define GSS_UMA_PREF_MODE_READ                            	0x97                                              	
#define GSS_UMA_PREF_MODE_WRITE                           	0x98                                              	
#define GSS_PROV_SGW_UNC_WRITE                            	0x99                                              	
#define GSS_PROV_SGW_UNC_READ                             	0x9A                                              	
#define GSS_GAN_CONNECT                                   	0x9B                                              	
#define GSS_SELECTED_RAT_READ                             	0x9C                                              	
#define GSS_SELECTED_BANDS_WRITE                          	0x9D                                              	
#define GSS_SELECTED_BANDS_READ                           	0x9E                                              	
#define GSS_CELL_BARRING_STATUS_READ                      	0x9F                                              	

//ConstantTable for GSS_COMMAND_QUALIFIER
#define GSS_CELL_INFO_CMD                                 	0x00                                                 	
#define GSS_LONG_CELL_INFO_CMD                            	0x01                                                 	

//ConstantTable for GSS_POWERCLASS
#define GSS_POWER_CLASS_1                                 	0x01                                                 	
#define GSS_POWER_CLASS_2                                 	0x02                                                 	
#define GSS_POWER_CLASS_3                                 	0x03                                                 	
#define GSS_POWER_CLASS_4                                 	0x04                                                 	
#define GSS_POWER_CLASS_5                                 	0x05                                                 	
#define GSS_POWER_CLASS_NO_CHANGE                         	0xFD                                              	
#define GSS_POWER_CLASS_DEFAULT                           	0xFE                                              	
#define GSS_POWER_CLASS_NOT_IN_USE                        	0xFF                                              	

//ConstantTable for GSS_CAUSE
#define GSS_SERVICE_FAIL                                  	0x01                                                 	
#define GSS_SERVICE_NOT_ALLOWED                           	0x02                                                 	
#define GSS_SERVICE_FAIL_CS_INACTIVE                      	0x03                                                 	

//BitmaskTables for GSS_GSM_BAND_INFO
#define GSM_900_BAND_SUPPORT                              	0x01                                                 	
#define GSM_1800_BAND_SUPPORT                             	0x02                                                 	
#define GSM_1900_BAND_SUPPORT                             	0x04                                                 	
#define GSM_850_BAND_SUPPORT                              	0x08                                                 	

//ConstantTable for GSS_CS_STATE
#define GSS_CS_IDLE                                       	0x00                                                 	
#define GSS_CS_SIGNALLING                                 	0x01                                                 	
#define GSS_CS_CALL                                       	0x02                                                 	
#define GSS_CS_PACKET_DATA                                	0x03                                                 	

//ConstantTable for GSS_CS_REGISTRATION_STATUS
#define GSS_CS_STATUS_SERV                                	0x80                                              	
#define GSS_CS_STATUS_CAMPED_ON_CELL                      	0x0C                                                 	
#define GSS_CS_STATUS_IDLE                                	0x0D                                                 	
#define GSS_CS_STATUS_RSSI_MEASURING                      	0x0E                                                 	
#define GSS_CS_STATUS_PSW_WAIT                            	0x10                                              	
#define GSS_CS_STATUS_SYNC_WAIT                           	0x11                                              	
#define GSS_CS_STATUS_SCANNING                            	0x12                                              	
#define GSS_CS_STATUS_ALL_CCCH                            	0x13                                              	
#define GSS_CS_STATUS_SDCCH                               	0x14                                              	
#define GSS_CS_STATUS_TCH                                 	0x15                                              	
#define GSS_CS_STATUS_INIT                                	0x16                                              	

//BitmaskTables for GSS_RAT_INFO_FUNCTIONS
#define GSS_GSM_RAT                                       	0x01                                                 	
#define GSS_UMTS_RAT                                      	0x02                                                 	
#define GSS_EUTRAN_RAT                                    	0x04                                                 	
#define GSS_DUAL_RAT                                      	0x03                                                 	

//BitmaskTables for GSS_GSM_BAND_VALUES
#define GSS_GSM_NO_BANDS                                  	0x00                                                 	
#define GSS_GSM_900_BAND_MASK                             	0x01                                                 	
#define GSS_GSM_1800_BAND_MASK                            	0x02                                                 	
#define GSS_GSM_1900_BAND_MASK                            	0x04                                                 	
#define GSS_GSM_850_BAND_MASK                             	0x08                                                 	
#define GSS_ALL_GSM_BANDS                                 	0xFF                                              	


//BitmaskTables for GSS_UMTS_BAND_VALUES
#define GSS_UMTS_NO_BANDS                                 	0x00                                                 	
#define GSS_UMTS_BAND_I_MASK                              	0x01                                                 	
#define GSS_UMTS_BAND_II_MASK                             	0x02                                                 	
#define GSS_UMTS_BAND_III_MASK                            	0x04                                                 	
#define GSS_UMTS_BAND_IV_MASK                             	0x08                                                 	
#define GSS_UMTS_BAND_V_MASK                              	0x10                                              	
#define GSS_UMTS_BAND_VI_MASK                             	0x20                                              	
#define GSS_UMTS_BAND_VII_MASK                            	0x40                                              	
#define GSS_UMTS_BAND_VIII_MASK                           	0x80                                              	
#define GSS_UMTS_BAND_IX_MASK                             	0x100                                             	
#define GSS_UMTS_BAND_X_MASK                              	0x200                                             	
#define GSS_UMTS_BAND_XI_MASK                             	0x400                                             	
#define GSS_UMTS_BAND_XII_MASK                            	0x800                                             	
#define GSS_UMTS_BAND_XIII_MASK                           	0x1000                                            	
#define GSS_UMTS_BAND_XIV_MASK                            	0x2000                                            	
#define GSS_UMTS_BAND_XV_MASK                             	0x4000                                            	
#define GSS_UMTS_BAND_XVI_MASK                            	0x8000                                            	
#define GSS_UMTS_BAND_XVII_MASK                           	0x10000                                           	
#define GSS_UMTS_BAND_XVIII_MASK                          	0x20000                                           	
#define GSS_UMTS_BAND_XIX_MASK                            	0x40000                                           	
#define GSS_UMTS_BAND_XX_MASK                             	0x80000                                           	
#define GSS_UMTS_BAND_XXI_MASK                            	0x100000                                          	
#define GSS_UMTS_BAND_XXII_MASK                           	0x200000                                          	
#define GSS_UMTS_ALL_BANDS                                	0xFFFFFFFF                                        	

//ConstantTable for TIMING_ADVANCE_STATUS
#define GSS_TIMING_ADVANCE_NOT_VALID                      	0x00                                                 	
#define GSS_TIMING_ADVANCE_VALID                          	0x01                                                 	

//ConstantTable for GSS_UMA_MODE_SELECTION
#define GSS_GERAN_ONLY                                    	0x00                                                 	
#define GSS_GERAN_PREFERRED                               	0x01                                                 	
#define GSS_UMAN_ONLY                                     	0x02                                                 	
#define GSS_UMAN_PREFERRED                                	0x03                                                 	

//ConstantTable for GSS_SGW_UNC_IP_ADDRESS
#define GSS_SRV_NO_IP_ADDRESS                             	0x00                                                 	
#define GSS_SRV_IP_ADDR_TYPE_IPv4                         	0x01                                                 	
#define GSS_SRV_IP_ADDR_TYPE_IPv6                         	0x02                                                 	

//ConstantTable for GSS_HSXPA_USER_SETTING
#define GSS_HSXPA_DISABLED                                	0x01                                                 	
#define GSS_HSXPA_ALLOWED                                 	0x02                                                 	

//ConstantTable for GSS_WRITE_STATUS
#define GSS_OK                                            	0x01                                                 	
#define GSS_FAIL                                          	0x02                                                 	

//ConstantTable for GSS_HSXPA_CAUSE
#define GSS_HSXPA_NO_CAUSE                                	0x00                                                 	
#define GSS_HSXPA_DISABLED_VIA_PP                         	0x01                                                 	

//ConstantTable for GSS_HAC_MODE
#define GSS_HAC_ACTIVE                                    	0x01                                                 	
#define GSS_HAC_INACTIVE                                  	0x02                                                 	

//ConstantTable for GSS_ENV_REQUEST_TYPE
#define GSS_ENV_INFO_SET                                  	0x01                                                 	
#define GSS_ENV_INFO_QUERY_CERTAIN                        	0x02                                                 	
#define GSS_ENV_INFO_QUERY_ALL                            	0x03                                                 	

//ConstantTable for GSS_ENV_INFO_RESP_STATUS
#define GSS_ENV_INFO_OK                                   	0x01                                                 	
#define GSS_ENV_INFO_FAIL                                 	0x02                                                 	

//ConstantTable for GSS_USER_ACTIVITY_STATUS
//#define GSS_ENV_INFO_STATUS_UNKNOWN                     	0x00                   	 //Already defined as 0x00
#define GSS_ENV_USER_ACTIVE                               	0x01                                                 	
#define GSS_ENV_USER_INACTIVE                             	0x02                                                 	

//ConstantTable for GSS_BATTERY_INFO
//#define GSS_ENV_INFO_STATUS_UNKNOWN                     	0x00                   	 //Already defined as 0x00
#define GSS_BATTERY_IN_CHARGER                            	0x01                                                 	
#define GSS_BATTERY_NORMAL                                	0x02                                                 	
#define GSS_BATTERY_LOW                                   	0x03                                                 	


//SUBBLOCKS


//Definition for GSS_CS_LOCAL_INFO
#define GSS_CS_LOCAL_INFO                                 	0x01                                                 	
#define GSS_CS_LOCAL_INFO_OFFSET_SUBBLOCKID               	0 //size 1 byte(s)
#define GSS_CS_LOCAL_INFO_OFFSET_SUBBLOCKLENGTH           	1 //size 1 byte(s)
#define GSS_CS_LOCAL_INFO_OFFSET_COMMANDQUALIFIER         	2 //size 1 byte(s)
#define GSS_CS_LOCAL_INFO_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_GSS_CS_LOCAL_INFO                            	4


//Definition for GSS_POWER_CLASS
#define GSS_POWER_CLASS                                   	0x02                                                 	
#define GSS_POWER_CLASS_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define GSS_POWER_CLASS_OFFSET_SUBBLOCKLENGTH             	1 //size 1 byte(s)
#define GSS_POWER_CLASS_OFFSET_GSMLOWERBANDSPOWERCLASS    	2 //size 1 byte(s)
#define GSS_POWER_CLASS_OFFSET_GSMUPPERBANDSPOWERCLASS    	3 //size 1 byte(s)
#define SIZE_GSS_POWER_CLASS                              	4


//Definition for GSS_CS_STATUS
#define GSS_CS_STATUS                                     	0x00                                                 	
#define GSS_CS_STATUS_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define GSS_CS_STATUS_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define GSS_CS_STATUS_OFFSET_REGISTRATIONSTATUS           	2 //size 1 byte(s)
#define GSS_CS_STATUS_OFFSET_FILLERBYTE1                  	3 //size 1 byte(s)
#define SIZE_GSS_CS_STATUS                                	4


//Definition for GSS_CELL_INFO
#define GSS_CELL_INFO                                     	0x03                                                 	
#define GSS_CELL_INFO_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define GSS_CELL_INFO_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define GSS_CELL_INFO_OFFSET_MCCMNC                       	2 //size 3 byte(s)
#define GSS_CELL_INFO_OFFSET_TIMINGADVANCE                	5 //size 1 byte(s)
#define GSS_CELL_INFO_OFFSET_LAC                          	6 //size 2 byte(s)
#define GSS_CELL_INFO_OFFSET_CELLID                       	8 //size 2 byte(s)
#define GSS_CELL_INFO_OFFSET_FILLERBYTE1                  	10 //size 2 byte(s)
#define SIZE_GSS_CELL_INFO                                	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GSS_LONG_CELL_INFO
#define GSS_LONG_CELL_INFO                                	0x11                                              	
#define GSS_LONG_CELL_INFO_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_MCCMNC                  	2 //size 3 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_TIMINGADVANCE           	5 //size 1 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_LAC                     	6 //size 2 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_CELLID                  	8 //size 4 byte(s)
#define SIZE_GSS_LONG_CELL_INFO                           	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GSS_BAND_INFO
#define GSS_BAND_INFO                                     	0x04                                                 	
#define GSS_BAND_INFO_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_BANDINFO                     	2 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_GSMLOWERBANDSPOWERCLASS      	3 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_GSMUPPERBANDSPOWERCLASS      	4 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_FILLERBYTE1                  	5 //size 3 byte(s)
#define SIZE_GSS_BAND_INFO                                	8


//Definition for GSS_RAT_INFO
#define GSS_RAT_INFO                                      	0x0B                                                 	
#define GSS_RAT_INFO_OFFSET_SUBBLOCKID                    	0 //size 1 byte(s)
#define GSS_RAT_INFO_OFFSET_SUBBLOCKLENGTH                	1 //size 1 byte(s)
#define GSS_RAT_INFO_OFFSET_FUNCTION                      	2 //size 1 byte(s)
#define GSS_RAT_INFO_OFFSET_FILLERBYTE1                   	3 //size 1 byte(s)
#define SIZE_GSS_RAT_INFO                                 	4


//Definition for GSS_ATK_TIMING_ADVANCE
#define GSS_ATK_TIMING_ADVANCE                            	0x0C                                                 	
#define GSS_ATK_TIMING_ADVANCE_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define GSS_ATK_TIMING_ADVANCE_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define GSS_ATK_TIMING_ADVANCE_OFFSET_TASTATUS            	2 //size 1 byte(s)
#define GSS_ATK_TIMING_ADVANCE_OFFSET_TIMINGADVANCE       	3 //size 1 byte(s)
#define SIZE_GSS_ATK_TIMING_ADVANCE                       	4


//Definition for GSS_UMA_PREF_MODE_INFO
#define GSS_UMA_PREF_MODE_INFO                            	0x0D                                                 	
#define GSS_UMA_PREF_MODE_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_INFO_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_INFO_OFFSET_UMAPREFMODEI        	2 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_INFO_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_GSS_UMA_PREF_MODE_INFO                       	4


//Definition for GSS_PROV_INFO_SB_IDS
#define GSS_PROV_INFO_SB_IDS                              	0x10                                              	
#define GSS_PROV_INFO_SB_IDS_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define GSS_PROV_INFO_SB_IDS_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define GSS_PROV_INFO_SB_IDS_OFFSET_SBIDCOUNT             	2 //size 1 byte(s)
#define GSS_PROV_INFO_SB_IDS_OFFSET_PROVSBID              	3 //size 1 byte(s)
#define SIZE_GSS_PROV_INFO_SB_IDS                         	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GSS_SGW_INFO
#define GSS_SGW_INFO                                      	0x0E                                                 	
#define GSS_SGW_INFO_OFFSET_SUBBLOCKID                    	0 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_FILLERBYTE1                   	1 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_SUBBLOCKLENGTH                	2 //size 2 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWIPADDRTYPE                 	4 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWIPADDRESS                  	5 //size 16 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWFQDNLENGTH                 	21 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWFQDN                       	22 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_FILLERBYTE2                   	23 //size 1 byte(s)
#define SIZE_GSS_SGW_INFO                                 	24
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GSS_UNC_INFO
#define GSS_UNC_INFO                                      	0x0F                                                 	
#define GSS_UNC_INFO_OFFSET_SUBBLOCKID                    	0 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE1                   	1 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_SUBBLOCKLENGTH                	2 //size 2 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCTCPPORT                    	4 //size 2 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCIPADDRTYPE                 	6 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCIPADDRESS                  	7 //size 16 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCFQDNLENGTH                 	23 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCFQDN                       	24 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE2                   	25 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE3                   	26 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE4                   	27 //size 1 byte(s)
#define SIZE_GSS_UNC_INFO                                 	28
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GSS_SELECTED_UMTS_BAND_INFO
#define GSS_SELECTED_UMTS_BAND_INFO                       	0x12                                              	
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SUBBLOCKLENGTH 	1 //size 1 byte(s)
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_FILLERBYTE1    	2 //size 2 byte(s)
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SELECTEDUMTSBANDS	4 //size 4 byte(s)
#define SIZE_GSS_SELECTED_UMTS_BAND_INFO                  	8


//Definition for GSS_SELECTED_GSM_BAND_INFO
#define GSS_SELECTED_GSM_BAND_INFO                        	0x13                                              	
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_SUBBLOCKID      	0 //size 1 byte(s)
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_SUBBLOCKLENGTH  	1 //size 1 byte(s)
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_SELECTEDGSMBANDS	2 //size 1 byte(s)
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_FILLERBYTE1     	3 //size 1 byte(s)
#define SIZE_GSS_SELECTED_GSM_BAND_INFO                   	4
//Definition for GSS_REL_SIGNAL_LEVEL_INFO
#define GSS_REL_SIGNAL_LEVEL_INFO                         	0x14                                              	
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_SUBBLOCKLENGTH   	1 //size 1 byte(s)
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_RELSIGNALLEVEL   	2 //size 1 byte(s)
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_GSS_REL_SIGNAL_LEVEL_INFO                    	4


//Definition for GSS_THRESHOLD_INFO
#define GSS_THRESHOLD_INFO                                	0x15                                              	
#define GSS_THRESHOLD_INFO_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define GSS_THRESHOLD_INFO_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define GSS_THRESHOLD_INFO_OFFSET_THRESHOLD               	2 //size 1 byte(s)
#define GSS_THRESHOLD_INFO_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_GSS_THRESHOLD_INFO                           	4


//Definition for GSS_ENV_USER_ACTIVITY_INFO
#define GSS_ENV_USER_ACTIVITY_INFO                        	0x17                                              	
#define GSS_ENV_USER_ACTIVITY_INFO_OFFSET_SUBBLOCKID      	0 //size 1 byte(s)
#define GSS_ENV_USER_ACTIVITY_INFO_OFFSET_SUBBLOCKLENGTH  	1 //size 1 byte(s)
#define GSS_ENV_USER_ACTIVITY_INFO_OFFSET_ACTIVITYSTATUS  	2 //size 1 byte(s)
#define GSS_ENV_USER_ACTIVITY_INFO_OFFSET_FILLERBYTE1     	3 //size 1 byte(s)
#define SIZE_GSS_ENV_USER_ACTIVITY_INFO                   	4


//Definition for GSS_ENV_BATTERY_INFO
#define GSS_ENV_BATTERY_INFO                              	0x18                                              	
#define GSS_ENV_BATTERY_INFO_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define GSS_ENV_BATTERY_INFO_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define GSS_ENV_BATTERY_INFO_OFFSET_BATTERYINFORMATION    	2 //size 1 byte(s)
#define GSS_ENV_BATTERY_INFO_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_GSS_ENV_BATTERY_INFO                         	4


//MESSAGES


//Definition for GSS_CS_SERVICE_REQ
#define GSS_CS_SERVICE_REQ                                	0x00                                                 	
#define GSS_CS_SERVICE_REQ_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define GSS_CS_SERVICE_REQ_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define GSS_CS_SERVICE_REQ_OFFSET_OPERATION               	2 //size 1 byte(s)
#define GSS_CS_SERVICE_REQ_OFFSET_NBROFSUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_GSS_CS_SERVICE_REQ                           	4


//Definition for GSS_CS_SERVICE_RESP
#define GSS_CS_SERVICE_RESP                               	0x01                                                 	
#define GSS_CS_SERVICE_RESP_OFFSET_TRANSID                	0 //size 1 byte(s)
#define GSS_CS_SERVICE_RESP_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define GSS_CS_SERVICE_RESP_OFFSET_OPERATION              	2 //size 1 byte(s)
#define GSS_CS_SERVICE_RESP_OFFSET_NBROFSUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_GSS_CS_SERVICE_RESP                          	4


//Definition for GSS_CS_SERVICE_FAIL_RESP
#define GSS_CS_SERVICE_FAIL_RESP                          	0x02                                                 	
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_OPERATION         	2 //size 1 byte(s)
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_CAUSE             	3 //size 1 byte(s)
#define SIZE_GSS_CS_SERVICE_FAIL_RESP                     	4


//Definition for GSS_CS_IND
#define GSS_CS_IND                                        	0x03                                                 	
#define GSS_CS_IND_OFFSET_TRANSID                         	0 //size 1 byte(s)
#define GSS_CS_IND_OFFSET_MESSAGEID                       	1 //size 1 byte(s)
#define GSS_CS_IND_OFFSET_CSSTATE                         	2 //size 1 byte(s)
#define GSS_CS_IND_OFFSET_FILLERBYTE1                     	3 //size 1 byte(s)
#define SIZE_GSS_CS_IND                                   	4


//Definition for GSS_POWER_CLASS_IND
#define GSS_POWER_CLASS_IND                               	0x0A                                                 	
#define GSS_POWER_CLASS_IND_OFFSET_TRANSID                	0 //size 1 byte(s)
#define GSS_POWER_CLASS_IND_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define GSS_POWER_CLASS_IND_OFFSET_FILLERBYTE1            	2 //size 1 byte(s)
#define GSS_POWER_CLASS_IND_OFFSET_NBROFSUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_GSS_POWER_CLASS_IND                          	4


//Definition for GSS_SELECTED_RAT_IND
#define GSS_SELECTED_RAT_IND                              	0x14                                              	
#define GSS_SELECTED_RAT_IND_OFFSET_TRANSID               	0 //size 1 byte(s)
#define GSS_SELECTED_RAT_IND_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define GSS_SELECTED_RAT_IND_OFFSET_SELECTEDRAT           	2 //size 1 byte(s)
#define GSS_SELECTED_RAT_IND_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_GSS_SELECTED_RAT_IND                         	4


//Definition for GSS_UMA_PREF_MODE_IND
#define GSS_UMA_PREF_MODE_IND                             	0x15                                              	
#define GSS_UMA_PREF_MODE_IND_OFFSET_TRANSID              	0 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_IND_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_IND_OFFSET_UMAPREFMODE          	2 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_IND_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_GSS_UMA_PREF_MODE_IND                        	4


//Definition for GSS_HSXPA_USER_SETTING_WRITE_REQ
#define GSS_HSXPA_USER_SETTING_WRITE_REQ                  	0x0F                                                 	
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_TRANSID   	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_HSXPAUSERSETTING	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_WRITE_REQ             	4


//Definition for GSS_HSXPA_USER_SETTING_WRITE_RESP
#define GSS_HSXPA_USER_SETTING_WRITE_RESP                 	0x10                                              	
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_TRANSID  	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_WRITESTATUS	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_FAILCAUSE	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_WRITE_RESP            	4


//Definition for GSS_HSXPA_USER_SETTING_IND
#define GSS_HSXPA_USER_SETTING_IND                        	0x11                                              	
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_TRANSID         	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_MESSAGEID       	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_HSXPAUSERSETTING	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_CAUSE           	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_IND                   	4


//Definition for GSS_HSXPA_USER_SETTING_READ_REQ
#define GSS_HSXPA_USER_SETTING_READ_REQ                   	0x12                                              	
#define GSS_HSXPA_USER_SETTING_READ_REQ_OFFSET_TRANSID    	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_REQ_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_REQ_OFFSET_FILLERBYTE1	2 //size 2 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_READ_REQ              	4


//Definition for GSS_HSXPA_USER_SETTING_READ_RESP
#define GSS_HSXPA_USER_SETTING_READ_RESP                  	0x13                                              	
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_TRANSID   	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_HSXPAUSERSETTING	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_CAUSE     	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_READ_RESP             	4


//Definition for GSS_HAC_MODE_WRITE_REQ
#define GSS_HAC_MODE_WRITE_REQ                            	0x17                                              	
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_TRANSID             	0 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_HACMODE             	2 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_GSS_HAC_MODE_WRITE_REQ                       	4


//Definition for GSS_HAC_MODE_WRITE_RESP
#define GSS_HAC_MODE_WRITE_RESP                           	0x18                                              	
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_TRANSID            	0 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_WRITESTATUS        	2 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_GSS_HAC_MODE_WRITE_RESP                      	4


//Definition for GSS_ENV_INFO_REQ
#define GSS_ENV_INFO_REQ                                  	0x19                                              	
#define GSS_ENV_INFO_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define GSS_ENV_INFO_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define GSS_ENV_INFO_REQ_OFFSET_ENVREQUESTTYPE            	2 //size 1 byte(s)
#define GSS_ENV_INFO_REQ_OFFSET_NBROFSUBBLOCKS            	3 //size 1 byte(s)
#define SIZE_GSS_ENV_INFO_REQ                             	4


//Definition for GSS_ENV_INFO_RESP
#define GSS_ENV_INFO_RESP                                 	0x20                                              	
#define GSS_ENV_INFO_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define GSS_ENV_INFO_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define GSS_ENV_INFO_RESP_OFFSET_ENVINFOSTATUS            	2 //size 1 byte(s)
#define GSS_ENV_INFO_RESP_OFFSET_NBROFSUBBLOCKS           	3 //size 1 byte(s)
#define SIZE_GSS_ENV_INFO_RESP                            	4


//Definition for GSS_ENV_INFO_IND
#define GSS_ENV_INFO_IND                                  	0x21                                              	
#define GSS_ENV_INFO_IND_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define GSS_ENV_INFO_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define GSS_ENV_INFO_IND_OFFSET_FILLERBYTE1               	2 //size 1 byte(s)
#define GSS_ENV_INFO_IND_OFFSET_NBROFSUBBLOCKS            	3 //size 1 byte(s)
#define SIZE_GSS_ENV_INFO_IND                             	4

#endif