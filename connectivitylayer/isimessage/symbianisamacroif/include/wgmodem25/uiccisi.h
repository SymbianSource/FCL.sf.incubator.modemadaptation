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
* Name      : PN_UICC [ 0x8C ] Resource UICC Server
* Version   : 003.000
*
*/


#ifndef _SYMBIAN_ISI_UICCISI_H_
#define _SYMBIAN_ISI_UICCISI_H_

#define UICC_SERVER_ISI_VERSION_Z	 3
#define UICC_SERVER_ISI_VERSION_Y	 0


//CONSTANTS
#define UICC_EF_ID_NOT_PRESENT                            	0x00                                                 	
#define UICC_SFI_NOT_PRESENT                              	0x00                                                 	
#define UICC_APPL_ID_UNKNOWN                              	0x00                                                 	
#define UICC_APPL_LAST                                    	0xFF                                              	
#define UICC_SESSION_ID_NOT_USED                          	0x00                                                 	
#define UICC_ANY_SIZE                                     	0x01                                                 	


//TABLES

//ConstantTable for UICC_STATUS_CONSTANTS
#define UICC_STATUS_OK                                    	0x00                                                 	
#define UICC_STATUS_FAIL                                  	0x01                                                 	
#define UICC_STATUS_UNKNOWN                               	0x02                                                 	
#define UICC_STATUS_NOT_READY                             	0x10                                              	
#define UICC_STATUS_START_UP_COMPLETED                    	0x11                                              	
#define UICC_STATUS_SHUTTING_DOWN                         	0x12                                              	
#define UICC_STATUS_CARD_NOT_READY                        	0x20                                              	
#define UICC_STATUS_CARD_READY                            	0x21                                              	
#define UICC_STATUS_CARD_DISCONNECTED                     	0x22                                              	
#define UICC_STATUS_CARD_NOT_PRESENT                      	0x23                                              	
#define UICC_STATUS_APPL_ACTIVE                           	0x30                                              	
#define UICC_STATUS_APPL_NOT_ACTIVE                       	0x31                                              	
#define UICC_STATUS_PIN_ENABLED                           	0x40                                              	
#define UICC_STATUS_PIN_DISABLED                          	0x41                                              	

//ConstantTable for UICC_DETAILS_CONSTANTS
#define UICC_NO_DETAILS                                   	0x00                                                 	
#define UICC_INVALID_PARAMETERS                           	0x01                                                 	
#define UICC_FILE_NOT_FOUND                               	0x02                                                 	
#define UICC_SECURITY_CONDITIONS_NOT_SATISFIED            	0x03                                                 	
#define UICC_APPL_CONFLICT                                	0x04                                                 	
#define UICC_CARD_ERROR                                   	0x05                                                 	
#define UICC_SERVICE_NOT_SUPPORTED                        	0x06                                                 	
#define UICC_SESSION_EXPIRED                              	0x07                                                 	
#define UICC_PIN_INVALID                                  	0x08                                                 	
#define UICC_PIN_BLOCKED                                  	0x09                                                 	

//ConstantTable for UICC_APPL_TYPE_TABLE
#define UICC_APPL_TYPE_UNKNOWN                            	0x00                                                 	
#define UICC_APPL_TYPE_ICC_SIM                            	0x01                                                 	
#define UICC_APPL_TYPE_UICC_USIM                          	0x02                                                 	
#define UICC_APPL_TYPE_UICC_ISIM                          	0x03                                                 	

//ConstantTable for UICC_APPL_STATUS_TABLE

//ConstantTable for UICC_APPL_START_UP_TYPE_TABLE
#define UICC_APPL_START_UP_NO_INIT_PROC                   	0x00                                                 	
#define UICC_APPL_START_UP_INIT_PROC                      	0x01                                                 	

//ConstantTable for UICC_APDU_CMD_FORCE_TYPE_TABLE
#define UICC_APDU_CMD_FORCE_NOT_USED                      	0x00                                                 	
#define UICC_APDU_CMD_FORCE                               	0x01                                                 	

//ConstantTable for UICC_CARD_TYPE_TABLE
#define UICC_CARD_TYPE_UNKNOWN                            	0x00                                                 	
#define UICC_CARD_TYPE_ICC                                	0x01                                                 	
#define UICC_CARD_TYPE_UICC                               	0x02                                                 	

//ConstantTable for UICC_PIN_STATUS_TABLE

//ConstantTable for UICC_PIN_QUALIFIER_TABLE
#define UICC_PIN_UNKNOWN                                  	0x00                                                 	
#define UICC_PIN_NEW                                      	0x01                                                 	
#define UICC_PIN_OLD                                      	0x02                                                 	

//ConstantTable for UICC_CHV_QUALIFIER_TABLE
#define UICC_CHV1                                         	0x01                                                 	
#define UICC_CHV2                                         	0x02                                                 	

//ConstantTable for UICC_PIN_PROMPT_VERIFY_TABLE

//ConstantTable for UICC_REFRESH_TYPE_CONSTANTS
#define UICC_REFRESH_NAA_INIT                             	0x00                                                 	
#define UICC_REFRESH_NAA_FILE_CHANGE                      	0x01                                                 	
#define UICC_REFRESH_NAA_INIT_FILE_CHANGE                 	0x02                                                 	
#define UICC_REFRESH_NAA_INIT_FULL_FILE_CHANGE            	0x03                                                 	
#define UICC_REFRESH_UICC_RESET                           	0x04                                                 	
#define UICC_REFRESH_NAA_APPLICATION_RESET                	0x05                                                 	
#define UICC_REFRESH_NAA_SESSION_RESET                    	0x06                                                 	

//ConstantTable for UICC_ACTION_CONSTANTS
#define UICC_CONTROL_COLD_RESET                           	0x00                                                 	
#define UICC_CONTROL_WARM_RESET                           	0x01                                                 	
#define UICC_CONTROL_CARD_ACTIVATE                        	0x02                                                 	
#define UICC_CONTROL_CARD_DEACTIVATE                      	0x03                                                 	

//ConstantTable for UICC_SERVICE_TYPE_CONSTANTS
#define UICC_APPL_LIST                                    	0x01                                                 	
#define UICC_APPL_HOST_ACTIVATE                           	0x03                                                 	
#define UICC_APPL_START_UP_COMPLETE                       	0x05                                                 	
#define UICC_APPL_SHUT_DOWN_INITIATED                     	0x06                                                 	
#define UICC_APPL_STATUS_GET                              	0x07                                                 	
#define UICC_APPL_HOST_DEACTIVATE                         	0x09                                                 	
#define UICC_PIN_VERIFY                                   	0x11                                              	
#define UICC_PIN_UNBLOCK                                  	0x12                                              	
#define UICC_PIN_DISABLE                                  	0x13                                              	
#define UICC_PIN_ENABLE                                   	0x14                                              	
#define UICC_PIN_CHANGE                                   	0x15                                              	
#define UICC_PIN_SUBSTITUTE                               	0x16                                              	
#define UICC_PIN_INFO                                     	0x17                                              	
#define UICC_PIN_PROMPT_VERIFY                            	0x18                                              	
#define UICC_APPL_READ_TRANSPARENT                        	0x21                                              	
#define UICC_APPL_UPDATE_TRANSPARENT                      	0x22                                              	
#define UICC_APPL_READ_LINEAR_FIXED                       	0x23                                              	
#define UICC_APPL_UPDATE_LINEAR_FIXED                     	0x24                                              	
#define UICC_APPL_FILE_INFO                               	0x25                                              	
#define UICC_APPL_APDU_SEND                               	0x26                                              	
#define UICC_APPL_CLEAR_CACHE                             	0x27                                              	
#define UICC_APPL_SESSION_START                           	0x28                                              	
#define UICC_APPL_SESSION_END                             	0x29                                              	
#define UICC_APPL_CACHE_UPDATED                           	0x30                                              	
#define UICC_CONNECT                                      	0x31                                              	
#define UICC_DISCONNECT                                   	0x32                                              	
#define UICC_RECONNECT                                    	0x33                                              	
#define UICC_CAT_ENABLE                                   	0x41                                              	
#define UICC_CAT_DISABLE                                  	0x42                                              	
#define UICC_CAT_TERMINAL_PROFILE                         	0x43                                              	
#define UICC_CAT_TERMINAL_RESPONSE                        	0x44                                              	
#define UICC_CAT_ENVELOPE                                 	0x45                                              	
#define UICC_CAT_POLLING_SET                              	0x46                                              	
#define UICC_CAT_REFRESH                                  	0x47                                              	
#define UICC_APDU_SEND                                    	0x51                                              	
#define UICC_APDU_ATR_GET                                 	0x52                                              	
#define UICC_APDU_CONTROL                                 	0x53                                              	
#define UICC_REFRESH_STATUS                               	0x61                                              	
#define UICC_APPL_TERMINATED                              	0x71                                              	
#define UICC_APPL_RECOVERED                               	0x72                                              	
#define UICC_APPL_ACTIVATED                               	0x75                                              	
#define UICC_PIN_VERIFY_NEEDED                            	0x81                                              	
#define UICC_PIN_UNBLOCK_NEEDED                           	0x82                                              	
#define UICC_PIN_PERMANENTLY_BLOCKED                      	0x83                                              	
#define UICC_PIN_VERIFIED                                 	0x84                                              	
#define UICC_CAT_FETCHED_CMD                              	0x91                                              	
#define UICC_CAT_NOT_SUPPORTED                            	0x92                                              	
#define UICC_CAT_REG_FAILED                               	0x93                                              	
#define UICC_REFRESH_PERMISSION                           	0xA1                                              	
#define UICC_REFRESH_STARTING                             	0xA2                                              	
#define UICC_REFRESH_CANCELLED                            	0xA3                                              	
#define UICC_REFRESH_NOW                                  	0xA4                                              	
#define UICC_START_UP_COMPLETE                            	0xB0                                              	
#define UICC_STATUS_GET                                   	0xB1                                              	
#define UICC_READY                                        	0xB2                                              	
#define UICC_INITIALIZED                                  	0xB4                                              	
#define UICC_SHUTTING_DOWN                                	0xB5                                              	
#define UICC_CARD_DISCONNECTED                            	0xC0                                              	
#define UICC_CARD_REMOVED                                 	0xC1                                              	
#define UICC_CARD_NOT_PRESENT                             	0xC2                                              	
#define UICC_CARD_RESET                                   	0xC3                                              	
#define UICC_CARD_READY                                   	0xC4                                              	
#define UICC_CARD_STATUS_GET                              	0xC5                                              	
#define UICC_SIMLOCK_ACTIVE                               	0xD0                                              	

//ConstantTable for UICC_REFRESH_STATUS_TABLE
#define UICC_REFRESH_OK                                   	0x01                                                 	
#define UICC_REFRESH_NOT_OK                               	0x02                                                 	
#define UICC_REFRESH_DONE                                 	0x03                                                 	

//ConstantTable for UICC_SIMLOCK_STATUS_TABLE
#define UICC_SIMLOCK_STATUS_ACTIVE                        	0x01                                                 	
#define UICC_SIMLOCK_STATUS_INACTIVE                      	0x02                                                 	


//SUBBLOCKS


//Definition for UICC_SB_CARD_STATUS
#define UICC_SB_CARD_STATUS                               	0x01                                                 	
#define UICC_SB_CARD_STATUS_OFFSET_SBID                   	0 //size 2 byte(s)
#define UICC_SB_CARD_STATUS_OFFSET_SBLEN                  	2 //size 2 byte(s)
#define UICC_SB_CARD_STATUS_OFFSET_FILLERBYTE1            	4 //size 3 byte(s)
#define UICC_SB_CARD_STATUS_OFFSET_CARDSTATUS             	7 //size 1 byte(s)
#define SIZE_UICC_SB_CARD_STATUS                          	8


//Definition for UICC_SB_CLIENT
#define UICC_SB_CLIENT                                    	0x1F                                              	
#define UICC_SB_CLIENT_OFFSET_SBID                        	0 //size 2 byte(s)
#define UICC_SB_CLIENT_OFFSET_SBLEN                       	2 //size 2 byte(s)
#define UICC_SB_CLIENT_OFFSET_FILLERBYTE1                 	4 //size 3 byte(s)
#define UICC_SB_CLIENT_OFFSET_CLIENTID                    	7 //size 1 byte(s)
#define SIZE_UICC_SB_CLIENT                               	8


//Definition for UICC_SB_APPL_DATA_OBJECT
#define UICC_SB_APPL_DATA_OBJECT                          	0x02                                                 	
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_SBID              	0 //size 2 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_SBLEN             	2 //size 2 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_FILLERBYTE1       	4 //size 2 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLICATIONTYPE   	6 //size 1 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLID            	7 //size 1 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLSTATUS        	8 //size 1 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLDOLEN         	9 //size 1 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_APPLDO            	10 //size 1 byte(s)
#define UICC_SB_APPL_DATA_OBJECT_OFFSET_FILLERBYTE2       	11 //size 1 byte(s)
#define SIZE_UICC_SB_APPL_DATA_OBJECT                     	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_APPLICATION
#define UICC_SB_APPLICATION                               	0x03                                                 	
#define UICC_SB_APPLICATION_OFFSET_SBID                   	0 //size 2 byte(s)
#define UICC_SB_APPLICATION_OFFSET_SBLEN                  	2 //size 2 byte(s)
#define UICC_SB_APPLICATION_OFFSET_FILLERBYTE1            	4 //size 2 byte(s)
#define UICC_SB_APPLICATION_OFFSET_APPLICATIONTYPE        	6 //size 1 byte(s)
#define UICC_SB_APPLICATION_OFFSET_APPLID                 	7 //size 1 byte(s)
#define SIZE_UICC_SB_APPLICATION                          	8


//Definition for UICC_SB_APPL_INFO
#define UICC_SB_APPL_INFO                                 	0x04                                                 	
#define UICC_SB_APPL_INFO_OFFSET_SBID                     	0 //size 2 byte(s)
#define UICC_SB_APPL_INFO_OFFSET_SBLEN                    	2 //size 2 byte(s)
#define UICC_SB_APPL_INFO_OFFSET_FILLERBYTE1              	4 //size 3 byte(s)
#define UICC_SB_APPL_INFO_OFFSET_STRATUPTYPE              	7 //size 1 byte(s)
#define SIZE_UICC_SB_APPL_INFO                            	8


//Definition for UICC_SB_APPL_STATUS
#define UICC_SB_APPL_STATUS                               	0x05                                                 	
#define UICC_SB_APPL_STATUS_OFFSET_SBID                   	0 //size 2 byte(s)
#define UICC_SB_APPL_STATUS_OFFSET_SBLEN                  	2 //size 2 byte(s)
#define UICC_SB_APPL_STATUS_OFFSET_FILLERBYTE1            	4 //size 3 byte(s)
#define UICC_SB_APPL_STATUS_OFFSET_APPLSTATUS             	7 //size 1 byte(s)
#define SIZE_UICC_SB_APPL_STATUS                          	8


//Definition for UICC_SB_FCI
#define UICC_SB_FCI                                       	0x1C                                              	
#define UICC_SB_FCI_OFFSET_SBID                           	0 //size 2 byte(s)
#define UICC_SB_FCI_OFFSET_SBLEN                          	2 //size 2 byte(s)
#define UICC_SB_FCI_OFFSET_FCILENGTH                      	4 //size 2 byte(s)
#define UICC_SB_FCI_OFFSET_CARDTYPE                       	6 //size 1 byte(s)
#define UICC_SB_FCI_OFFSET_FILLERBYTE1                    	7 //size 1 byte(s)
#define UICC_SB_FCI_OFFSET_FCI                            	8 //size 1 byte(s)
#define UICC_SB_FCI_OFFSET_FILLERBYTE2                    	9 //size 1 byte(s)
#define UICC_SB_FCI_OFFSET_FILLERBYTE3                    	10 //size 1 byte(s)
#define UICC_SB_FCI_OFFSET_FILLERBYTE4                    	11 //size 1 byte(s)
#define SIZE_UICC_SB_FCI                                  	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_CHV
#define UICC_SB_CHV                                       	0x1B                                              	
#define UICC_SB_CHV_OFFSET_SBID                           	0 //size 2 byte(s)
#define UICC_SB_CHV_OFFSET_SBLEN                          	2 //size 2 byte(s)
#define UICC_SB_CHV_OFFSET_CHVQUALIFIER                   	4 //size 1 byte(s)
#define UICC_SB_CHV_OFFSET_PINID                          	5 //size 1 byte(s)
#define UICC_SB_CHV_OFFSET_FILLERBYTE1                    	6 //size 2 byte(s)
#define SIZE_UICC_SB_CHV                                  	8


//Definition for UICC_SB_PIN
#define UICC_SB_PIN                                       	0x08                                                 	
#define UICC_SB_PIN_OFFSET_SBID                           	0 //size 2 byte(s)
#define UICC_SB_PIN_OFFSET_SBLEN                          	2 //size 2 byte(s)
#define UICC_SB_PIN_OFFSET_PINID                          	4 //size 1 byte(s)
#define UICC_SB_PIN_OFFSET_PINQUALIFIER                   	5 //size 1 byte(s)
#define UICC_SB_PIN_OFFSET_FILLERBYTE1                    	6 //size 1 byte(s)
#define UICC_SB_PIN_OFFSET_LENGTH                         	7 //size 1 byte(s)
#define UICC_SB_PIN_OFFSET_PINCODE                        	8 //size 1 byte(s)
#define UICC_SB_PIN_OFFSET_FILLERBYTE2                    	9 //size 1 byte(s)
#define UICC_SB_PIN_OFFSET_FILLERBYTE3                    	10 //size 1 byte(s)
#define UICC_SB_PIN_OFFSET_FILLERBYTE4                    	11 //size 1 byte(s)
#define SIZE_UICC_SB_PIN                                  	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for UICC_SB_PIN_REF
#define UICC_SB_PIN_REF                                   	0x09                                                 	
#define UICC_SB_PIN_REF_OFFSET_SBID                       	0 //size 2 byte(s)
#define UICC_SB_PIN_REF_OFFSET_SBLEN                      	2 //size 2 byte(s)
#define UICC_SB_PIN_REF_OFFSET_PINID                      	4 //size 1 byte(s)
#define UICC_SB_PIN_REF_OFFSET_FILLERBYTE1                	5 //size 3 byte(s)
#define SIZE_UICC_SB_PIN_REF                              	8


//Definition for UICC_SB_PUK
#define UICC_SB_PUK                                       	0x0A                                                 	
#define UICC_SB_PUK_OFFSET_SBID                           	0 //size 2 byte(s)
#define UICC_SB_PUK_OFFSET_SBLEN                          	2 //size 2 byte(s)
#define UICC_SB_PUK_OFFSET_PINID                          	4 //size 1 byte(s)
#define UICC_SB_PUK_OFFSET_FILLERBYTE1                    	5 //size 2 byte(s)
#define UICC_SB_PUK_OFFSET_PUKLENGTH                      	7 //size 1 byte(s)
#define UICC_SB_PUK_OFFSET_PUKCODE                        	8 //size 1 byte(s)
#define UICC_SB_PUK_OFFSET_FILLERBYTE2                    	9 //size 1 byte(s)
#define UICC_SB_PUK_OFFSET_FILLERBYTE3                    	10 //size 1 byte(s)
#define UICC_SB_PUK_OFFSET_FILLERBYTE4                    	11 //size 1 byte(s)
#define SIZE_UICC_SB_PUK                                  	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for UICC_SB_PIN_SUBST
#define UICC_SB_PIN_SUBST                                 	0x0B                                                 	
#define UICC_SB_PIN_SUBST_OFFSET_SBID                     	0 //size 2 byte(s)
#define UICC_SB_PIN_SUBST_OFFSET_SBLEN                    	2 //size 2 byte(s)
#define UICC_SB_PIN_SUBST_OFFSET_PINID1                   	4 //size 1 byte(s)
#define UICC_SB_PIN_SUBST_OFFSET_PINID2                   	5 //size 1 byte(s)
#define UICC_SB_PIN_SUBST_OFFSET_FILLERBYTE1              	6 //size 2 byte(s)
#define SIZE_UICC_SB_PIN_SUBST                            	8


//Definition for UICC_SB_PIN_INFO
#define UICC_SB_PIN_INFO                                  	0x0C                                                 	
#define UICC_SB_PIN_INFO_OFFSET_SBID                      	0 //size 2 byte(s)
#define UICC_SB_PIN_INFO_OFFSET_SBLEN                     	2 //size 2 byte(s)
#define UICC_SB_PIN_INFO_OFFSET_PINSTATUS                 	4 //size 1 byte(s)
#define UICC_SB_PIN_INFO_OFFSET_PINATT                    	5 //size 1 byte(s)
#define UICC_SB_PIN_INFO_OFFSET_PUKATT                    	6 //size 1 byte(s)
#define UICC_SB_PIN_INFO_OFFSET_FILLERBYTE1               	7 //size 1 byte(s)
#define SIZE_UICC_SB_PIN_INFO                             	8


//Definition for UICC_SB_APPL_PATH
#define UICC_SB_APPL_PATH                                 	0x0D                                                 	
#define UICC_SB_APPL_PATH_OFFSET_SBID                     	0 //size 2 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_SBLEN                    	2 //size 2 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_EF                       	4 //size 2 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_SFI                      	6 //size 1 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_FILLERBYTE1              	7 //size 1 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_PATHLENGTH               	8 //size 1 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_FILLERBYTE2              	9 //size 1 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_PATH                     	10 //size 1 byte(s)
#define UICC_SB_APPL_PATH_OFFSET_FILLERBYTE3              	11 //size 1 byte(s)
#define SIZE_UICC_SB_APPL_PATH                            	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_SESSION
#define UICC_SB_SESSION                                   	0x0E                                                 	
#define UICC_SB_SESSION_OFFSET_SBID                       	0 //size 2 byte(s)
#define UICC_SB_SESSION_OFFSET_SBLEN                      	2 //size 2 byte(s)
#define UICC_SB_SESSION_OFFSET_FILLERBYTE1                	4 //size 3 byte(s)
#define UICC_SB_SESSION_OFFSET_SESSIONID                  	7 //size 1 byte(s)
#define SIZE_UICC_SB_SESSION                              	8


//Definition for UICC_SB_FILE_DATA
#define UICC_SB_FILE_DATA                                 	0x0F                                                 	
#define UICC_SB_FILE_DATA_OFFSET_SBID                     	0 //size 2 byte(s)
#define UICC_SB_FILE_DATA_OFFSET_SBLEN                    	2 //size 2 byte(s)
#define UICC_SB_FILE_DATA_OFFSET_DATALENGTH               	4 //size 4 byte(s)
#define UICC_SB_FILE_DATA_OFFSET_DATA                     	8 //size 1 byte(s)
#define UICC_SB_FILE_DATA_OFFSET_FILLERBYTE1              	9 //size 1 byte(s)
#define UICC_SB_FILE_DATA_OFFSET_FILLERBYTE2              	10 //size 1 byte(s)
#define UICC_SB_FILE_DATA_OFFSET_FILLERBYTE3              	11 //size 1 byte(s)
#define SIZE_UICC_SB_FILE_DATA                            	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_APDU
#define UICC_SB_APDU                                      	0x14                                              	
#define UICC_SB_APDU_OFFSET_SBID                          	0 //size 2 byte(s)
#define UICC_SB_APDU_OFFSET_SBLEN                         	2 //size 2 byte(s)
#define UICC_SB_APDU_OFFSET_CMDFORCE                      	4 //size 1 byte(s)
#define UICC_SB_APDU_OFFSET_FILLERBYTE1                   	5 //size 1 byte(s)
#define UICC_SB_APDU_OFFSET_APDULENGTH                    	6 //size 2 byte(s)
#define UICC_SB_APDU_OFFSET_APDU                          	8 //size 1 byte(s)
#define UICC_SB_APDU_OFFSET_FILLERBYTE2                   	9 //size 1 byte(s)
#define UICC_SB_APDU_OFFSET_FILLERBYTE3                   	10 //size 1 byte(s)
#define UICC_SB_APDU_OFFSET_FILLERBYTE4                   	11 //size 1 byte(s)
#define SIZE_UICC_SB_APDU                                 	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_TRANSPARENT
#define UICC_SB_TRANSPARENT                               	0x12                                              	
#define UICC_SB_TRANSPARENT_OFFSET_SBID                   	0 //size 2 byte(s)
#define UICC_SB_TRANSPARENT_OFFSET_SBLEN                  	2 //size 2 byte(s)
#define UICC_SB_TRANSPARENT_OFFSET_OFFSET                 	4 //size 2 byte(s)
#define UICC_SB_TRANSPARENT_OFFSET_AMOUNT                 	6 //size 2 byte(s)
#define SIZE_UICC_SB_TRANSPARENT                          	8


//Definition for UICC_SB_LINEAR_FIXED
#define UICC_SB_LINEAR_FIXED                              	0x13                                              	
#define UICC_SB_LINEAR_FIXED_OFFSET_SBID                  	0 //size 2 byte(s)
#define UICC_SB_LINEAR_FIXED_OFFSET_SBLEN                 	2 //size 2 byte(s)
#define UICC_SB_LINEAR_FIXED_OFFSET_RECORD                	4 //size 1 byte(s)
#define UICC_SB_LINEAR_FIXED_OFFSET_OFFSET                	5 //size 1 byte(s)
#define UICC_SB_LINEAR_FIXED_OFFSET_AMOUNT                	6 //size 1 byte(s)
#define UICC_SB_LINEAR_FIXED_OFFSET_FILLERBYTE1           	7 //size 1 byte(s)
#define SIZE_UICC_SB_LINEAR_FIXED                         	8


//Definition for UICC_SB_TERMINAL_PROFILE
#define UICC_SB_TERMINAL_PROFILE                          	0x15                                              	
#define UICC_SB_TERMINAL_PROFILE_OFFSET_SBID              	0 //size 2 byte(s)
#define UICC_SB_TERMINAL_PROFILE_OFFSET_SBLEN             	2 //size 2 byte(s)
#define UICC_SB_TERMINAL_PROFILE_OFFSET_FILLERBYTE1       	4 //size 2 byte(s)
#define UICC_SB_TERMINAL_PROFILE_OFFSET_TPLENGTH          	6 //size 2 byte(s)
#define UICC_SB_TERMINAL_PROFILE_OFFSET_TP                	8 //size 1 byte(s)
#define UICC_SB_TERMINAL_PROFILE_OFFSET_FILLERBYTE2       	9 //size 1 byte(s)
#define UICC_SB_TERMINAL_PROFILE_OFFSET_FILLERBYTE3       	10 //size 1 byte(s)
#define UICC_SB_TERMINAL_PROFILE_OFFSET_FILLERBYTE4       	11 //size 1 byte(s)
#define SIZE_UICC_SB_TERMINAL_PROFILE                     	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_TERMINAL_SUPPORT_TABLE
#define UICC_SB_TERMINAL_SUPPORT_TABLE                    	0x1E                                              	
#define UICC_SB_TERMINAL_SUPPORT_TABLE_OFFSET_SBID        	0 //size 2 byte(s)
#define UICC_SB_TERMINAL_SUPPORT_TABLE_OFFSET_SBLEN       	2 //size 2 byte(s)
#define UICC_SB_TERMINAL_SUPPORT_TABLE_OFFSET_FILLERBYTE1 	4 //size 3 byte(s)
#define UICC_SB_TERMINAL_SUPPORT_TABLE_OFFSET_NSB         	7 //size 1 byte(s)
#define SIZE_UICC_SB_TERMINAL_SUPPORT_TABLE               	8


//Definition for UICC_SB_TERMINAL_RESPONSE
#define UICC_SB_TERMINAL_RESPONSE                         	0x1D                                              	
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_SBID             	0 //size 2 byte(s)
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_SBLEN            	2 //size 2 byte(s)
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_FILLERBYTE1      	4 //size 2 byte(s)
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_TRLENGTH         	6 //size 2 byte(s)
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_TR               	8 //size 1 byte(s)
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_FILLERBYTE2      	9 //size 1 byte(s)
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_FILLERBYTE3      	10 //size 1 byte(s)
#define UICC_SB_TERMINAL_RESPONSE_OFFSET_FILLERBYTE4      	11 //size 1 byte(s)
#define SIZE_UICC_SB_TERMINAL_RESPONSE                    	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_POLLING_SET
#define UICC_SB_POLLING_SET                               	0x16                                              	
#define UICC_SB_POLLING_SET_OFFSET_SBID                   	0 //size 2 byte(s)
#define UICC_SB_POLLING_SET_OFFSET_SBLEN                  	2 //size 2 byte(s)
#define UICC_SB_POLLING_SET_OFFSET_POLLINT                	4 //size 2 byte(s)
#define UICC_SB_POLLING_SET_OFFSET_FILLERBYTE1            	6 //size 2 byte(s)
#define SIZE_UICC_SB_POLLING_SET                          	8


//Definition for UICC_SB_REFRESH
#define UICC_SB_REFRESH                                   	0x17                                              	
#define UICC_SB_REFRESH_OFFSET_SBID                       	0 //size 2 byte(s)
#define UICC_SB_REFRESH_OFFSET_SBLEN                      	2 //size 2 byte(s)
#define UICC_SB_REFRESH_OFFSET_TYPE                       	4 //size 1 byte(s)
#define UICC_SB_REFRESH_OFFSET_FILLERBYTE1                	5 //size 3 byte(s)
#define SIZE_UICC_SB_REFRESH                              	8


//Definition for UICC_SB_AID
#define UICC_SB_AID                                       	0x06                                                 	
#define UICC_SB_AID_OFFSET_SBID                           	0 //size 2 byte(s)
#define UICC_SB_AID_OFFSET_SBLEN                          	2 //size 2 byte(s)
#define UICC_SB_AID_OFFSET_STRLEN                         	4 //size 1 byte(s)
#define UICC_SB_AID_OFFSET_STR                            	5 //size 1 byte(s)
#define UICC_SB_AID_OFFSET_FILLERBYTE1                    	6 //size 1 byte(s)
#define UICC_SB_AID_OFFSET_FILLERBYTE2                    	7 //size 1 byte(s)
#define SIZE_UICC_SB_AID                                  	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for UICC_SB_REFRESH_RESULT
#define UICC_SB_REFRESH_RESULT                            	0x18                                              	
#define UICC_SB_REFRESH_RESULT_OFFSET_SBID                	0 //size 2 byte(s)
#define UICC_SB_REFRESH_RESULT_OFFSET_SBLEN               	2 //size 2 byte(s)
#define UICC_SB_REFRESH_RESULT_OFFSET_REFRESHSTATUS       	4 //size 1 byte(s)
#define UICC_SB_REFRESH_RESULT_OFFSET_FILLERBYTE1         	5 //size 3 byte(s)
#define SIZE_UICC_SB_REFRESH_RESULT                       	8


//Definition for UICC_SB_APDU_ACTIONS
#define UICC_SB_APDU_ACTIONS                              	0x19                                              	
#define UICC_SB_APDU_ACTIONS_OFFSET_SBID                  	0 //size 2 byte(s)
#define UICC_SB_APDU_ACTIONS_OFFSET_SBLEN                 	2 //size 2 byte(s)
#define UICC_SB_APDU_ACTIONS_OFFSET_ACTION                	4 //size 1 byte(s)
#define UICC_SB_APDU_ACTIONS_OFFSET_FILLERBYTE1           	5 //size 3 byte(s)
#define SIZE_UICC_SB_APDU_ACTIONS                         	8

#define UICC_SB_STATUS_WORD                                 0x0020
#define UICC_SB_STATUS_WORD_OFFSET_SW1                    	6 //size 1 byte(s)
#define UICC_SB_STATUS_WORD_OFFSET_SW2                    	7 //size 1 byte(s)

//Definition for UICC_SB_OBJECT_ID
#define UICC_SB_OBJECT_ID                                 	0x1A                                              	
#define UICC_SB_OBJECT_ID_OFFSET_SBID                     	0 //size 2 byte(s)
#define UICC_SB_OBJECT_ID_OFFSET_SBLEN                    	2 //size 2 byte(s)
#define UICC_SB_OBJECT_ID_OFFSET_OBJID                    	4 //size 2 byte(s)
#define UICC_SB_OBJECT_ID_OFFSET_DEVICEID                 	6 //size 1 byte(s)
#define UICC_SB_OBJECT_ID_OFFSET_FILLERBYTE1              	7 //size 1 byte(s)
#define SIZE_UICC_SB_OBJECT_ID                            	8


//MESSAGES


//Definition for UICC_REQ
#define UICC_REQ                                          	0x00                                                 	
#define UICC_REQ_OFFSET_TRANSID                           	0 //size 1 byte(s)
#define UICC_REQ_OFFSET_MESSAGEID                         	1 //size 1 byte(s)
#define UICC_REQ_OFFSET_SERVICETYPE                       	2 //size 1 byte(s)
#define SIZE_UICC_REQ                                     	3


//Definition for UICC_RESP
#define UICC_RESP                                         	0x01                                                 	
#define UICC_RESP_OFFSET_TRANSID                          	0 //size 1 byte(s)
#define UICC_RESP_OFFSET_MESSAGEID                        	1 //size 1 byte(s)
#define UICC_RESP_OFFSET_SERVICETYPE                      	2 //size 1 byte(s)
#define UICC_RESP_OFFSET_STATUS                           	3 //size 1 byte(s)
#define UICC_RESP_OFFSET_DETAILS                          	4 //size 1 byte(s)
#define UICC_RESP_OFFSET_SERVERSTATUS                     	5 //size 1 byte(s)
#define UICC_RESP_OFFSET_FILLERBYTE1                      	6 //size 2 byte(s)
#define SIZE_UICC_RESP                                    	8


//Definition for UICC_CARD_REQ
#define UICC_CARD_REQ                                     	0x03                                                 	
#define UICC_CARD_REQ_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define UICC_CARD_REQ_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define UICC_CARD_REQ_OFFSET_SERVICETYPE                  	2 //size 1 byte(s)
#define UICC_CARD_REQ_OFFSET_CARDTYPE                     	3 //size 1 byte(s)
#define SIZE_UICC_CARD_REQ                                	4


//Definition for UICC_CARD_RESP
#define UICC_CARD_RESP                                    	0x04                                                 	
#define UICC_CARD_RESP_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define UICC_CARD_RESP_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define UICC_CARD_RESP_OFFSET_SERVICETYPE                 	2 //size 1 byte(s)
#define UICC_CARD_RESP_OFFSET_STATUS                      	3 //size 1 byte(s)
#define UICC_CARD_RESP_OFFSET_DETAILS                     	4 //size 1 byte(s)
#define UICC_CARD_RESP_OFFSET_FILLERBYTE1                 	5 //size 1 byte(s)
#define UICC_CARD_RESP_OFFSET_CARDTYPE                    	6 //size 1 byte(s)
#define UICC_CARD_RESP_OFFSET_NSB                         	7 //size 1 byte(s)
#define SIZE_UICC_CARD_RESP                               	8


//Definition for UICC_APPLICATION_REQ
#define UICC_APPLICATION_REQ                              	0x06                                                 	
#define UICC_APPLICATION_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define UICC_APPLICATION_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define UICC_APPLICATION_REQ_OFFSET_SERVICETYPE           	2 //size 1 byte(s)
#define UICC_APPLICATION_REQ_OFFSET_NSB                   	3 //size 1 byte(s)
#define SIZE_UICC_APPLICATION_REQ                         	4


//Definition for UICC_APPLICATION_RESP
#define UICC_APPLICATION_RESP                             	0x07                                                 	
#define UICC_APPLICATION_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define UICC_APPLICATION_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define UICC_APPLICATION_RESP_OFFSET_SERVICETYPE          	2 //size 1 byte(s)
#define UICC_APPLICATION_RESP_OFFSET_STATUS               	3 //size 1 byte(s)
#define UICC_APPLICATION_RESP_OFFSET_DETAILS              	4 //size 1 byte(s)
#define UICC_APPLICATION_RESP_OFFSET_FILLERBYTE1          	5 //size 1 byte(s)
#define UICC_APPLICATION_RESP_OFFSET_CARDTYPE             	6 //size 1 byte(s)
#define UICC_APPLICATION_RESP_OFFSET_NSB                  	7 //size 1 byte(s)
#define SIZE_UICC_APPLICATION_RESP                        	8


//Definition for UICC_PIN_REQ
#define UICC_PIN_REQ                                      	0x09                                                 	
#define UICC_PIN_REQ_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define UICC_PIN_REQ_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define UICC_PIN_REQ_OFFSET_SERVICETYPE                   	2 //size 1 byte(s)
#define UICC_PIN_REQ_OFFSET_APPLID                        	3 //size 1 byte(s)
#define UICC_PIN_REQ_OFFSET_FILLERBYTE1                   	4 //size 3 byte(s)
#define UICC_PIN_REQ_OFFSET_NSB                           	7 //size 1 byte(s)
#define SIZE_UICC_PIN_REQ                                 	8


//Definition for UICC_PIN_RESP
#define UICC_PIN_RESP                                     	0x0A                                                 	
#define UICC_PIN_RESP_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define UICC_PIN_RESP_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define UICC_PIN_RESP_OFFSET_SERVICETYPE                  	2 //size 1 byte(s)
#define UICC_PIN_RESP_OFFSET_STATUS                       	3 //size 1 byte(s)
#define UICC_PIN_RESP_OFFSET_DETAILS                      	4 //size 1 byte(s)
#define UICC_PIN_RESP_OFFSET_FILLERBYTE1                  	5 //size 1 byte(s)
#define UICC_PIN_RESP_OFFSET_CARDTYPE                     	6 //size 1 byte(s)
#define UICC_PIN_RESP_OFFSET_NSB                          	7 //size 1 byte(s)
#define SIZE_UICC_PIN_RESP                                	8


//Definition for UICC_APPL_CMD_REQ
#define UICC_APPL_CMD_REQ                                 	0x0C                                                 	
#define UICC_APPL_CMD_REQ_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define UICC_APPL_CMD_REQ_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define UICC_APPL_CMD_REQ_OFFSET_SERVICETYPE              	2 //size 1 byte(s)
#define UICC_APPL_CMD_REQ_OFFSET_APPLID                   	3 //size 1 byte(s)
#define UICC_APPL_CMD_REQ_OFFSET_SESSIONID                	4 //size 1 byte(s)
#define UICC_APPL_CMD_REQ_OFFSET_FILLERBYTE1              	5 //size 2 byte(s)
#define UICC_APPL_CMD_REQ_OFFSET_NSB                      	7 //size 1 byte(s)
#define SIZE_UICC_APPL_CMD_REQ                            	8


//Definition for UICC_APPL_CMD_RESP
#define UICC_APPL_CMD_RESP                                	0x0D                                                 	
#define UICC_APPL_CMD_RESP_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define UICC_APPL_CMD_RESP_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define UICC_APPL_CMD_RESP_OFFSET_SERVICETYPE             	2 //size 1 byte(s)
#define UICC_APPL_CMD_RESP_OFFSET_STATUS                  	3 //size 1 byte(s)
#define UICC_APPL_CMD_RESP_OFFSET_DETAILS                 	4 //size 1 byte(s)
#define UICC_APPL_CMD_RESP_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define UICC_APPL_CMD_RESP_OFFSET_CARDTYPE                	6 //size 1 byte(s)
#define UICC_APPL_CMD_RESP_OFFSET_NSB                     	7 //size 1 byte(s)
#define SIZE_UICC_APPL_CMD_RESP                           	8


//Definition for UICC_CONNECTOR_REQ
#define UICC_CONNECTOR_REQ                                	0x0F                                                 	
#define UICC_CONNECTOR_REQ_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define UICC_CONNECTOR_REQ_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define UICC_CONNECTOR_REQ_OFFSET_SERVICETYPE             	2 //size 1 byte(s)
#define UICC_CONNECTOR_REQ_OFFSET_NSB                     	3 //size 1 byte(s)
#define SIZE_UICC_CONNECTOR_REQ                           	4


//Definition for UICC_CONNECTOR_RESP
#define UICC_CONNECTOR_RESP                               	0x10                                              	
#define UICC_CONNECTOR_RESP_OFFSET_TRANSID                	0 //size 1 byte(s)
#define UICC_CONNECTOR_RESP_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define UICC_CONNECTOR_RESP_OFFSET_SERVICETYPE            	2 //size 1 byte(s)
#define UICC_CONNECTOR_RESP_OFFSET_STATUS                 	3 //size 1 byte(s)
#define UICC_CONNECTOR_RESP_OFFSET_DETAILS                	4 //size 1 byte(s)
#define SIZE_UICC_CONNECTOR_RESP                          	5


//Definition for UICC_CAT_REQ
#define UICC_CAT_REQ                                      	0x12                                              	
#define UICC_CAT_REQ_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define UICC_CAT_REQ_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define UICC_CAT_REQ_OFFSET_SERVICETYPE                   	2 //size 1 byte(s)
#define UICC_CAT_REQ_OFFSET_NSB                           	3 //size 1 byte(s)
#define SIZE_UICC_CAT_REQ                                 	4


//Definition for UICC_CAT_RESP
#define UICC_CAT_RESP                                     	0x13                                              	
#define UICC_CAT_RESP_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define UICC_CAT_RESP_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define UICC_CAT_RESP_OFFSET_SERVICETYPE                  	2 //size 1 byte(s)
#define UICC_CAT_RESP_OFFSET_STATUS                       	3 //size 1 byte(s)
#define UICC_CAT_RESP_OFFSET_DETAILS                      	4 //size 1 byte(s)
#define UICC_CAT_RESP_OFFSET_FILLERBYTE1                  	5 //size 1 byte(s)
#define UICC_CAT_RESP_OFFSET_CARDTYPE                     	6 //size 1 byte(s)
#define UICC_CAT_RESP_OFFSET_NSB                          	7 //size 1 byte(s)
#define SIZE_UICC_CAT_RESP                                	8


//Definition for UICC_APDU_REQ
#define UICC_APDU_REQ                                     	0x15                                              	
#define UICC_APDU_REQ_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define UICC_APDU_REQ_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define UICC_APDU_REQ_OFFSET_SERVICETYPE                  	2 //size 1 byte(s)
#define UICC_APDU_REQ_OFFSET_NSB                          	3 //size 1 byte(s)
#define SIZE_UICC_APDU_REQ                                	4


//Definition for UICC_APDU_RESP
#define UICC_APDU_RESP                                    	0x16                                              	
#define UICC_APDU_RESP_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define UICC_APDU_RESP_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define UICC_APDU_RESP_OFFSET_SERVICETYPE                 	2 //size 1 byte(s)
#define UICC_APDU_RESP_OFFSET_STATUS                      	3 //size 1 byte(s)
#define UICC_APDU_RESP_OFFSET_DETAILS                     	4 //size 1 byte(s)
#define UICC_APDU_RESP_OFFSET_FILLERBYTE1                 	5 //size 2 byte(s)
#define UICC_APDU_RESP_OFFSET_NSB                         	7 //size 1 byte(s)
#define SIZE_UICC_APDU_RESP                               	8


//Definition for UICC_REFRESH_REQ
#define UICC_REFRESH_REQ                                  	0x18                                              	
#define UICC_REFRESH_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define UICC_REFRESH_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define UICC_REFRESH_REQ_OFFSET_SERVICETYPE               	2 //size 1 byte(s)
#define UICC_REFRESH_REQ_OFFSET_STATUS                    	3 //size 1 byte(s)
#define UICC_REFRESH_REQ_OFFSET_CLIENTID                  	4 //size 1 byte(s)
#define UICC_REFRESH_REQ_OFFSET_FILLERBYTE1               	5 //size 2 byte(s)
#define UICC_REFRESH_REQ_OFFSET_NSB                       	7 //size 1 byte(s)
#define SIZE_UICC_REFRESH_REQ                             	8


//Definition for UICC_REFRESH_RESP
#define UICC_REFRESH_RESP                                 	0x19                                              	
#define UICC_REFRESH_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define UICC_REFRESH_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define UICC_REFRESH_RESP_OFFSET_SERVICETYPE              	2 //size 1 byte(s)
#define UICC_REFRESH_RESP_OFFSET_STATUS                   	3 //size 1 byte(s)
#define UICC_REFRESH_RESP_OFFSET_DETAILS                  	4 //size 1 byte(s)
#define SIZE_UICC_REFRESH_RESP                            	5


//Definition for UICC_SIMLOCK_REQ
#define UICC_SIMLOCK_REQ                                  	0x1B                                              	
#define UICC_SIMLOCK_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define UICC_SIMLOCK_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define UICC_SIMLOCK_REQ_OFFSET_SERVICETYPE               	2 //size 1 byte(s)
#define SIZE_UICC_SIMLOCK_REQ                             	3


//Definition for UICC_SIMLOCK_RESP
#define UICC_SIMLOCK_RESP                                 	0x1C                                              	
#define UICC_SIMLOCK_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define UICC_SIMLOCK_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define UICC_SIMLOCK_RESP_OFFSET_SERVICETYPE              	2 //size 1 byte(s)
#define UICC_SIMLOCK_RESP_OFFSET_STATUS                   	3 //size 1 byte(s)
#define UICC_SIMLOCK_RESP_OFFSET_DETAILS                  	4 //size 1 byte(s)
#define UICC_SIMLOCK_RESP_OFFSET_SIMLOCKSTATUS            	5 //size 1 byte(s)
#define SIZE_UICC_SIMLOCK_RESP                            	6


//Definition for UICC_APPLICATION_IND
#define UICC_APPLICATION_IND                              	0x08                                                 	
#define UICC_APPLICATION_IND_OFFSET_TRANSID               	0 //size 1 byte(s)
#define UICC_APPLICATION_IND_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define UICC_APPLICATION_IND_OFFSET_SERVICETYPE           	2 //size 1 byte(s)
#define UICC_APPLICATION_IND_OFFSET_APPLID                	3 //size 1 byte(s)
#define UICC_APPLICATION_IND_OFFSET_FILLERBYTE1           	4 //size 3 byte(s)
#define UICC_APPLICATION_IND_OFFSET_NSB                   	7 //size 1 byte(s)
#define SIZE_UICC_APPLICATION_IND                         	8


//Definition for UICC_REFRESH_IND
#define UICC_REFRESH_IND                                  	0x1A                                              	
#define UICC_REFRESH_IND_OFFSET_UTID                      	0 //size 1 byte(s)
#define UICC_REFRESH_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define UICC_REFRESH_IND_OFFSET_SERVICETYPE               	2 //size 1 byte(s)
#define UICC_REFRESH_IND_OFFSET_TYPE                      	3 //size 1 byte(s)
#define UICC_REFRESH_IND_OFFSET_APPLID                    	4 //size 1 byte(s)
#define UICC_REFRESH_IND_OFFSET_FILLERBYTE1               	5 //size 2 byte(s)
#define UICC_REFRESH_IND_OFFSET_NSB                       	7 //size 1 byte(s)
#define SIZE_UICC_REFRESH_IND                             	8


//Definition for UICC_IND
#define UICC_IND                                          	0x02                                                 	
#define UICC_IND_OFFSET_TRANSID                           	0 //size 1 byte(s)
#define UICC_IND_OFFSET_MESSAGEID                         	1 //size 1 byte(s)
#define UICC_IND_OFFSET_SERVICETYPE                       	2 //size 1 byte(s)
#define SIZE_UICC_IND                                     	3


//Definition for UICC_CARD_IND
#define UICC_CARD_IND                                     	0x05                                                 	
#define UICC_CARD_IND_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define UICC_CARD_IND_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define UICC_CARD_IND_OFFSET_SERVICETYPE                  	2 //size 1 byte(s)
#define UICC_CARD_IND_OFFSET_CARDTYPE                     	3 //size 1 byte(s)
#define SIZE_UICC_CARD_IND                                	4


//Definition for UICC_PIN_IND
#define UICC_PIN_IND                                      	0x0B                                                 	
#define UICC_PIN_IND_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define UICC_PIN_IND_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define UICC_PIN_IND_OFFSET_SERVICETYPE                   	2 //size 1 byte(s)
#define UICC_PIN_IND_OFFSET_PINID                         	3 //size 1 byte(s)
#define UICC_PIN_IND_OFFSET_APPLID                        	4 //size 1 byte(s)
#define UICC_PIN_IND_OFFSET_FILLERBYTE1                   	5 //size 3 byte(s)
#define SIZE_UICC_PIN_IND                                 	8


//Definition for UICC_APPL_CMD_IND
#define UICC_APPL_CMD_IND                                 	0x0E                                                 	
#define UICC_APPL_CMD_IND_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define UICC_APPL_CMD_IND_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define UICC_APPL_CMD_IND_OFFSET_SERVICETYPE              	2 //size 1 byte(s)
#define UICC_APPL_CMD_IND_OFFSET_APPLID                   	3 //size 1 byte(s)
#define UICC_APPL_CMD_IND_OFFSET_FILLERBYTE1              	4 //size 3 byte(s)
#define UICC_APPL_CMD_IND_OFFSET_NSB                      	7 //size 1 byte(s)
#define SIZE_UICC_APPL_CMD_IND                            	8


//Definition for UICC_CAT_IND
#define UICC_CAT_IND                                      	0x14                                              	
#define UICC_CAT_IND_OFFSET_TRANSID                       	0 //size 1 byte(s)
#define UICC_CAT_IND_OFFSET_MESSAGEID                     	1 //size 1 byte(s)
#define UICC_CAT_IND_OFFSET_SERVICETYPE                   	2 //size 1 byte(s)
#define UICC_CAT_IND_OFFSET_CARDTYPE                      	3 //size 1 byte(s)
#define UICC_CAT_IND_OFFSET_APPLID                        	4 //size 1 byte(s)
#define UICC_CAT_IND_OFFSET_FILLERBYTE1                   	5 //size 2 byte(s)
#define UICC_CAT_IND_OFFSET_NSB                           	7 //size 1 byte(s)
#define SIZE_UICC_CAT_IND                                 	8


//Definition for UICC_APDU_RESET_IND
#define UICC_APDU_RESET_IND                               	0x17                                              	
#define UICC_APDU_RESET_IND_OFFSET_TRANSID                	0 //size 1 byte(s)
#define UICC_APDU_RESET_IND_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define UICC_APDU_RESET_IND_OFFSET_SERVICETYPE            	2 //size 1 byte(s)
#define UICC_APDU_RESET_IND_OFFSET_FILLERBYTE1            	3 //size 1 byte(s)
#define SIZE_UICC_APDU_RESET_IND                          	4

#endif