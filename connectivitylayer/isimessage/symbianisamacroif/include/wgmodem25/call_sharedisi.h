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
* Name      : PN_CALL [ 0x01 ] Resource Call Shared Subblock Descriptions
* Version   : 007.024
*
*/


#ifndef _SYMBIAN_ISI_CALL_SHAREDISI_H_
#define _SYMBIAN_ISI_CALL_SHAREDISI_H_

#define CALL_SHARED_SUBBLOCK_DESCRIPTIONS_ISI_VERSION_Z	 7
#define CALL_SHARED_SUBBLOCK_DESCRIPTIONS_ISI_VERSION_Y	 24


//CONSTANTS
#define CALL_NO_LINK_ID                                   	0x00                                                 	


//TABLES

//BitmaskTables for CALL_ADDRESS_TYPE
#define CALL_NBR_PLAN_UNKNOWN                             	0x00                                                 	
#define CALL_NBR_PLAN_ISDN_TELEPHONY                      	0x01                                                 	
#define CALL_NBR_PLAN_DATA                                	0x03                                                 	
#define CALL_NBR_PLAN_TELEX                               	0x04                                                 	
#define CALL_NBR_PLAN_PRIVATE                             	0x09                                                 	
#define CALL_GSM_NBR_PLAN_NATIONAL                        	0x08                                                 	
#define CALL_NBR_TYPE_UNKNOWN                             	0x00                                                 	
#define CALL_NBR_TYPE_INTERNATIONAL                       	0x10                                              	
#define CALL_NBR_TYPE_NATIONAL                            	0x20                                              	
#define CALL_NBR_TYPE_NETWORK_SPECIFIC                    	0x30                                              	
#define CALL_GSM_NBR_TYPE_DEDICATED                       	0x80                                              	

//ConstantTable for CALL_ADDRESS_SOURCE
#define CALL_NBR_FROM_NETWORK                             	0x01                                                 	
#define CALL_NBR_FROM_PHONEBOOK                           	0x02                                                 	

//BitmaskTables for CALL_PRESENTATION_SCREEN_INDICATOR
#define CALL_PRESENTATION_ALLOWED                         	0x00                                                 	
#define CALL_PRESENTATION_RESTRICTED                      	0x01                                                 	
#define CALL_PRESENTATION_UNAVAILABLE                     	0x02                                                 	
#define CALL_GSM_PRESENTATION_DEFAULT                     	0x07                                                 	
#define CALL_USER_NOT_SCREENED                            	0x00                                                 	
#define CALL_USER_SCREENED_PASSED                         	0x10                                              	
#define CALL_USER_SCREENED_FAILED                         	0x20                                              	
#define CALL_SCREEN_NETW_PROVIDED                         	0x30                                              	

//BitmaskTables for CALL_PRESENTATION_INDICATOR
//#define CALL_PRESENTATION_ALLOWED                       	0x00                   	 //Already defined as 0x00
//#define CALL_PRESENTATION_RESTRICTED                    	0x00                   	 //Already defined as 0x01
//#define CALL_PRESENTATION_UNAVAILABLE                   	0x00                   	 //Already defined as 0x02
//#define CALL_GSM_PRESENTATION_DEFAULT                   	0x00                   	 //Already defined as 0x07

//BitmaskTables for CALL_ADDITIONAL_ADDRESS_INFO
#define CALL_P_ADDR_PRESENTATION_ALLOWED                  	0x01                                                 	

//ConstantTable for CALL_MODE
#define CALL_MODE_EMERGENCY                               	0x00                                                 	
#define CALL_MODE_SPEECH                                  	0x01                                                 	
#define CALL_MODE_DATA                                    	0x02                                                 	
#define CALL_MODE_FAX                                     	0x03                                                 	
#define CALL_DAMPS_MODE_ANALOGDATA                        	0x80                                              	
#define CALL_DAMPS_MODE_DATA_DIRECT                       	0x81                                              	
#define CALL_GSM_MODE_ALT_SPEECH_FAX                      	0xA0                                              	
#define CALL_GSM_MODE_ALT_FAX_SPEECH                      	0xA1                                              	
#define CALL_GSM_MODE_ALS_LINE_2                          	0xA2                                              	
#define CALL_GSM_MODE_MULTIMEDIA                          	0xA3                                              	
#define CALL_GSM_MODE_ALT_MULTIMEDIA_SPEECH               	0xA4                                              	
#define CALL_GSM_MODE_ALS_LINE_1                          	0xA5                                              	

//BitmaskTables for CALL_MODE_INFO
#define CALL_MODE_ORIGINATOR                              	0x01                                                 	
#define CALL_GSM_NETWORK_INIT_MO_CALL                     	0x02                                                 	
#define CALL_LIMITED_CALL                                 	0x04                                                 	
#define CALL_SIM_ATK_CC_ADDR_CHANGED                      	0x20                                              	
#define CALL_GSM_CCP_CHANGED                              	0x40                                              	
#define CALL_MODE_SIM_CALL                                	0x80                                              	

//ConstantTable for CALL_UUS_TYPE
#define CALL_UUS_TYPE_UUS1                                	0x01                                                 	
#define CALL_UUS_TYPE_UUS2                                	0x02                                                 	
#define CALL_UUS_TYPE_UUS3                                	0x03                                                 	

//ConstantTable for CALL_UUS_ACTIVATION_TYPE
#define CALL_UUS_ACTIVATION_IMPLICIT                      	0x01                                                 	
#define CALL_UUS_ACTIVATION_EXPLICIT_REQ                  	0x02                                                 	
#define CALL_UUS_ACTIVATION_EXPLICIT_NOT_REQ              	0x03                                                 	


//SUBBLOCKS


//Definition for CALL_SHARED_DESTINATION_ADDRESS
#define CALL_SHARED_DESTINATION_ADDRESS                   	0xE0                                              	
#define CALL_SHARED_DESTINATION_ADDRESS_OFFSET_SUBBLOCKID 	0 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_ADDRESS_OFFSET_ADDRTYPE   	2 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_ADDRESS_OFFSET_PRESENTATION	3 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_ADDRESS_OFFSET_FILLERBYTE1	4 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_ADDRESS_OFFSET_ADDRLEN    	5 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_ADDRESS_OFFSET_ADDR       	6 //size 2 byte(s)
#define SIZE_CALL_SHARED_DESTINATION_ADDRESS              	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_SHARED_DESTINATION_PRE_ADDRESS
#define CALL_SHARED_DESTINATION_PRE_ADDRESS               	0xE1                                              	
#define CALL_SHARED_DESTINATION_PRE_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_PRE_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_PRE_ADDRESS_OFFSET_ADDRINFO	2 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_PRE_ADDRESS_OFFSET_ADDRLEN	3 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_PRE_ADDRESS_OFFSET_ADDR   	4 //size 2 byte(s)
#define CALL_SHARED_DESTINATION_PRE_ADDRESS_OFFSET_FILLERBYTE1	6 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_PRE_ADDRESS_OFFSET_FILLERBYTE2	7 //size 1 byte(s)
#define SIZE_CALL_SHARED_DESTINATION_PRE_ADDRESS          	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_SHARED_DESTINATION_POST_ADDRESS
#define CALL_SHARED_DESTINATION_POST_ADDRESS              	0xE2                                              	
#define CALL_SHARED_DESTINATION_POST_ADDRESS_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_POST_ADDRESS_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_POST_ADDRESS_OFFSET_ADDRINFO	2 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_POST_ADDRESS_OFFSET_ADDRLEN	3 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_POST_ADDRESS_OFFSET_ADDR  	4 //size 2 byte(s)
#define CALL_SHARED_DESTINATION_POST_ADDRESS_OFFSET_FILLERBYTE1	6 //size 1 byte(s)
#define CALL_SHARED_DESTINATION_POST_ADDRESS_OFFSET_FILLERBYTE2	7 //size 1 byte(s)
#define SIZE_CALL_SHARED_DESTINATION_POST_ADDRESS         	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_SHARED_MODE
#define CALL_SHARED_MODE                                  	0xE3                                              	
#define CALL_SHARED_MODE_OFFSET_SUBBLOCKID                	0 //size 1 byte(s)
#define CALL_SHARED_MODE_OFFSET_SUBBLOCKLEN               	1 //size 1 byte(s)
#define CALL_SHARED_MODE_OFFSET_MODE                      	2 //size 1 byte(s)
#define CALL_SHARED_MODE_OFFSET_MODEINFO                  	3 //size 1 byte(s)
#define SIZE_CALL_SHARED_MODE                             	4


//Definition for CALL_SHARED_ORIGIN_INFO
#define CALL_SHARED_ORIGIN_INFO                           	0xE4                                              	
#define CALL_SHARED_ORIGIN_INFO_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_SUBBLOCKLEN        	1 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_PRESENTATION       	2 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_NBRSOURCE          	3 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_NBRTYPE            	4 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_NBRGROUP           	5 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_LINKID             	6 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_NAMELEN            	7 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_NAME               	8 //size 2 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_FILLERBYTE1        	10 //size 1 byte(s)
#define CALL_SHARED_ORIGIN_INFO_OFFSET_FILLERBYTE2        	11 //size 1 byte(s)
#define SIZE_CALL_SHARED_ORIGIN_INFO                      	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for CALL_SHARED_GSM_CCP
#define CALL_SHARED_GSM_CCP                               	0xE5                                              	
#define CALL_SHARED_GSM_CCP_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define CALL_SHARED_GSM_CCP_OFFSET_SUBBLOCKLEN            	1 //size 1 byte(s)
#define CALL_SHARED_GSM_CCP_OFFSET_BCLENGTH               	2 //size 1 byte(s)
#define CALL_SHARED_GSM_CCP_OFFSET_BCDATA                 	3 //size 1 byte(s)
#define SIZE_CALL_SHARED_GSM_CCP                          	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_SHARED_USER_TO_USER
#define CALL_SHARED_USER_TO_USER                          	0xE6                                              	
#define CALL_SHARED_USER_TO_USER_OFFSET_SUBBLOCKID        	0 //size 1 byte(s)
#define CALL_SHARED_USER_TO_USER_OFFSET_SUBBLOCKLEN       	1 //size 1 byte(s)
#define CALL_SHARED_USER_TO_USER_OFFSET_UUSTYPE           	2 //size 1 byte(s)
#define CALL_SHARED_USER_TO_USER_OFFSET_UUSACTTYPE        	3 //size 1 byte(s)
#define CALL_SHARED_USER_TO_USER_OFFSET_UTOULEN           	4 //size 1 byte(s)
#define CALL_SHARED_USER_TO_USER_OFFSET_UTOU              	5 //size 1 byte(s)
#define CALL_SHARED_USER_TO_USER_OFFSET_FILLERBYTE1       	6 //size 1 byte(s)
#define CALL_SHARED_USER_TO_USER_OFFSET_FILLERBYTE2       	7 //size 1 byte(s)
#define SIZE_CALL_SHARED_USER_TO_USER                     	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_SHARED_LLC
#define CALL_SHARED_LLC                                   	0xE7                                              	
#define CALL_SHARED_LLC_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define CALL_SHARED_LLC_OFFSET_SUBBLOCKLEN                	1 //size 1 byte(s)
#define CALL_SHARED_LLC_OFFSET_BCLENGTH                   	2 //size 1 byte(s)
#define CALL_SHARED_LLC_OFFSET_LLCDATA                    	3 //size 1 byte(s)
#define SIZE_CALL_SHARED_LLC                              	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_SHARED_HLC
#define CALL_SHARED_HLC                                   	0xE8                                              	
#define CALL_SHARED_HLC_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define CALL_SHARED_HLC_OFFSET_SUBBLOCKLEN                	1 //size 1 byte(s)
#define CALL_SHARED_HLC_OFFSET_BCLENGTH                   	2 //size 1 byte(s)
#define CALL_SHARED_HLC_OFFSET_HLCDATA                    	3 //size 1 byte(s)
#define SIZE_CALL_SHARED_HLC                              	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CALL_SHARED_PHONEBOOK_REF
#define CALL_SHARED_PHONEBOOK_REF                         	0xE9                                              	
#define CALL_SHARED_PHONEBOOK_REF_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define CALL_SHARED_PHONEBOOK_REF_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define CALL_SHARED_PHONEBOOK_REF_OFFSET_LINKID           	2 //size 1 byte(s)
#define CALL_SHARED_PHONEBOOK_REF_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define CALL_SHARED_PHONEBOOK_REF_OFFSET_PBREF            	4 //size 4 byte(s)
#define SIZE_CALL_SHARED_PHONEBOOK_REF                    	8

#endif